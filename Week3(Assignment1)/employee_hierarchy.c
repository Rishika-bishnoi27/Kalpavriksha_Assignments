
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

void freeEmployee(Emp *employee) {
    free(employee->name);
    free(employee->role);
    free(employee);
}

void clearTree(Emp *person, Emp *root) {
    Emp *child = person->child;

    while (child) {
        Emp *nextReport = child->sibling;
        clearTree(child, root);
        child = nextReport;
    }

    if (person != root)
        freeEmployee(person);
}

char *readText(const char *message) {
    size_t size = 16, length, start, end;
    char *text = malloc(size);

    if (!text) {
        puts("Out of memory.");
        return NULL;
    }

    while (1) {
        length = 0;
        printf("%s", message);
        fflush(stdout);

        int character;
        while ((character = getchar()) != '\n' &&
               character != EOF) {
            if (length + 1 >= size) {
                if (size > SIZE_MAX / 2) {
                    free(text);
                    puts("Input too large.");
                    return NULL;
                }

                char *grown = realloc(text, size * 2);
                if (!grown) {
                    free(text);
                    puts("Out of memory.");
                    return NULL;
                }

                text = grown;
                size *= 2;
            }

            text[length++] = (char)character;
        }

        if (character == EOF && length == 0) {
            free(text);
            return NULL;
        }

        text[length] = '\0';

        start = 0;
        while (start < length &&
               isspace((unsigned char)text[start]))
            start++;

        end = length;
        while (end > start &&
               isspace((unsigned char)text[end - 1]))
            end--;

        if (start < end) {
            memmove(text, text + start, end - start);
            text[end - start] = '\0';
            return text;
        }

        if (character == EOF) {
            free(text);
            return NULL;
        }

        puts("Input cannot be empty.");
    }
}

int readNumber(const char *message, int lowest, int *valid) {
    while (1) {
        char *input = readText(message);

        if (!input) {
            *valid = 0;
            return 0;
        }

        char *end;
        errno = 0;
        long value = strtol(input, &end, 10);

        int invalid = end == input || *end != '\0' ||
                      errno == ERANGE ||
                      value > INT_MAX || value < INT_MIN;

        free(input);

        if (invalid)
            puts("Enter a valid whole number.");
        else if (value < lowest)
            printf("Number must be %d or more.\n", lowest);
        else {
            *valid = 1;
            return (int)value;
        }
    }
}

Emp *findById(Emp *start, int id) {
    if (start->id == id)
        return start;

    for (Emp *child = start->child; child; child = child->sibling) {
        Emp *found = findById(child, id);
        if (found)
            return found;
    }

    return NULL;
}

size_t countChildren(Emp *person) {
    size_t total = 0;

    for (Emp *child = person->child; child; child = child->sibling)
        total++;

    return total;
}

size_t countUnder(Emp *person) {
    size_t total = 0;

    for (Emp *child = person->child; child; child = child->sibling)
        total += 1 + countUnder(child);

    return total;
}

void showTree(Emp *person, int level) {
    printf("%*s%d - %s - %s\n",
           level * 4, "", person->id, person->name, person->role);

    for (Emp *child = person->child; child; child = child->sibling)
        showTree(child, level + 1);
}

void attach(Emp *boss, Emp *employee) {
    employee->boss = boss;
    employee->sibling = NULL;

    if (!boss->child) {
        boss->child = employee;
        return;
    }

    Emp *last = boss->child;
    while (last->sibling)
        last = last->sibling;

    last->sibling = employee;
}

