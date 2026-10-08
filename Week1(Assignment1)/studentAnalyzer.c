#include <stdio.h>

struct Student {
    int roll;
    char name[50];
    int mark1;
    int mark2;
    int mark3;
};

int totalMarks(struct Student student) {
    return student.mark1 + student.mark2 + student.mark3;
}

float averageMarks(struct Student student) {
    int total = totalMarks(student);
    return total / 3.0;
}

char getGrade(float average) {
    if (average >= 85)
        return 'A';
    else if (average >= 70)
        return 'B';
    else if (average >= 50)
        return 'C';
    else if (average >= 35)
        return 'D';
    else
        return 'F';
}

void printStars(char grade) {
    int count;

    if (grade == 'A')
        count = 5;
    else if (grade == 'B')
        count = 4;
    else if (grade == 'C')
        count = 3;
    else
        count = 2;

    for (int i = 0; i < count; i++)
        printf("*");
}

void printRolls(struct Student student[], int size, int index) {
    if (index == size)
        return;

    printf("%d", student[index].roll);

    if (index < size - 1)
        printf(" ");

    printRolls(student, size, index + 1);
}

int main() {
    int count;

    scanf("%d", &count);

    if (count < 1 || count > 100) {
        printf("Invalid number of students\n");
        return 0;
    }

    struct Student student[count];

    for (int i = 0; i < count; i++) {
        scanf("%d %s %d %d %d",
              &student[i].roll,
              student[i].name,
              &student[i].mark1,
              &student[i].mark2,
              &student[i].mark3);

        if (student[i].mark1 < 0 || student[i].mark1 > 100 ||
            student[i].mark2 < 0 || student[i].mark2 > 100 ||
            student[i].mark3 < 0 || student[i].mark3 > 100) {
            printf("Invalid marks for student %d\n", student[i].roll);
            return 0;
        }
    }

    for (int i = 0; i < count; i++) {
        int total = totalMarks(student[i]);
        float average = averageMarks(student[i]);
        char grade = getGrade(average);

        printf("Roll: %d\n", student[i].roll);
        printf("Name: %s\n", student[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);

        if (average < 35)
            continue;

        printf("Performance: ");
        printStars(grade);
        printf("\n");
    }

    printf("\nList of Roll Numbers (via recursion): ");
    printRolls(student, count, 0);
    printf("\n");

    return 0;
}