# Student Performance Analyzer
This assignment is a simple **C program** that analyzes the performance of multiple students based on their marks in three subjects.

## Features
- Stores student details using a structure.
- Takes roll number, name, and marks of three subjects as input.
- Calculates total and average marks.
- Assigns grades based on the average:
  - **A** → 85 and above
  - **B** → 70 to 84
  - **C** → 50 to 69
  - **D** → 35 to 49
  - **F** → Below 35
- Displays a star pattern according to the grade.
- Skips the star pattern for students with an average below 35.
- Uses functions for different calculations.
- Uses recursion to display all roll numbers at the end.
- Validates the number of students and marks.

## Input
First, enter the number of students. Then enter the roll number, name, and marks of three subjects for each student.

Example:
```text
2
1 Arti 78 82 90
2 Meena 32 28 35
```

## Output
```text
Roll: 1
Name: Arti
Total: 250
Average: 83.33
Grade: B
Performance: ****

Roll: 2
Name: Meena
Total: 95
Average: 31.67
Grade: F

List of Roll Numbers (via recursion): 1 2
```

## Concepts Used
- Structures
- Functions
- Arithmetic operators
- If-else statements
- Loops
- Continue statement
- Recursion
- Variable scope
- Input validation

## Constraints
- Number of students: **1 to 100**
- Marks in each subject: **0 to 100**

## File
```text
student.c
```

## How to Run
Compile the program using:

```bash
gcc student.c -o student
```

Then run:

```bash
./student
```

For Windows:

```bash
student.exe
```