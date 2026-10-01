#include <stdio.h>

struct User {
    int id;
    char name[50];
    int age;
};

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
    }else {
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

    while (fscanf(file, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3) {
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
    scanf("%d", &user.id);

    if (idExists(user.id)) {
        printf("ID already exists. Please use another ID.\n");
        return;
    }
    printf("Enter Name: ");
    scanf(" %49[^\n]", user.name);
    printf("Enter Age: ");
    scanf("%d", &user.age);

    file = fopen("users.txt", "a");

    if (file == NULL) {
        printf("File could not be opened.\n");
        return;
    }
    fprintf(file, "%d,%s,%d\n", user.id, user.name, user.age);
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

    while (fscanf(file, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3) {
        printf("%d\t%-20s%d\n", user.id, user.name, user.age);
    }
    fclose(file);
}

void updateUser() {
    FILE *file;
    FILE *temp;
    struct User user;
    int id;
    int found = 0;

    file = fopen("users.txt", "r");

    if (file == NULL) {
        printf("File not found.\n");
        return;
    }
    temp = fopen("temp.txt", "w");

    if (temp == NULL) {
        printf("Temporary file could not be created.\n");
        fclose(file);
        return;
    }
    printf("\nEnter ID to update: ");
    scanf("%d", &id);

    while (fscanf(file, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) {
            found = 1;
            printf("Enter new name: ");
            scanf(" %49[^\n]", user.name);
            printf("Enter new age: ");
            scanf("%d", &user.age);
        }

        fprintf(temp, "%d,%s,%d\n", user.id, user.name, user.age);
    }
    fclose(file);
    fclose(temp);
    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found) {
        printf("User updated successfully.\n");
    }else {
        printf("User ID not found.\n");
    }
}

void deleteUser() {
    FILE *file;
    FILE *temp;
    struct User user;
    int id;
    int found = 0;

    file = fopen("users.txt", "r");

    if (file == NULL) {
        printf("File not found.\n");
        return;
    }

    temp = fopen("temp.txt", "w");

    if (temp == NULL) {
        printf("Temporary file could not be created.\n");
        fclose(file);
        return;
    }
    printf("\nEnter ID to delete: ");
    scanf("%d", &id);

    while (fscanf(file, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) {
            found = 1;
            continue;
        }
        fprintf(temp, "%d,%s,%d\n", user.id, user.name, user.age);
    }
    fclose(file);
    fclose(temp);
    remove("users.txt");
    rename("temp.txt", "users.txt");
    if (found) {
        printf("User deleted successfully.\n");
    }else {
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
        scanf("%d", &choice);

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