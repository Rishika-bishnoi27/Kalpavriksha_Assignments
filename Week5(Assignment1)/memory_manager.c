#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

#define MIN_POOL 1
#define MAX_POOL 1000000
#define START_CAPACITY 8
#define LINE_SIZE 64
#define NOT_FOUND ((size_t)-1)
#define DEBUG_BYTE_MASK 0xFF

typedef enum { FIRST_FIT = 1, BEST_FIT = 2, WORST_FIT = 3 } Strategy;
typedef enum { INPUT_OK, INPUT_BAD, INPUT_EOF } InputState;

typedef enum {
    MENU_ALLOCATE = 1,
    MENU_RELEASE = 2,
    MENU_RESIZE = 3,
    MENU_DISPLAY_MAP = 4,
    MENU_DISPLAY_STATS = 5,
    MENU_CHANGE_STRATEGY = 6,
    MENU_EXIT = 7
} MenuOption;

typedef enum {
    OP_OK,
    OP_ERR_INVALID_SIZE,
    OP_ERR_NO_IDS,
    OP_ERR_NO_FIT,
    OP_ERR_NO_RECORD_MEM,
    OP_ERR_INVALID_ID,
    OP_ERR_ALREADY_RELEASED,
    OP_ERR_UNKNOWN_ID,
    OP_NO_CHANGE,
    OP_SHRUNK,
    OP_EXPANDED_IN_PLACE,
    OP_MOVED
} OpResult;

typedef struct {
    size_t start;
    size_t size;
    int id;
    bool is_free;
} Block;

typedef struct {
    unsigned char *pool;
    size_t pool_size;
    Block *blocks;
    size_t count;
    size_t capacity;
    int next_id;
    Strategy strategy;
} Manager;

typedef struct {
    size_t used_bytes;
    size_t free_bytes;
    size_t used_blocks;
    size_t free_blocks;
    size_t biggest_free;
} Stats;

static InputState read_number(const char *prompt, size_t *value)
{
    char line[LINE_SIZE];
    const char *cursor;
    char *end;
    unsigned long long parsed;
    size_t len;

    printf("%s", prompt);
    if (fgets(line, sizeof line, stdin) == NULL)
        return INPUT_EOF;

    len = strlen(line);
    if (len > 0 && line[len - 1] != '\n' && !feof(stdin)) {
        int letter;
        while ((letter = getchar()) != '\n' && letter != EOF) { }
        printf("Invalid input: line too long.\n");
        return INPUT_BAD;
    }

    cursor = line;
    while (isspace((unsigned char)*cursor))
        cursor++;

    errno = 0;
    if (!isdigit((unsigned char)*cursor))
        parsed = 0, errno = EINVAL;
    else
        parsed = strtoull(cursor, &end, 10);

    if (errno != 0 || parsed > SIZE_MAX) {
        printf("Invalid input: enter a whole non-negative number.\n");
        return INPUT_BAD;
    }

    while (isspace((unsigned char)*end))
        end++;
    if (*end != '\0') {
        printf("Invalid input: enter a whole non-negative number.\n");
        return INPUT_BAD;
    }

    *value = (size_t)parsed;
    return INPUT_OK;
}

static bool ask_strategy(Strategy *strategy)
{
    size_t choice;
    InputState state;

    while (1) {
        state = read_number(
            "Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ",
            &choice);
        if (state == INPUT_EOF)
            return false;
        if (state == INPUT_BAD)
            continue;
        if (choice < FIRST_FIT || choice > WORST_FIT) {
            printf("Please choose 1, 2 or 3.\n");
            continue;
        }
        *strategy = (Strategy)choice;
        return true;
    }
}

static const char *strategy_name(Strategy strategy)
{
    if (strategy == BEST_FIT)
        return "Best Fit";
    if (strategy == WORST_FIT)
        return "Worst Fit";
    return "First Fit";
}

static int init_manager(Manager *mgr, size_t pool_size)
{
    mgr->pool = malloc(pool_size);
    mgr->blocks = malloc(START_CAPACITY * sizeof(Block));
    if (mgr->pool == NULL || mgr->blocks == NULL) {
        free(mgr->pool);
        free(mgr->blocks);
        mgr->pool = NULL;
        mgr->blocks = NULL;
        return 0;
    }

    memset(mgr->pool, 0, pool_size);
    mgr->pool_size = pool_size;
    mgr->capacity = START_CAPACITY;
    mgr->count = 1;
    mgr->blocks[0].start = 0;
    mgr->blocks[0].size = pool_size;
    mgr->blocks[0].id = 0;
    mgr->blocks[0].is_free = true;
    mgr->next_id = 1;
    mgr->strategy = FIRST_FIT;
    return 1;
}

