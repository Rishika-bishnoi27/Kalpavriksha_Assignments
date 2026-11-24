#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define NAME_LEN 50
#define LINE_LEN 65536

#if NUM_BLOCKS < 1 || NUM_BLOCKS > 5000
#error NUM_BLOCKS must be between 1 and 5000
#endif

typedef enum { TYPE_FILE, TYPE_DIR } NodeType;

typedef struct FreeBlock {
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct FileNode {
    char name[NAME_LEN + 1];
    NodeType type;
    int size;
    int blockCount;
    int *blockPointers;
    struct FileNode *parent;
    struct FileNode *next;
    struct FileNode *prev;
    struct FileNode *child;
} FileNode;

static unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
static FreeBlock blockNodes[NUM_BLOCKS];
static char isFree[NUM_BLOCKS];
static FreeBlock *freeHead = NULL;
static FreeBlock *freeTail = NULL;
static int freeCount = 0;
static FileNode *root = NULL;
static FileNode *current = NULL;
static char seen[NUM_BLOCKS];
static char owned[NUM_BLOCKS];

static int addFreeBlock(int index) {
    if (index < 0 || index >= NUM_BLOCKS || isFree[index])
        return 0;
    FreeBlock *slot = &blockNodes[index];
    isFree[index] = 1;
    slot->index = index;
    slot->next = NULL;
    slot->prev = freeTail;
    if (freeTail != NULL)
        freeTail->next = slot;
    else
        freeHead = slot;
    freeTail = slot;
    freeCount++;
    return 1;
}

static int takeFreeBlock(void) {
    if (freeHead == NULL || freeHead->index < 0 || freeHead->index >= NUM_BLOCKS || !isFree[freeHead->index])
        return -1;
    FreeBlock *slot = freeHead;
    freeHead = slot->next;
    if (freeHead != NULL)
        freeHead->prev = NULL;
    else
        freeTail = NULL;
    slot->next = NULL;
    slot->prev = NULL;
    isFree[slot->index] = 0;
    freeCount--;
    return slot->index;
}

static int freeListValid(void) {
    int count = 0;
    FreeBlock *prev = NULL;
    for (FreeBlock *slot = freeHead; slot != NULL; slot = slot->next) {
        count++;
        if (count > NUM_BLOCKS || slot->prev != prev)
            return 0;
        if (slot->index < 0 || slot->index >= NUM_BLOCKS || !isFree[slot->index])
            return 0;
        prev = slot;
    }
    if (prev != freeTail || count != freeCount)
        return 0;
    int marked = 0;
    for (int block = 0; block < NUM_BLOCKS; block++) {
        if (isFree[block])
            marked++;
    }
    return marked == count;
}

static FileNode *newNode(const char *name, NodeType type, FileNode *parent) {
    FileNode *fresh = malloc(sizeof(FileNode));
    if (fresh == NULL)
        return NULL;
    strcpy(fresh->name, name);
    fresh->type = type;
    fresh->size = 0;
    fresh->blockCount = 0;
    fresh->blockPointers = NULL;
    fresh->parent = parent;
    fresh->next = fresh;
    fresh->prev = fresh;
    fresh->child = NULL;
    return fresh;
}

static void addChild(FileNode *folder, FileNode *node) {
    node->parent = folder;
    if (folder->child == NULL) {
        node->next = node;
        node->prev = node;
        folder->child = node;
        return;
    }
    FileNode *head = folder->child;
    FileNode *tail = head->prev;
    tail->next = node;
    node->prev = tail;
    node->next = head;
    head->prev = node;
}

static int removeChild(FileNode *folder, FileNode *node) {
    if (folder == NULL || node == NULL || node->parent != folder || folder->child == NULL)
        return 0;
    int found = 0;
    FileNode *entry = folder->child;
    do {
        if (entry == node) {
            found = 1;
            break;
        }
        entry = entry->next;
    } while (entry != folder->child);
    if (!found)
        return 0;
    if (node->next == node) {
        folder->child = NULL;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if (folder->child == node)
            folder->child = node->next;
    }
    node->next = node;
    node->prev = node;
    return 1;
}

static FileNode *findChild(FileNode *folder, const char *name) {
    if (folder == NULL || folder->type != TYPE_DIR || folder->child == NULL)
        return NULL;
    FileNode *entry = folder->child;
    do {
        if (strcmp(entry->name, name) == 0)
            return entry;
        entry = entry->next;
    } while (entry != folder->child);
    return NULL;
}

static int fileValid(const FileNode *file) {
    if (file->size < 0 || file->blockCount < 0 || file->blockCount > NUM_BLOCKS)
        return 0;
    int expected = file->size / BLOCK_SIZE + (file->size % BLOCK_SIZE != 0);
    if (file->blockCount != expected)
        return 0;
    if (file->blockCount > 0 && file->blockPointers == NULL)
        return 0;
    for (int block = 0; block < file->blockCount; block++) {
        if (file->blockPointers[block] < 0 || file->blockPointers[block] >= NUM_BLOCKS)
            return 0;
    }
    return 1;
}

static int blocksValid(const int *list, int count) {
    if (count < 0 || count > NUM_BLOCKS || (count > 0 && list == NULL))
        return 0;
    int ok = 1;
    int checked = 0;
    for (; checked < count; checked++) {
        int block = list[checked];
        if (block < 0 || block >= NUM_BLOCKS || isFree[block] || seen[block]) {
            ok = 0;
            break;
        }
        seen[block] = 1;
    }
    for (int undo = 0; undo < checked; undo++)
        seen[list[undo]] = 0;
    return ok;
}

static int ownershipValid(void) {
    memset(owned, 0, sizeof(owned));
    FileNode *walker = root->child;
    while (walker != NULL) {
        if (walker->type == TYPE_FILE) {
            if (!fileValid(walker))
                return 0;
            for (int block = 0; block < walker->blockCount; block++) {
                int number = walker->blockPointers[block];
                if (isFree[number] || owned[number])
                    return 0;
                owned[number] = 1;
            }
        } else if (walker->child != NULL) {
            walker = walker->child;
            continue;
        }
        while (walker != NULL) {
            FileNode *above = walker->parent;
            if (walker->next != above->child) {
                walker = walker->next;
                break;
            }
            walker = (above == root) ? NULL : above;
        }
    }
    for (int block = 0; block < NUM_BLOCKS; block++) {
        if (!isFree[block] && !owned[block])
            return 0;
    }
    return 1;
}

static int releaseBlocks(FileNode *file) {
    if (!blocksValid(file->blockPointers, file->blockCount))
        return 0;
    for (int block = 0; block < file->blockCount; block++)
        addFreeBlock(file->blockPointers[block]);
    free(file->blockPointers);
    file->blockPointers = NULL;
    file->blockCount = 0;
    file->size = 0;
    return 1;
}

static void freeTree(FileNode *start) {
    FileNode *walker = start;
    while (walker != NULL) {
        if (walker->child != NULL) {
            walker = walker->child;
        } else {
            FileNode *above = walker->parent;
            int atStart = (walker == start);
            if (!atStart && above != NULL && !removeChild(above, walker))
                return;
            free(walker->blockPointers);
            free(walker);
            walker = atStart ? NULL : above;
        }
    }
}

static int validName(const char *name) {
    size_t length = strlen(name);
    if (length == 0 || length > NAME_LEN) {
        printf("Error: Name must be 1 to %d characters.\n", NAME_LEN);
        return 0;
    }
    for (const char *letter = name; *letter != '\0'; letter++) {
        if (isspace((unsigned char)*letter)) {
            printf("Error: Name cannot contain spaces.\n");
            return 0;
        }
    }
    if (strchr(name, '/') != NULL || strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        printf("Error: Invalid name '%s'.\n", name);
        return 0;
    }
    return 1;
}

static char *buildPath(FileNode *node) {
    if (node == root) {
        char *slash = malloc(2);
        if (slash != NULL)
            strcpy(slash, "/");
        return slash;
    }
    size_t total = 0;
    for (FileNode *step = node; step->parent != NULL; step = step->parent) {
        size_t part = strlen(step->name) + 1;
        if (part > SIZE_MAX - total - 1)
            return NULL;
        total += part;
    }
    char *result = malloc(total + 1);
    if (result == NULL)
        return NULL;
    result[total] = '\0';
    size_t pos = total;
    for (FileNode *step = node; step->parent != NULL; step = step->parent) {
        size_t length = strlen(step->name);
        pos -= length;
        memcpy(result + pos, step->name, length);
        result[--pos] = '/';
    }
    return result;
}

static void showPath(void) {
    char *path = buildPath(current);
    if (path == NULL) {
        printf("Error: Out of memory.\n");
        return;
    }
    printf("%s\n", path);
    free(path);
}

static void makeNode(const char *name, NodeType type) {
    if (!validName(name))
        return;
    if (findChild(current, name) != NULL) {
        printf("Error: '%s' already exists.\n", name);
        return;
    }
    FileNode *fresh = newNode(name, type, current);
    if (fresh == NULL) {
        printf("Error: Out of memory.\n");
        return;
    }
    addChild(current, fresh);
    if (type == TYPE_DIR)
        printf("Directory '%s' created successfully.\n", name);
    else
        printf("File '%s' created successfully.\n", name);
}

static int unquote(char *text) {
    char quote = text[0];
    char *reader = text + 1;
    char *writer = text + 1;
    while (*reader != '\0' && *reader != quote) {
        if (*reader == '\\' && (reader[1] == quote || reader[1] == '\\'))
            reader++;
        *writer++ = *reader++;
    }
    if (*reader != quote)
        return 1;
    *writer = '\0';
    for (char *rest = reader + 1; *rest != '\0'; rest++) {
        if (!isspace((unsigned char)*rest))
            return 2;
    }
    return 0;
}

static void writeFile(const char *name, char *text) {
    FileNode *file = findChild(current, name);
    if (file == NULL) {
        printf("Error: File '%s' not found.\n", name);
        return;
    }
    if (file->type == TYPE_DIR) {
        printf("Error: '%s' is a directory.\n", name);
        return;
    }
    if (text[0] == '"' || text[0] == '\'') {
        int status = unquote(text);
        if (status == 1) {
            printf("Error: Missing closing quote.\n");
            return;
        }
        if (status == 2) {
            printf("Error: Unexpected text after closing quote.\n");
            return;
        }
        text++;
    }

    if (!fileValid(file) || !blocksValid(file->blockPointers, file->blockCount)) {
        printf("Error: File blocks are inconsistent. Write cancelled.\n");
        return;
    }

    int length = (int)strlen(text);
    int needed = (length + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int old = file->blockCount;
    int keep = needed < old ? needed : old;
    int extra = needed - keep;

    if (extra > freeCount) {
        printf("Error: Disk full. Write failed.\n");
        return;
    }

    int *fresh = NULL;
    if (needed > 0) {
        fresh = malloc(needed * sizeof(int));
        if (fresh == NULL) {
            printf("Error: Out of memory. File unchanged.\n");
            return;
        }
    }
    for (int block = 0; block < keep; block++)
        fresh[block] = file->blockPointers[block];
    for (int block = keep; block < needed; block++) {
        int taken = takeFreeBlock();
        if (taken < 0) {
            for (int undo = keep; undo < block; undo++)
                addFreeBlock(fresh[undo]);
            free(fresh);
            printf("Error: Block allocation failed. File unchanged.\n");
            return;
        }
        fresh[block] = taken;
    }

    for (int block = needed; block < old; block++) {
        if (!addFreeBlock(file->blockPointers[block]))
            printf("Error: Disk accounting problem with block %d.\n", file->blockPointers[block]);
    }
    free(file->blockPointers);

    for (int block = 0; block < needed; block++) {
        int start = block * BLOCK_SIZE;
        int chunk = length - start;
        if (chunk > BLOCK_SIZE)
            chunk = BLOCK_SIZE;
        memset(virtualDisk[fresh[block]], 0, BLOCK_SIZE);
        memcpy(virtualDisk[fresh[block]], text + start, chunk);
    }
    file->blockPointers = fresh;
    file->blockCount = needed;
    file->size = length;
    printf("Data written successfully (size=%d bytes).\n", length);
}

static void readFile(const char *name) {
    FileNode *file = findChild(current, name);
    if (file == NULL) {
        printf("Error: File '%s' not found.\n", name);
        return;
    }
    if (file->type == TYPE_DIR) {
        printf("Error: '%s' is a directory.\n", name);
        return;
    }
    if (!fileValid(file) || !blocksValid(file->blockPointers, file->blockCount)) {
        printf("Error: File metadata is corrupted.\n");
        return;
    }
    if (file->size == 0) {
        printf("(empty file)\n");
        return;
    }
    int left = file->size;
    for (int block = 0; block < file->blockCount; block++) {
        int chunk = left < BLOCK_SIZE ? left : BLOCK_SIZE;
        if (fwrite(virtualDisk[file->blockPointers[block]], 1, chunk, stdout) != (size_t)chunk) {
            printf("\nError: Failed to write output.\n");
            return;
        }
        left -= chunk;
    }
    printf("\n");
}

static void deleteFile(const char *name) {
    FileNode *file = findChild(current, name);
    if (file == NULL) {
        printf("Error: File '%s' not found.\n", name);
        return;
    }
    if (file->type == TYPE_DIR) {
        printf("Error: '%s' is a directory. Use rmdir.\n", name);
        return;
    }
    if (!removeChild(current, file)) {
        printf("Error: Directory list is corrupted.\n");
        return;
    }
    if (!releaseBlocks(file)) {
        addChild(current, file);
        printf("Error: File blocks are inconsistent. Delete cancelled.\n");
        return;
    }
    free(file);
    printf("File deleted successfully.\n");
}

static void removeDir(const char *name) {
    FileNode *folder = findChild(current, name);
    if (folder == NULL) {
        printf("Error: Directory '%s' not found.\n", name);
        return;
    }
    if (folder->type != TYPE_DIR) {
        printf("Error: '%s' is not a directory.\n", name);
        return;
    }
    if (folder->child != NULL) {
        printf("Error: Directory '%s' is not empty.\n", name);
        return;
    }
    if (!removeChild(current, folder)) {
        printf("Error: Directory list is corrupted.\n");
        return;
    }
    free(folder);
    printf("Directory removed successfully.\n");
}

static void listDir(void) {
    if (current->child == NULL) {
        printf("(empty)\n");
        return;
    }
    FileNode *entry = current->child;
    do {
        printf("%s%s\n", entry->name, entry->type == TYPE_DIR ? "/" : "");
        entry = entry->next;
    } while (entry != current->child);
}

static void changeDir(const char *name) {
    FileNode *target;
    if (strcmp(name, "..") == 0) {
        if (current == root) {
            printf("Error: Already at root.\n");
            return;
        }
        target = current->parent;
    } else if (strcmp(name, ".") == 0) {
        target = current;
    } else if (strcmp(name, "/") == 0) {
        target = root;
    } else {
        target = findChild(current, name);
        if (target == NULL) {
            printf("Error: Directory '%s' not found.\n", name);
            return;
        }
        if (target->type != TYPE_DIR) {
            printf("Error: '%s' is not a directory.\n", name);
            return;
        }
    }
    current = target;
    char *path = buildPath(current);
    if (path == NULL) {
        printf("Moved.\n");
        return;
    }
    printf("Moved to %s\n", path);
    free(path);
}

static void showDisk(void) {
    if (!freeListValid() || !ownershipValid()) {
        printf("Error: Free block list is inconsistent.\n");
        return;
    }
    int used = NUM_BLOCKS - freeCount;
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", used);
    printf("Free Blocks: %d\n", freeCount);
    printf("Disk Usage: %.2f%%\n", used * 100.0 / NUM_BLOCKS);
}

static char *nextWord(char **cursor) {
    while (isspace((unsigned char)**cursor))
        (*cursor)++;
    if (**cursor == '\0')
        return NULL;
    char *word = *cursor;
    while (**cursor && !isspace((unsigned char)**cursor))
        (*cursor)++;
    if (**cursor) {
        **cursor = '\0';
        (*cursor)++;
    }
    return word;
}

static void usage(const char *text) {
    printf("Usage: %s\n", text);
}

static void cleanExit(void) {
    freeTree(root);
    root = NULL;
    current = NULL;
    freeHead = NULL;
    freeTail = NULL;
    freeCount = 0;
    printf("Memory released. Exiting program...\n");
}

int main(void) {
    static char line[LINE_LEN];

    for (int block = 0; block < NUM_BLOCKS; block++)
        addFreeBlock(block);
    root = newNode("/", TYPE_DIR, NULL);
    if (root == NULL) {
        fprintf(stderr, "Fatal: out of memory.\n");
        return 1;
    }
    current = root;

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    while (1) {
        printf("%s > ", current == root ? "/" : current->name);
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            cleanExit();
            return 0;
        }

        size_t length = strlen(line);
        if (length == sizeof(line) - 1 && line[length - 1] != '\n') {
            int leftover = getchar();
            if (leftover != EOF && leftover != '\n') {
                while ((leftover = getchar()) != '\n' && leftover != EOF)
                    ;
                printf("Error: Input too long.\n");
                continue;
            }
        }
        while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r'))
            line[--length] = '\0';

        char *cursor = line;
        char *command = nextWord(&cursor);
        if (command == NULL)
            continue;
        char *operand = nextWord(&cursor);

        if (strcmp(command, "write") == 0) {
            while (isspace((unsigned char)*cursor))
                cursor++;
            if (operand == NULL || *cursor == '\0')
                usage("write <filename> \"text\"");
            else
                writeFile(operand, cursor);
            continue;
        }

        char *surplus = nextWord(&cursor);

        if (strcmp(command, "mkdir") == 0) {
            if (operand == NULL || surplus != NULL) usage("mkdir <name>");
            else makeNode(operand, TYPE_DIR);
        } else if (strcmp(command, "create") == 0) {
            if (operand == NULL || surplus != NULL) usage("create <filename>");
            else makeNode(operand, TYPE_FILE);
        } else if (strcmp(command, "read") == 0) {
            if (operand == NULL || surplus != NULL) usage("read <filename>");
            else readFile(operand);
        } else if (strcmp(command, "delete") == 0) {
            if (operand == NULL || surplus != NULL) usage("delete <filename>");
            else deleteFile(operand);
        } else if (strcmp(command, "rmdir") == 0) {
            if (operand == NULL || surplus != NULL) usage("rmdir <name>");
            else removeDir(operand);
        } else if (strcmp(command, "cd") == 0) {
            if (operand == NULL || surplus != NULL) usage("cd <name | .. | />");
            else changeDir(operand);
        } else if (strcmp(command, "ls") == 0) {
            if (operand != NULL) usage("ls");
            else listDir();
        } else if (strcmp(command, "pwd") == 0) {
            if (operand != NULL) usage("pwd");
            else showPath();
        } else if (strcmp(command, "df") == 0) {
            if (operand != NULL) usage("df");
            else showDisk();
        } else if (strcmp(command, "exit") == 0) {
            if (operand != NULL) usage("exit");
            else {
                cleanExit();
                return 0;
            }
        } else {
            printf("Error: Unknown command '%s'.\n", command);
        }
    }
}