#include <stdio.h>
#include <string.h>

struct User {
    int id;
    char name[50];
    int age;
};

void createUser() {
    struct User u;
    FILE *file = fopen("users.txt", "a");

    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &u.id);

    printf("Enter Name: ");
    scanf(" %[^\n]", u.name);

    printf("Enter Age: ");
    scanf("%d", &u.age);

    fprintf(file, "%d %s %d\n", u.id, u.name, u.age);

    fclose(file);

    printf("User added successfully.\n");
}

void readUsers() {
    struct User u;
    FILE *file = fopen("users.txt", "r");

    if (file == NULL) {
        printf("No users found.\n");
        return;
    }

    printf("\nID\tName\tAge\n");
    printf("--------------------\n");

    while (fscanf(file, "%d %s %d", &u.id, u.name, &u.age) == 3) {
        printf("%d\t%s\t%d\n", u.id, u.name, u.age);
    }

    fclose(file);
}

void updateUser() {
    struct User u;
    int id, found = 0;

    FILE *file = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (file == NULL || temp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &id);

    while (fscanf(file, "%d %s %d", &u.id, u.name, &u.age) == 3) {

        if (u.id == id) {
            printf("Enter new name: ");
            scanf(" %[^\n]", u.name);

            printf("Enter new age: ");
            scanf("%d", &u.age);

            found = 1;
        }

        fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
    }

    fclose(file);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found)
        printf("User updated successfully.\n");
    else
        printf("User not found.\n");
}

void deleteUser() {
    struct User u;
    int id, found = 0;

    FILE *file = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (file == NULL || temp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter ID to delete: ");
    scanf("%d", &id);

    while (fscanf(file, "%d %s %d", &u.id, u.name, &u.age) == 3) {

        if (u.id == id) {
            found = 1;
            continue;
        }

        fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
    }

    fclose(file);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found)
        printf("User deleted successfully.\n");
    else
        printf("User not found.\n");
}

int main() {
    int choice;

    while (1) {
        printf("\n===== USER MANAGEMENT =====\n");
        printf("1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createUser();
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
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }
}