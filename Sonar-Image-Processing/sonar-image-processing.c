#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 10

void generateMatrix(int *matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            *(matrix + i * n + j) = rand() % 256;
        }
    }
}

void displayMatrix(int *matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%4d", *(matrix + i * n + j));
        }
        printf("\n");
    }
}

void rotateClockwise(int *matrix, int n)
{
   
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            int *p1 = matrix + i * n + j;
            int *p2 = matrix + j * n + i;

            int temp = *p1;
            *p1 = *p2;
            *p2 = temp;
        }
    }

   
    for (int i = 0; i < n; i++)
    {
        int left = 0;
        int right = n - 1;

        while (left < right)
        {
            int *p1 = matrix + i * n + left;
            int *p2 = matrix + i * n + right;

            int temp = *p1;
            *p1 = *p2;
            *p2 = temp;

            left++;
            right--;
        }
    }
}

void smoothingFilter(int *matrix, int n)
{
    

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int sum = 0;
            int count = 0;

            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int row = i + di;
                    int col = j + dj;

                    if (row >= 0 && row < n &&
                        col >= 0 && col < n)
                    {
                        int value = *(matrix + row * n + col);

                       
                        value = value & 255;

                        sum += value;
                        count++;
                    }
                }
            }

            int average = sum / count;

            *(matrix + i * n + j) =
                (*(matrix + i * n + j) & 255) |
                (average << 8);
        }
    }

    
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            *(matrix + i * n + j) =
                *(matrix + i * n + j) >> 8;
        }
    }
}

int main()
{
    int n;

    srand((unsigned int)time(NULL));

    printf("Enter matrix size (2-10): ");
    scanf("%d", &n);

    if (n < 2 || n > 10)
    {
        printf("Invalid matrix size. Please enter a value between 2 and 10.\n");
        return 1;
    }

    int matrix[MAX_SIZE * MAX_SIZE];

    generateMatrix(matrix, n);

    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrix, n);

    rotateClockwise(matrix, n);

    printf("\nMatrix after 90 Degree Clockwise Rotation:\n");
    displayMatrix(matrix, n);

    smoothingFilter(matrix, n);

    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(matrix, n);

    return 0;
}
