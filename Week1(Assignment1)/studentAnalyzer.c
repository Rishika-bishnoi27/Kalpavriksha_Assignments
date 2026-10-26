
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

    for (int star = 0; star < count; star++)
        printf("*");
}

void printRolls(struct Student student[], int size, int position) {
    if (position == size)
        return;

    printf("%d", student[position].roll);

    if (position < size - 1)
        printf(" ");

    printRolls(student, size, position + 1);
}

int main() {
    int count;

    scanf("%d", &count);

    if (count < 1 || count > 100) {
        printf("Invalid number of students\n");
        return 0;
    }

    struct Student student[count];

    for (int studentno = 0; studentno < count; studentno++) {
        scanf("%d %s %d %d %d",
              &student[studentno].roll,
              student[studentno].name,
              &student[studentno].mark1,
              &student[studentno].mark2,
              &student[studentno].mark3);

        if (student[studentno].mark1 < 0 || student[studentno].mark1 > 100 ||
            student[studentno].mark2 < 0 || student[studentno].mark2 > 100 ||
            student[studentno].mark3 < 0 || student[studentno].mark3 > 100) {
            printf("Invalid marks for student %d\n", student[studentno].roll);
            return 0;
        }
    }

    for (int studentno = 0; studentno < count; studentno++) {
        int total = totalMarks(student[studentno]);
        float average = averageMarks(student[studentno]);
        char grade = getGrade(average);

        printf("Roll: %d\n", student[studentno].roll);
        printf("Name: %s\n", student[studentno].name);
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