void addEmployee(Emp *root) {
    int valid;
    int id = readNumber("Enter Employee ID: ", 1, &valid);
    if (!valid) return;

    if (findById(root, id)) {
        printf("Employee ID %d already exists.\n", id);
        return;
    }

    char *name = readText("Enter Name: ");
    if (!name) return;

    char *role = readText("Enter Designation: ");
    if (!role) {
        free(name);
        return;
    }

    int managerId = readNumber(
        "Enter Reporting Manager ID (0 for none): ", 0, &valid);

    if (!valid) {
        free(name);
        free(role);
        return;
    }

    Emp *boss = managerId == 0 ? root : findById(root, managerId);

    if (managerId == id) {
        puts("An employee cannot report to themselves.");
        free(name);
        free(role);
        return;
    }

    if (!boss) {
        printf("Manager ID %d does not exist.\n", managerId);
        free(name);
        free(role);
        return;
    }

    Emp *employee = calloc(1, sizeof *employee);

    if (!employee) {
        puts("Out of memory. Employee not added.");
        free(name);
        free(role);
        return;
    }

    employee->id = id;
    employee->name = name;
    employee->role = role;

    attach(boss, employee);
    printf("Employee %d added.\n", id);
}

void removeEmployee(Emp *root) {
    int valid;
    int id = readNumber("Enter Employee ID to remove: ", 1, &valid);
    if (!valid) return;

    Emp *employee = findById(root, id);

    if (!employee) {
        printf("Employee ID %d not found.\n", id);
        return;
    }

    if (employee->boss == root) {
        puts("Top-level employee cannot be removed.");
        return;
    }

    Emp *boss = employee->boss;
    Emp *heir = NULL;
    size_t heirChildCount = SIZE_MAX;

    for (Emp *sibling = boss->child; sibling;
         sibling = sibling->sibling) {
        if (sibling == employee)
            continue;

        size_t siblingChildCount = countChildren(sibling);

        if (!heir || siblingChildCount < heirChildCount) {
            heir = sibling;
            heirChildCount = siblingChildCount;
        }
    }

    if (!heir)
        heir = boss;

    Emp **employeeLink = &boss->child;

    while (*employeeLink != employee)
        employeeLink = &(*employeeLink)->sibling;

    *employeeLink = employee->sibling;

    size_t movedReports = countChildren(employee);
    Emp *child = employee->child;

    while (child) {
        Emp *nextReport = child->sibling;
        attach(heir, child);
        child = nextReport;
    }

    printf("Employee %d removed.", id);

    if (movedReports)
        printf(" %zu report(s) moved to %d.",
               movedReports, heir->id);

    puts("");
    freeEmployee(employee);
}

void showEmployee(Emp *root) {
    int valid;
    int id = readNumber("Enter Employee ID: ", 1, &valid);
    if (!valid) return;

    Emp *employee = findById(root, id);

    if (!employee) {
        printf("Employee ID %d not found.\n", id);
        return;
    }

    puts("");
    showTree(employee, 0);
    printf("\nTotal Employees Under %d: %zu\n",
           id, countUnder(employee));
}

void showAll(Emp *root) {
    puts("\n===== ORGANIZATION HIERARCHY =====\n");

    for (Emp *topEmployee = root->child;
         topEmployee; topEmployee = topEmployee->sibling)
        showTree(topEmployee, 0);

    printf("\nTotal Employees: %zu\n", countUnder(root));
}

int main(void) {
    /* Sentinel node with ID 0; top-level employees attach here. */
    Emp root = {0};

    while (1) {
        puts("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====");
        puts("1. Add Employee\n2. Remove Employee");
        puts("3. Display Employee Hierarchy");
        puts("4. Display Complete Hierarchy\n5. Exit");

        int valid;
        int choice = readNumber("Enter your choice: ", INT_MIN, &valid);

        if (!valid || choice == 5)
            break;
        else if (choice < 1 || choice > 5)
            puts("Invalid choice. Enter 1 to 5.");
        else if (choice > 1 && !root.child)
            puts("No employees in the organisation.");
        else if (choice == 1)
            addEmployee(&root);
        else if (choice == 2)
            removeEmployee(&root);
        else if (choice == 3)
            showEmployee(&root);
        else
            showAll(&root);
    }

    clearTree(&root, &root);
    return 0;
}
