# ICC ODI Player Performance Analyzer

## About the Project
ICC ODI Player Performance Analyzer is a menu-driven C program that manages and analyzes ODI cricket player data. It stores player and team information and allows users to search, sort, add, and display players based on different conditions.

The project uses linked lists, searching, sorting, and performance index calculations to organize player data efficiently.

## Features
- Add new players to a team.
- Display all players of a specific team.
- Display teams according to their average batting strike rate.
- Display Top K players of a team based on their role and performance index.
- Display all players of a specific role across all teams in descending order of performance index.
- Calculate performance index for batsmen, bowlers, and all-rounders.
- Search teams using Binary Search.
- Manage player data using linked lists.

## Technologies Used
- C Programming Language
- Linked Lists
- Binary Search
- Sorting Algorithms
- Dynamic Memory Allocation

## Player Roles
The program supports three player roles:

- **Batsman**
- **Bowler**
- **All-rounder**

## Performance Index
The performance index is calculated according to the player's role.

### Batsman

`(Batting Average × Strike Rate) / 100`

### Bowler

`(Wickets × 2) + (100 − Economy Rate)`

### All-rounder

`(Batting Average × Strike Rate) / 100 + (Wickets × 2)`

Performance indexes of different roles are not directly comparable.

## Menu Options
1. Add Player to Team
2. Display Players of a Specific Team
3. Display Teams by Average Batting Strike Rate
4. Display Top K Players of a Specific Team by Role
5. Display All Players of a Specific Role Across All Teams
6. Exit

## Data Structures Used

### Linked List
Linked lists are used to store player data dynamically and manage players without a fixed player limit in the program's structure.

### Binary Search
Binary Search is used to find a team based on its Team ID efficiently.

### Sorting
Sorting is used to arrange teams according to average batting strike rate and players according to their performance index.

## How to Run

### 1. Clone the Repository
```bash
git clone <repository-url>
```

### 2. Move to the Project Directory
```bash
cd <project-folder>
```

### 3. Compile the Program
```bash
gcc main.c -o analyzer
```

### 4. Run the Program
```bash
./analyzer
```

## Objective
The main objective of this project is to manage ODI player data and analyze player and team performance using basic data structures and algorithms.

## Author
Developed as a C programming project.