static void destroy_manager(Manager *mgr)
{
    free(mgr->pool);
    free(mgr->blocks);
    mgr->pool = NULL;
    mgr->blocks = NULL;
    mgr->count = 0;
    mgr->capacity = 0;
}

static int make_room(Manager *mgr, size_t extra)
{
    size_t needed = mgr->count + extra;
    size_t new_capacity;
    Block *grown;

    if (needed <= mgr->capacity)
        return 1;

    new_capacity = mgr->capacity * 2;
    if (new_capacity < needed)
        new_capacity = needed;
    if (new_capacity > SIZE_MAX / sizeof(Block))
        return 0;

    grown = realloc(mgr->blocks, new_capacity * sizeof(Block));
    if (grown == NULL)
        return 0;

    mgr->blocks = grown;
    mgr->capacity = new_capacity;
    return 1;
}

static void insert_block(Manager *mgr, size_t slot, Block block)
{
    memmove(&mgr->blocks[slot + 1], &mgr->blocks[slot],
            (mgr->count - slot) * sizeof(Block));
    mgr->blocks[slot] = block;
    mgr->count++;
}

static void remove_block(Manager *mgr, size_t slot)
{
    memmove(&mgr->blocks[slot], &mgr->blocks[slot + 1],
            (mgr->count - slot - 1) * sizeof(Block));
    mgr->count--;
}

static size_t find_first_fit(const Manager *mgr, size_t size)
{
    size_t slot;
    for (slot = 0; slot < mgr->count; slot++)
        if (mgr->blocks[slot].is_free && mgr->blocks[slot].size >= size)
            return slot;
    return NOT_FOUND;
}

static size_t find_best_fit(const Manager *mgr, size_t size)
{
    size_t slot;
    size_t best = NOT_FOUND;
    for (slot = 0; slot < mgr->count; slot++) {
        if (!mgr->blocks[slot].is_free || mgr->blocks[slot].size < size)
            continue;
        if (best == NOT_FOUND || mgr->blocks[slot].size < mgr->blocks[best].size)
            best = slot;
    }
    return best;
}

static size_t find_worst_fit(const Manager *mgr, size_t size)
{
    size_t slot;
    size_t worst = NOT_FOUND;
    for (slot = 0; slot < mgr->count; slot++) {
        if (!mgr->blocks[slot].is_free || mgr->blocks[slot].size < size)
            continue;
        if (worst == NOT_FOUND || mgr->blocks[slot].size > mgr->blocks[worst].size)
            worst = slot;
    }
    return worst;
}

static size_t find_free_block(const Manager *mgr, size_t size)
{
    if (mgr->strategy == BEST_FIT)
        return find_best_fit(mgr, size);
    if (mgr->strategy == WORST_FIT)
        return find_worst_fit(mgr, size);
    return find_first_fit(mgr, size);
}

static size_t find_by_id(const Manager *mgr, int id)
{
    size_t slot;
    for (slot = 0; slot < mgr->count; slot++)
        if (!mgr->blocks[slot].is_free && mgr->blocks[slot].id == id)
            return slot;
    return NOT_FOUND;
}

static OpResult find_allocation(const Manager *mgr, size_t id_value, size_t *out_slot)
{
    size_t slot;

    if (id_value == 0 || id_value > INT_MAX)
        return OP_ERR_INVALID_ID;

    slot = find_by_id(mgr, (int)id_value);
    if (slot == NOT_FOUND) {
        if (id_value < (size_t)mgr->next_id)
            return OP_ERR_ALREADY_RELEASED;
        return OP_ERR_UNKNOWN_ID;
    }

    *out_slot = slot;
    return OP_OK;
}

static void merge_free_blocks(Manager *mgr)
{
    size_t slot = 0;
    while (slot + 1 < mgr->count) {
        if (mgr->blocks[slot].is_free && mgr->blocks[slot + 1].is_free) {
            mgr->blocks[slot].size += mgr->blocks[slot + 1].size;
            remove_block(mgr, slot + 1);
        } else {
            slot++;
        }
    }
}

