#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char exp[1000];
    int numbers[500];
    char operators[500];

    int n = 0, opCount = 0;
    int i = 0;

    printf("Enter expression: ");
    fgets(exp, sizeof(exp), stdin);

    exp[strcspn(exp, "\n")] = '\0';

    
    while (exp[i] != '\0') {

        if (isspace(exp[i])) {
            i++;
            continue;
        }

      
        if (isdigit(exp[i])) {
            int num = 0;

            while (isdigit(exp[i])) {
                num = num * 10 + (exp[i] - '0');
                i++;
            }

            numbers[n++] = num;
        }

        else if (exp[i] == '+' || exp[i] == '-' ||
                 exp[i] == '*' || exp[i] == '/') {

            operators[opCount++] = exp[i];
            i++;
        }

        else {
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }

    
    if (n == 0 || n != opCount + 1) {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    
    for (i = 0; i < opCount; i++) {

        if (operators[i] == '*' || operators[i] == '/') {

            if (operators[i] == '/' && numbers[i + 1] == 0) {
                printf("Error: Division by zero.\n");
                return 0;
            }

            if (operators[i] == '*')
                numbers[i] = numbers[i] * numbers[i + 1];
            else
                numbers[i] = numbers[i] / numbers[i + 1];

           
            for (int j = i + 1; j < n - 1; j++)
                numbers[j] = numbers[j + 1];

            for (int j = i; j < opCount - 1; j++)
                operators[j] = operators[j + 1];

            n--;
            opCount--;
            i--;
        }
    }

    
    int result = numbers[0];

    for (i = 0; i < opCount; i++) {

        if (operators[i] == '+')
            result += numbers[i + 1];

        else
            result -= numbers[i + 1];
    }

    printf("%d\n", result);

    return 0;
}