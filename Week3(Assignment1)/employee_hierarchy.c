
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>

typedef struct Emp {
    int id;
    char *name, *role;
    struct Emp *boss, *child, *sibling;
} Emp;

Emp root;

void freeEmployee(Emp *e) {
    free(e->name); free(e->role); free(e);
}

void clearTree(Emp *e) {
    Emp *c, *next;
    for (c = e->child; c; c = next) {
        next = c->sibling;
        clearTree(c);
    }
    if (e != &root) freeEmployee(e);
}

void quit(const char *msg, int status) {
    puts(msg); clearTree(&root); exit(status);
}

char *readText(const char *msg) {
    size_t size = 16, len, start, end;
    char *s = malloc(size), *p;
    int ch;
    if (!s) quit("Out of memory.", 1);

    while (1) {
        len = 0; printf("%s", msg); fflush(stdout);
        while ((ch = getchar()) != '\n' && ch != EOF) {
            if (len > size - 2) {
                if (size > SIZE_MAX / 2) {
                    free(s); quit("Input too large.", 1);
                }
                p = realloc(s, size * 2);
                if (!p) { free(s); quit("Out of memory.", 1); }
                s = p; size *= 2;
            }
            s[len++] = (char)ch;
        }

        if (ch == EOF && !len) {
            free(s); quit("\nInput ended. Exiting.", 0);
        }

        start = 0;
        while (start < len && isspace((unsigned char)s[start])) start++;
        end = len;
        while (end > start && isspace((unsigned char)s[end-1])) end--;

        if (end > start) {
            memmove(s, s + start, end - start);
            s[end - start] = '\0';
            return s;
        }
        if (ch == EOF) { free(s); quit("\nInput ended.", 0); }
        puts("Input cannot be empty.");
    }
}

int readNumber(const char *msg, int low) {
    char *s, *end;
    long n;
    int bad;
    while (1) {
        s = readText(msg); errno = 0;
        n = strtol(s, &end, 10);
        bad = end == s || *end || errno == ERANGE ||
              n > INT_MAX || n < INT_MIN;
        free(s);
        if (bad) puts("Enter a valid whole number.");
        else if (n < low) printf("Number must be %d or more.\n", low);
        else return (int)n;
    }
}

Emp *findById(Emp *e, int id) {
    if (e->id == id) return e;
    for (Emp *c = e->child; c; c = c->sibling) {
        Emp *found = findById(c, id);
        if (found) return found;
    }
    return NULL;
}

size_t countChildren(Emp *e) {
    size_t n = 0;
    for (e = e->child; e; e = e->sibling) n++;
    return n;
}

size_t countUnder(Emp *e) {
    size_t n = 0;
    for (Emp *c = e->child; c; c = c->sibling)
        n += 1 + countUnder(c);
    return n;
}

void showTree(Emp *e, int level) {
    printf("%*s%d - %s - %s\n", level * 4, "", e->id, e->name, e->role);
    for (Emp *c = e->child; c; c = c->sibling) showTree(c, level + 1);
}

void attach(Emp *boss, Emp *worker) {
    Emp *last = boss->child;
    worker->boss = boss; worker->sibling = NULL;
    if (!last) { boss->child = worker; return; }
    while (last->sibling) last = last->sibling;
    last->sibling = worker;
}

void addEmployee(void) {
    int id = readNumber("Enter Employee ID: ", 1);
    if (findById(&root, id)) {
        printf("Employee ID %d already exists.\n", id); return;
    }
    char *name = readText("Enter Name: "), *role = readText("Enter Designation: ");
    int mid = readNumber("Enter Reporting Manager ID (0 for none): ", 0);
    Emp *boss = mid == 0 ? &root : findById(&root, mid);
    Emp *e = NULL;

    if (mid == id) puts("An employee cannot report to themselves.");
    else if (!boss) printf("Manager ID %d does not exist.\n", mid);
    else if (!(e = calloc(1, sizeof *e))) puts("Out of memory.");

    if (!e) { free(name); free(role); return; }
    e->id = id; e->name = name; e->role = role;
    attach(boss, e);
    printf("Employee %d added.\n", id);
}

void removeEmployee(void) {
    int id = readNumber("Enter Employee ID to remove: ", 1);
    Emp *e = findById(&root, id), *heir = NULL, *s, *c, *next, **p;
    if (!e) { printf("Employee ID %d not found.\n", id); return; }
    if (e->boss == &root) { puts("Top-level employee (CEO) cannot be removed."); return; }

    for (s = e->boss->child; s; s = s->sibling)
        if (s != e && (!heir || countChildren(s) < countChildren(heir))) heir = s;
    if (!heir) heir = e->boss;

    for (p = &e->boss->child; *p != e; p = &(*p)->sibling);
    *p = e->sibling;

    size_t moved = countChildren(e);
    for (c = e->child; c; c = next) {
        next = c->sibling; attach(heir, c);
    }
    printf("Employee %d removed.", id);
    if (moved) printf(" %zu report(s) moved to %d.", moved, heir->id);
    puts(""); freeEmployee(e);
}

void showEmployee(void) {
    int id = readNumber("Enter Employee ID: ", 1);
    Emp *e = findById(&root, id);
    if (!e) { printf("Employee ID %d not found.\n", id); return; }
    puts(""); showTree(e, 0);
    printf("\nTotal Employees Under %d: %zu\n", id, countUnder(e));
}

void showAll(void) {
    puts("\n===== ORGANIZATION HIERARCHY =====\n");
    for (Emp *e = root.child; e; e = e->sibling) showTree(e, 0);
    printf("\nTotal Employees: %zu\n", countUnder(&root));
}

int main(void) {
    while (1) {
        puts("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n"
             "1. Add Employee\n2. Remove Employee\n"
             "3. Display Employee Hierarchy\n4. Display Complete Hierarchy\n5. Exit\n");
        int ch = readNumber("Enter your choice: ", INT_MIN);
        if (ch == 5) break;
        if (ch < 1 || ch > 5) puts("Invalid choice. Enter 1 to 5.");
        else if (ch > 1 && !root.child) puts("No employees in the organization.");
        else if (ch == 1) addEmployee();
        else if (ch == 2) removeEmployee();
        else if (ch == 3) showEmployee();
        else showAll();
    }
    clearTree(&root);
    return 0;
}