static void carve_block(Manager *mgr, size_t slot, size_t size, int id)
{
    size_t old_start = mgr->blocks[slot].start;
    size_t leftover = mgr->blocks[slot].size - size;
    Block rest;

    mgr->blocks[slot].size = size;
    mgr->blocks[slot].id = id;
    mgr->blocks[slot].is_free = false;

    if (leftover > 0) {
        rest.start = old_start + size;
        rest.size = leftover;
        rest.id = 0;
        rest.is_free = true;
        insert_block(mgr, slot + 1, rest);
    }
}

static void compute_stats(const Manager *mgr, Stats *stats)
{
    size_t slot;
    memset(stats, 0, sizeof *stats);
    for (slot = 0; slot < mgr->count; slot++) {
        const Block *blk = &mgr->blocks[slot];
        if (blk->is_free) {
            stats->free_bytes += blk->size;
            stats->free_blocks++;
            if (blk->size > stats->biggest_free)
                stats->biggest_free = blk->size;
        } else {
            stats->used_bytes += blk->size;
            stats->used_blocks++;
        }
    }
}

static void report_no_fit(const Manager *mgr, const char *action, size_t size)
{
    Stats stats;
    size_t slot;

    compute_stats(mgr, &stats);
    printf("%s failed: no contiguous free block is large enough.\n", action);

    if (stats.free_bytes >= size) {
        printf("External fragmentation: %zu bytes requested, %zu bytes free in total,\n"
               "but the largest free block is only %zu bytes. Free blocks:",
               size, stats.free_bytes, stats.biggest_free);
        for (slot = 0; slot < mgr->count; slot++)
            if (mgr->blocks[slot].is_free)
                printf(" %zu", mgr->blocks[slot].size);
        printf("\n");
    }
}

static OpResult allocate_memory(Manager *mgr, size_t size, int *out_id, size_t *out_start)
{
    size_t slot;

    if (size == 0 || size > mgr->pool_size)
        return OP_ERR_INVALID_SIZE;
    if (mgr->next_id == INT_MAX)
        return OP_ERR_NO_IDS;

    slot = find_free_block(mgr, size);
    if (slot == NOT_FOUND)
        return OP_ERR_NO_FIT;
    if (mgr->blocks[slot].size > size && !make_room(mgr, 1))
        return OP_ERR_NO_RECORD_MEM;

    carve_block(mgr, slot, size, mgr->next_id);
    /* Fill allocated bytes with the low byte of the ID to mark allocations for debugging */
    memset(mgr->pool + mgr->blocks[slot].start, mgr->next_id & DEBUG_BYTE_MASK, size);
    *out_id = mgr->next_id;
    *out_start = mgr->blocks[slot].start;
    mgr->next_id++;
    return OP_OK;
}

static OpResult release_memory(Manager *mgr, size_t id_value)
{
    size_t slot;
    OpResult res = find_allocation(mgr, id_value, &slot);

    if (res != OP_OK)
        return res;

    mgr->blocks[slot].is_free = true;
    mgr->blocks[slot].id = 0;
    merge_free_blocks(mgr);
    return OP_OK;
}

static OpResult shrink_block(Manager *mgr, size_t slot, size_t new_size)
{
    size_t cut = mgr->blocks[slot].size - new_size;
    Block rest;

    if (slot + 1 < mgr->count && mgr->blocks[slot + 1].is_free) {
        mgr->blocks[slot + 1].start -= cut;
        mgr->blocks[slot + 1].size += cut;
        mgr->blocks[slot].size = new_size;
    } else {
        if (!make_room(mgr, 1))
            return OP_ERR_NO_RECORD_MEM;
        rest.start = mgr->blocks[slot].start + new_size;
        rest.size = cut;
        rest.id = 0;
        rest.is_free = true;
        mgr->blocks[slot].size = new_size;
        insert_block(mgr, slot + 1, rest);
    }
    merge_free_blocks(mgr);
    return OP_SHRUNK;
}

