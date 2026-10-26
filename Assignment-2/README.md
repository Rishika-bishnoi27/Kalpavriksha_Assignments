# Assignment 2
This assignment is based on C programming and file handling.

## About
The program is a simple User Management System. It stores user details such as ID, name, and age in a text file.
The program allows the user to add, read, update, and delete user records.

## Features
- Add a new user
- Read all users
- Update user details
- Delete a user
- Check for duplicate user IDs
- Store user data in a text file

## Concepts Used
- C Programming
- Structures
- Functions
- File Handling
- Loops
- Conditional Statements
- Input and Output

## File Handling
- The program uses `users.txt` to store user records.
- A temporary file named `temp.txt` is used while updating and deleting records.
- The `users.txt` file is created automatically when the program starts if it does not already exist.

## How to Run
Compile the program using GCC:
```bash
gcc crud.c -o crud
```

Run the program:
```bash
./crud
```

## Menu Options
1. Add User
2. Read Users
3. Update User
4. Delete User
5. Exit

## Files
- `crud.c` - Main C program
- `users.txt` - Stores user records
- `temp.txt` - Temporary file used during update and delete operations