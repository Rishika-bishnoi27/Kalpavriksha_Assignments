# Compact Virtual File System (VFS)

## About the Project
This project is a simple Virtual File System built using the C programming language. It works like a basic file system where users can create files and directories, store data, read files, and manage disk space through commands.

The file system runs in memory and uses a virtual disk instead of creating actual files on the computer.

## Features
- Create files and directories.
- Write data into files and read stored data.
- Delete files and remove empty directories.
- Move between directories.
- Display files and folders in the current directory.
- Show the current directory path.
- Check total, used, and free disk blocks.
- Handle errors such as duplicate names, missing files, and insufficient disk space.
- Release allocated memory when exiting the program.

## Disk Structure
The virtual disk uses the following settings:

| Component | Value |
|---|---:|
| Block Size | 512 bytes |
| Total Blocks | 1024 |
| Total Disk Capacity | 512 KB |
| Maximum Name Length | 50 characters |

The disk is divided into fixed-size blocks. Each file uses the required number of blocks depending on its data size.

## Commands
| Command | Description |
|---|---|
| `mkdir <name>` | Create a new directory |
| `create <filename>` | Create a new file |
| `write <filename> "text"` | Write data into a file |
| `read <filename>` | Display file contents |
| `delete <filename>` | Delete a file |
| `rmdir <name>` | Remove an empty directory |
| `cd <name>` | Move into a directory |
| `cd ..` | Move to the parent directory |
| `cd /` | Move to the root directory |
| `ls` | List files and directories |
| `pwd` | Display the current directory path |
| `df` | Display disk usage |
| `exit` | Exit the program |

## How to Compile and Run
Make sure GCC is installed on your system.

**1. Compile the program**
```bash
gcc -Wall -Wextra -Wpedantic vfs.c -o vfs
```

**2. Run the program**
```bash
./vfs
```

## Example Usage
```text
mkdir documents
cd documents
create notes.txt
write notes.txt "Learning C programming"
read notes.txt
pwd
ls
df
cd ..
delete documents/notes.txt
exit
```

Note: The `delete` command works with filenames in the current directory, so move into `documents` before deleting `notes.txt`.

## Implementation Details
- **C Language:** Used to implement the complete file system.
- **Circular Doubly Linked List:** Used to manage files and directories.
- **Free Block List:** Keeps track of available disk blocks.
- **Virtual Disk:** A two-dimensional array stores file data.
- **Dynamic Memory Allocation:** Used to create file and directory nodes.
- **Block Management:** Allocates and releases disk blocks as files are written or deleted.
- **Input Validation:** Handles invalid commands, names, and file operations.

## Error Handling
The program checks for common errors, such as:

- Creating a file or directory with an existing name.
- Reading or deleting a file that does not exist.
- Writing data when the disk is full.
- Removing a directory that is not empty.
- Performing file operations on a directory.
- Invalid or inconsistent block information.

## How to Exit
Use the `exit` command to close the program. The program releases its allocated file-system memory before exiting.

## Conclusion
This project helped me understand how a basic file system works internally. It gave me practical experience with linked lists, pointers, dynamic memory allocation, file management concepts, and memory handling in C.