static OpResult grow_block(Manager *mgr, size_t slot, size_t new_size, size_t *out_start)
{
    size_t old_size = mgr->blocks[slot].size;
    size_t old_start = mgr->blocks[slot].start;
    size_t extra = new_size - old_size;
    int id = mgr->blocks[slot].id;
    size_t target;
    size_t target_start;

    if (slot + 1 < mgr->count && mgr->blocks[slot + 1].is_free
        && mgr->blocks[slot + 1].size >= extra) {
        mgr->blocks[slot].size = new_size;
        mgr->blocks[slot + 1].start += extra;
        mgr->blocks[slot + 1].size -= extra;
        /* Fill newly expanded bytes with the low byte of the ID for debugging */
        memset(mgr->pool + old_start + old_size, id & DEBUG_BYTE_MASK, extra);
        if (mgr->blocks[slot + 1].size == 0)
            remove_block(mgr, slot + 1);
        return OP_EXPANDED_IN_PLACE;
    }

    target = find_free_block(mgr, new_size);
    if (target == NOT_FOUND)
        return OP_ERR_NO_FIT;
    if (mgr->blocks[target].size > new_size && !make_room(mgr, 1))
        return OP_ERR_NO_RECORD_MEM;

    target_start = mgr->blocks[target].start;
    memmove(mgr->pool + target_start, mgr->pool + old_start, old_size);
    /* Fill newly expanded bytes after moving with the low byte of the ID for debugging */
    memset(mgr->pool + target_start + old_size, id & DEBUG_BYTE_MASK, extra);

    mgr->blocks[slot].is_free = true;
    mgr->blocks[slot].id = 0;
    carve_block(mgr, target, new_size, id);
    merge_free_blocks(mgr);
    *out_start = target_start;
    return OP_MOVED;
}

static OpResult resize_memory(Manager *mgr, size_t id_value, size_t new_size, size_t *out_start)
{
    size_t slot;
    OpResult res;

    if (new_size == 0 || new_size > mgr->pool_size)
        return OP_ERR_INVALID_SIZE;

    res = find_allocation(mgr, id_value, &slot);
    if (res != OP_OK)
        return res;

    if (new_size == mgr->blocks[slot].size)
        return OP_NO_CHANGE;
    if (new_size < mgr->blocks[slot].size)
        return shrink_block(mgr, slot, new_size);
    return grow_block(mgr, slot, new_size, out_start);
}

static void display_map(const Manager *mgr)
{
    size_t slot;
    printf("Memory Map (%zu bytes):\n", mgr->pool_size);
    for (slot = 0; slot < mgr->count; slot++) {
        const Block *blk = &mgr->blocks[slot];
        if (blk->is_free)
            printf("[Free: %zu bytes, Start: %zu]\n", blk->size, blk->start);
        else
            printf("[Allocated #%d: %zu bytes, Start: %zu]\n",
                   blk->id, blk->size, blk->start);
    }
}

static void display_stats(const Manager *mgr)
{
    Stats stats;
    compute_stats(mgr, &stats);
    printf("Total memory: %zu bytes\n", mgr->pool_size);
    printf("Allocated: %zu bytes (%zu blocks)\n", stats.used_bytes, stats.used_blocks);
    printf("Free: %zu bytes\n", stats.free_bytes);
    printf("Free blocks: %zu\n", stats.free_blocks);
    printf("Largest free block: %zu bytes\n", stats.biggest_free);
    if (stats.free_blocks > 1)
        printf("Note: free memory is split into %zu separate blocks.\n",
               stats.free_blocks);
}

static void print_menu(void)
{
    printf("\nMenu:\n"
           "1. Allocate memory\n"
           "2. Release memory\n"
           "3. Resize allocation\n"
           "4. Display memory map\n"
           "5. Display memory statistics\n"
           "6. Change allocation strategy\n"
           "7. Exit\n\n");
}

static void print_id_error(OpResult res, size_t id_value)
{
    if (res == OP_ERR_INVALID_ID)
        printf("Invalid ID: allocation IDs are positive numbers.\n");
    else if (res == OP_ERR_ALREADY_RELEASED)
        printf("ID %zu is already released.\n", id_value);
    else if (res == OP_ERR_UNKNOWN_ID)
        printf("Unknown allocation ID: %zu.\n", id_value);
}

