# Employee Hierarchy Management System

## About the Project
This project is made using C to manage employees and their reporting relationships in an organization. It allows users to add employees, remove them, and view the organization hierarchy through a simple menu.

Each employee has a unique ID, name, designation, and reporting manager.

## Features
- Add a new employee under an existing manager or add a top-level employee.
- Remove an employee while keeping the hierarchy organized.
- Transfer the removed employee's direct reports to the sibling who has the fewest direct reports.
- If the employee has no siblings, transfer their reports to their superior.
- View the hierarchy of a particular employee.
- Display the complete organization hierarchy.
- Count the employees under a particular employee or in the entire organization.
- Handle invalid input and duplicate employee IDs.
- Use dynamic memory allocation to store employee details.

## Technologies Used
- C
- Structures and Pointers
- Dynamic Memory Allocation
- Tree Data Structure
- Recursion

## Menu Options
1. Add Employee
2. Remove Employee
3. Display Employee Hierarchy
4. Display Complete Hierarchy
5. Exit

## Rules for Removing an Employee
- A top-level employee (CEO) cannot be removed.
- If the employee has siblings, their direct reports are transferred to the sibling with the fewest direct reports.
- If two siblings have the same number of direct reports, the first one found is selected.
- If the employee has no siblings, their direct reports are transferred to their superior.
- The removed employee's memory is freed after deletion.

## How to Run the Program
Compile the code using GCC:

```bash
gcc employee_hierarchy.c -o employee_hierarchy
```

Run the program:
```bash
./employee_hierarchy
```

## What I Learned
While working on this project, I practised using structures, pointers, recursion, dynamic memory allocation, and tree traversal. I also learned how to maintain employee reporting relationships when adding or removing employees.

