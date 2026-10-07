#include <stdio.h>
#include <string.h>

struct User {
    int id;
    char name[50];
    int age;
};

void clearInput() {
    int character;

    while ((character = getchar()) != '\n' && character != EOF);
}

void createFile() {
    FILE *file;

    file = fopen("users.txt", "r");

    if (file == NULL) {
        file = fopen("users.txt", "w");

        if (file == NULL) {
            printf("users.txt could not be created.\n");
            return;
        }

        printf("users.txt created successfully.\n");
    }
    else {
        printf("users.txt already exists.\n");
    }

    fclose(file);
}

int idExists(int id) {
    FILE *file;
    struct User user;

    file = fopen("users.txt", "r");

    if (file == NULL) {
        return 0;
    }

    while (fscanf(file, "%d,%49[^,],%d",
                  &user.id, user.name, &user.age) == 3) {

        if (user.id == id) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

void addUser() {
    FILE *file;
    struct User user;

    printf("\nEnter ID: ");

    if (scanf("%d", &user.id) != 1) {
        printf("Error: Invalid ID.\n");
        clearInput();
        return;
    }

    if (user.id <= 0) {
        printf("Error: ID must be positive.\n");
        clearInput();
        return;
    }

    clearInput();

    if (idExists(user.id)) {
        printf("ID already exists. Please use another ID.\n");
        return;
    }

    printf("Enter Name: ");
    fgets(user.name, sizeof(user.name), stdin);

    user.name[strcspn(user.name, "\n")] = '\0';

    if (strlen(user.name) == 0) {
        printf("Error: Name cannot be empty.\n");
        return;
    }

    if (strchr(user.name, ',') != NULL) {
        printf("Error: Name cannot contain a comma.\n");
        return;
    }

    printf("Enter Age: ");

    if (scanf("%d", &user.age) != 1) {
        printf("Error: Invalid age.\n");
        clearInput();
        return;
    }

    if (user.age <= 0 || user.age > 120) {
        printf("Error: Age must be between 1 and 120.\n");
        clearInput();
        return;
    }

    file = fopen("users.txt", "a");

    if (file == NULL) {
        printf("File could not be opened.\n");
        return;
    }

    fprintf(file, "%d,%s,%d\n",
            user.id, user.name, user.age);

    fclose(file);

    printf("User added successfully.\n");
}

void readUsers() {
    FILE *file;
    struct User user;

    file = fopen("users.txt", "r");

    if (file == NULL) {
        printf("File not found.\n");
        return;
    }

    printf("\n=========== ALL USERS ===========\n");
    printf("ID\tName\t\t\tAge\n");
    printf("-----------------------------------\n");

    while (fscanf(file, "%d,%49[^,],%d",
                  &user.id, user.name, &user.age) == 3) {

        printf("%d\t%-20s%d\n",
               user.id, user.name, user.age);
    }

    fclose(file);
}

void updateUser() {
    FILE *file;
    FILE *temporary;
    struct User user;
    int id;
    int found = 0;

    file = fopen("users.txt", "r");

    if (file == NULL) {
        printf("File not found.\n");
        return;
    }

    temporary = fopen("temp.txt", "w");

    if (temporary == NULL) {
        printf("Temporary file could not be created.\n");
        fclose(file);
        return;
    }

    printf("\nEnter ID to update: ");

    if (scanf("%d", &id) != 1) {
        printf("Error: Invalid ID.\n");
        clearInput();
        fclose(file);
        fclose(temporary);
        remove("temp.txt");
        return;
    }

    clearInput();

    if (id <= 0) {
        printf("Error: ID must be positive.\n");
        fclose(file);
        fclose(temporary);
        remove("temp.txt");
        return;
    }

    while (fscanf(file, "%d,%49[^,],%d",
                  &user.id, user.name, &user.age) == 3) {

        if (user.id == id) {
            found = 1;

            printf("Enter new name: ");
            fgets(user.name, sizeof(user.name), stdin);

            user.name[strcspn(user.name, "\n")] = '\0';

            if (strlen(user.name) == 0 ||
                strchr(user.name, ',') != NULL) {

                printf("Error: Invalid name.\n");
                fclose(file);
                fclose(temporary);
                remove("temp.txt");
                return;
            }

            printf("Enter new age: ");

            if (scanf("%d", &user.age) != 1) {
                printf("Error: Invalid age.\n");
                clearInput();
                fclose(file);
                fclose(temporary);
                remove("temp.txt");
                return;
            }

            if (user.age <= 0 || user.age > 120) {
                printf("Error: Age must be between 1 and 120.\n");
                clearInput();
                fclose(file);
                fclose(temporary);
                remove("temp.txt");
                return;
            }

            clearInput();
        }

        fprintf(temporary, "%d,%s,%d\n",
                user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temporary);

    if (rename("temp.txt", "users.txt") != 0) {
        printf("Error: Could not update users.txt.\n");
        remove("temp.txt");
        return;
    }

    if (found) {
        printf("User updated successfully.\n");
    }
    else {
        printf("User ID not found.\n");
    }
}

void deleteUser() {
    FILE *file;
    FILE *temporary;
    struct User user;
    int id;
    int found = 0;

    file = fopen("users.txt", "r");

    if (file == NULL) {
        printf("File not found.\n");
        return;
    }

    temporary = fopen("temp.txt", "w");

    if (temporary == NULL) {
        printf("Temporary file could not be created.\n");
        fclose(file);
        return;
    }

    printf("\nEnter ID to delete: ");

    if (scanf("%d", &id) != 1) {
        printf("Error: Invalid ID.\n");
        clearInput();
        fclose(file);
        fclose(temporary);
        remove("temp.txt");
        return;
    }

    clearInput();

    if (id <= 0) {
        printf("Error: ID must be positive.\n");
        fclose(file);
        fclose(temporary);
        remove("temp.txt");
        return;
    }

    while (fscanf(file, "%d,%49[^,],%d",
                  &user.id, user.name, &user.age) == 3) {

        if (user.id == id) {
            found = 1;
            continue;
        }

        fprintf(temporary, "%d,%s,%d\n",
                user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temporary);

    if (rename("temp.txt", "users.txt") != 0) {
        printf("Error: Could not update users.txt.\n");
        remove("temp.txt");
        return;
    }

    if (found) {
        printf("User deleted successfully.\n");
    }
    else {
        printf("User ID not found.\n");
    }
}

int main() {
    int choice;

    createFile();

    while (1) {
        printf("\n========== USER MANAGEMENT ==========\n");
        printf("1. Add User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Error: Invalid choice.\n");
            clearInput();
            continue;
        }

        clearInput();

        switch (choice) {
            case 1:
                addUser();
                break;

            case 2:
                readUsers();
                break;

            case 3:
                updateUser();
                break;

            case 4:
                deleteUser();
                break;

            case 5:
                printf("Program ended.\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}