static bool ask_and_allocate(Manager *mgr)
{
    size_t size;
    int id;
    size_t start;
    OpResult res;
    InputState state = read_number("Requested size: ", &size);

    if (state == INPUT_EOF)
        return false;
    if (state == INPUT_OK) {
        res = allocate_memory(mgr, size, &id, &start);
        if (res == OP_OK)
            printf("Allocation successful. ID: %d, Start: %zu, Size: %zu\n",
                   id, start, size);
        else if (res == OP_ERR_INVALID_SIZE)
            printf("Invalid size: must be between 1 and %zu.\n", mgr->pool_size);
        else if (res == OP_ERR_NO_IDS)
            printf("Allocation failed: no more allocation IDs available.\n");
        else if (res == OP_ERR_NO_FIT)
            report_no_fit(mgr, "Allocation", size);
        else if (res == OP_ERR_NO_RECORD_MEM)
            printf("Allocation failed: out of memory for block records.\n");
    }
    return true;
}

static bool ask_and_release(Manager *mgr)
{
    size_t id_value;
    OpResult res;
    InputState state = read_number("Allocation ID: ", &id_value);

    if (state == INPUT_EOF)
        return false;
    if (state == INPUT_OK) {
        res = release_memory(mgr, id_value);
        if (res == OP_OK)
            printf("Allocation released.\n");
        else
            print_id_error(res, id_value);
    }
    return true;
}

static bool ask_and_resize(Manager *mgr)
{
    size_t id_value;
    size_t new_size;
    size_t target_start;
    OpResult res;
    InputState state = read_number("Allocation ID: ", &id_value);

    if (state == INPUT_EOF)
        return false;
    if (state == INPUT_BAD)
        return true;

    state = read_number("New size: ", &new_size);
    if (state == INPUT_EOF)
        return false;
    if (state == INPUT_OK) {
        res = resize_memory(mgr, id_value, new_size, &target_start);
        if (res == OP_ERR_INVALID_SIZE)
            printf("Invalid size: must be between 1 and %zu.\n", mgr->pool_size);
        else if (res == OP_NO_CHANGE)
            printf("Allocation already has that size. Nothing changed.\n");
        else if (res == OP_SHRUNK)
            printf("Resize successful. Shrunk to %zu bytes.\n", new_size);
        else if (res == OP_EXPANDED_IN_PLACE)
            printf("Resize successful. Expanded in place to %zu bytes.\n", new_size);
        else if (res == OP_MOVED)
            printf("Resize successful. Moved to start %zu, new size %zu bytes.\n",
                   target_start, new_size);
        else if (res == OP_ERR_NO_FIT) {
            report_no_fit(mgr, "Resize", new_size);
            printf("The original allocation is unchanged.\n");
        } else if (res == OP_ERR_NO_RECORD_MEM)
            printf("Resize failed: out of memory for block records. Allocation unchanged.\n");
        else
            print_id_error(res, id_value);
    }
    return true;
}

int main(void)
{
    Manager mgr;
    size_t pool_size = 0;
    size_t choice;
    InputState state;
    bool running = true;

    while (1) {
        state = read_number("Enter memory pool size: ", &pool_size);
        if (state == INPUT_EOF)
            return 0;
        if (state == INPUT_BAD)
            continue;
        if (pool_size < MIN_POOL || pool_size > MAX_POOL) {
            printf("Pool size must be between %d and %d bytes.\n", MIN_POOL, MAX_POOL);
            continue;
        }
        break;
    }

    if (!init_manager(&mgr, pool_size)) {
        printf("Could not allocate memory for the pool.\n");
        return 1;
    }

    if (!ask_strategy(&mgr.strategy)) {
        destroy_manager(&mgr);
        return 0;
    }
    printf("Strategy set to %s.\n", strategy_name(mgr.strategy));

    while (running) {
        print_menu();
        state = read_number("Choice: ", &choice);
        if (state == INPUT_EOF)
            break;
        if (state == INPUT_BAD)
            continue;

        switch ((MenuOption)choice) {
        case MENU_ALLOCATE: running = ask_and_allocate(&mgr); break;
        case MENU_RELEASE: running = ask_and_release(&mgr); break;
        case MENU_RESIZE: running = ask_and_resize(&mgr); break;
        case MENU_DISPLAY_MAP: display_map(&mgr); break;
        case MENU_DISPLAY_STATS: display_stats(&mgr); break;
        case MENU_CHANGE_STRATEGY:
            running = ask_strategy(&mgr.strategy);
            if (running)
                printf("Strategy set to %s.\n", strategy_name(mgr.strategy));
            break;
        case MENU_EXIT: running = false; break;
        default: printf("Invalid choice: pick a number from 1 to 7.\n");
        }
    }

    destroy_manager(&mgr);
    printf("Goodbye.\n");
    return 0;
}