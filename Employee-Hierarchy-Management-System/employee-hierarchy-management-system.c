#include <stdio.h>
#include <string.h>

#define MAX_EMPLOYEES 100

struct Employee
{
    int id;
    char name[50];
    char designation[50];
    int managerId;
};

struct Employee employees[MAX_EMPLOYEES];
int employeeCount = 0;


/* Find employee by ID */
int findEmployee(int id)
{
    for (int i = 0; i < employeeCount; i++)
    {
        if (employees[i].id == id)
        {
            return i;
        }
    }

    return -1;
}


/* Display hierarchy recursively */
void displayHierarchy(int managerId, int level)
{
    for (int i = 0; i < employeeCount; i++)
    {
        if (employees[i].managerId == managerId)
        {
            for (int j = 0; j < level; j++)
            {
                printf("    ");
            }

            printf("%d - %s - %s\n",
                   employees[i].id,
                   employees[i].name,
                   employees[i].designation);

            displayHierarchy(employees[i].id, level + 1);
        }
    }
}


/* Count all employees under a manager recursively */
int countReports(int managerId)
{
    int count = 0;

    for (int i = 0; i < employeeCount; i++)
    {
        if (employees[i].managerId == managerId)
        {
            count++;
            count += countReports(employees[i].id);
        }
    }

    return count;
}


/* Add employee */
void addEmployee()
{
    int id;
    int managerId;

    if (employeeCount >= MAX_EMPLOYEES)
    {
        printf("Employee limit reached.\n");
        return;
    }

    printf("Enter Employee ID: ");
    scanf("%d", &id);

    /* Check for duplicate ID */
    if (findEmployee(id) != -1)
    {
        printf("Employee ID already exists.\n");
        return;
    }

    printf("Enter Name: ");
    scanf("%s", employees[employeeCount].name);

    printf("Enter Designation: ");
    scanf("%s", employees[employeeCount].designation);

    printf("Enter Reporting Manager ID (0 for top-level employee): ");
    scanf("%d", &managerId);

    /* Check whether manager exists */
    if (managerId != 0 && findEmployee(managerId) == -1)
    {
        printf("Reporting manager does not exist.\n");
        return;
    }

    employees[employeeCount].id = id;
    employees[employeeCount].managerId = managerId;

    employeeCount++;

    printf("Employee added successfully.\n");
}


/* Remove employee */
void removeEmployee()
{
    int id;

    printf("Enter Employee ID to remove: ");
    scanf("%d", &id);

    int index = findEmployee(id);

    /* Employee does not exist */
    if (index == -1)
    {
        printf("Employee not found.\n");
        return;
    }

    int managerId = employees[index].managerId;

    /* CEO cannot be removed */
    if (managerId == 0)
    {
        printf("CEO cannot be removed.\n");
        return;
    }

    /*
       Find the sibling having the fewest
       direct reports.
    */
    int newManagerId = managerId;
    int minReports = MAX_EMPLOYEES + 1;
    int foundSibling = 0;

    for (int i = 0; i < employeeCount; i++)
    {
        /* Employee must have same manager = sibling */
        if (employees[i].managerId == managerId &&
            employees[i].id != id)
        {
            int directReports = 0;

            /* Count direct reports of this sibling */
            for (int j = 0; j < employeeCount; j++)
            {
                if (employees[j].managerId == employees[i].id)
                {
                    directReports++;
                }
            }

            /* Find sibling with minimum direct reports */
            if (directReports < minReports)
            {
                minReports = directReports;
                newManagerId = employees[i].id;
                foundSibling = 1;
            }
        }
    }

    /*
       If no sibling exists,
       newManagerId remains managerId.
    */

    /* Transfer direct reports */
    for (int i = 0; i < employeeCount; i++)
    {
        if (employees[i].managerId == id)
        {
            employees[i].managerId = newManagerId;
        }
    }

    /* Remove employee from array */
    for (int i = index; i < employeeCount - 1; i++)
    {
        employees[i] = employees[i + 1];
    }

    employeeCount--;

    printf("Employee removed successfully.\n");
}


/* Display hierarchy of one employee */
void displayEmployeeHierarchy()
{
    int id;

    printf("Enter Employee ID: ");
    scanf("%d", &id);

    int index = findEmployee(id);

    if (index == -1)
    {
        printf("Employee not found.\n");
        return;
    }

    printf("\n%d - %s - %s\n",
           employees[index].id,
           employees[index].name,
           employees[index].designation);

    displayHierarchy(id, 1);

    printf("\nTotal Employees Under %d: %d\n",
           id,
           countReports(id));
}


/* Display complete organization hierarchy */
void displayCompleteHierarchy()
{
    printf("\n====== ORGANIZATION HIERARCHY ======\n");

    for (int i = 0; i < employeeCount; i++)
    {
        /* Top-level employee */
        if (employees[i].managerId == 0)
        {
            printf("\n%d - %s - %s\n",
                   employees[i].id,
                   employees[i].name,
                   employees[i].designation);

            displayHierarchy(employees[i].id, 1);
        }
    }

    printf("\nTotal Employees: %d\n", employeeCount);
}


/* Main function */
int main()
{
    int choice;

    while (1)
    {
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                addEmployee();
                break;

            case 2:
                removeEmployee();
                break;

            case 3:
                displayEmployeeHierarchy();
                break;

            case 4:
                displayCompleteHierarchy();
                break;

            case 5:
                printf("Exiting program...\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}