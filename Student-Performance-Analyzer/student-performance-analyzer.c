
#include <stdio.h>

struct Student {
    int rollNumber;
    char name[50];
    int marks1;
    int marks2;
    int marks3;
};


int calculateTotal(struct Student s);
float calculateAverage(struct Student s);
char calculateGrade(float average);
void displayPerformance(char grade);
void printRollNumbers(struct Student students[], int n, int index);

int main() {

    int n;

    printf("Enter number of students: ");
    scanf("%d", &n);

    struct Student students[n];

  
    for (int i = 0; i < n; i++) {

        printf("\nEnter details for student %d:\n", i + 1);

        printf("Roll Number: ");
        scanf("%d", &students[i].rollNumber);

        printf("Name: ");
        scanf("%49s", students[i].name);

        do {
            printf("Marks 1 (0-100): ");
            scanf("%d", &students[i].marks1);

            if (students[i].marks1 < 0 || students[i].marks1 > 100) {
                printf("Invalid marks! Enter marks between 0 and 100.\n");
            }

        } while (students[i].marks1 < 0 || students[i].marks1 > 100);

        
        do {
            printf("Marks 2 (0-100): ");
            scanf("%d", &students[i].marks2);

            if (students[i].marks2 < 0 || students[i].marks2 > 100) {
                printf("Invalid marks! Enter marks between 0 and 100.\n");
            }

        } while (students[i].marks2 < 0 || students[i].marks2 > 100);

      
        do {
            printf("Marks 3 (0-100): ");
            scanf("%d", &students[i].marks3);

            if (students[i].marks3 < 0 || students[i].marks3 > 100) {
                printf("Invalid marks! Enter marks between 0 and 100.\n");
            }

        } while (students[i].marks3 < 0 || students[i].marks3 > 100);
    }

 
    for (int i = 0; i < n; i++) {

        int total = calculateTotal(students[i]);
        float average = calculateAverage(students[i]);
        char grade = calculateGrade(average);

        printf("\nRoll: %d\n", students[i].rollNumber);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);

        if (students[i].marks1 < 35 ||
            students[i].marks2 < 35 ||
            students[i].marks3 < 35) {

            printf("Status: Fail\n");

           
            continue;
        }

        printf("Performance: ");
        displayPerformance(grade);

        printf("Status: Pass\n");
    }

    printf("\nList of Roll Numbers (via recursion): ");

    printRollNumbers(students, n, 0);

    printf("\n");

    return 0;
}


int calculateTotal(struct Student s) {

    int total;

    total = s.marks1 + s.marks2 + s.marks3;

    return total;
}



float calculateAverage(struct Student s) {

    int total = calculateTotal(s);

    float average = total / 3.0;

    return average;
}



char calculateGrade(float average) {

    if (average >= 85) {
        return 'A';
    }
    else if (average >= 70) {
        return 'B';
    }
    else if (average >= 50) {
        return 'C';
    }
    else if (average >= 35) {
        return 'D';
    }
    else {
        return 'F';
    }
}


void displayPerformance(char grade) {

    switch (grade) {

        case 'A':
            printf("*****\n");
            break;

        case 'B':
            printf("****\n");
            break;

        case 'C':
            printf("***\n");
            break;

        case 'D':
            printf("**\n");
            break;

        case 'F':
            break;

        default:
            break;
    }
}


void printRollNumbers(struct Student students[], int n, int index) {

    if (index == n) {
        return;
    }

    printf("%d", students[index].rollNumber);

    if (index < n - 1) {
        printf(" ");
    }

    printRollNumbers(students, n, index + 1);
}
