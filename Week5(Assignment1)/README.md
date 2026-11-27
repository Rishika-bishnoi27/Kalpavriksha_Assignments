# Smart Memory Manager

## About the Project

Smart Memory Manager is a C program that simulates how memory is allocated and released in a computer. It uses a fixed-size memory pool and manages different memory blocks based on user requests.

The program supports three memory allocation strategies: First Fit, Best Fit, and Worst Fit. It also allows users to release and resize allocated memory while keeping the memory map updated.

## Features

- Create a memory pool using dynamic memory allocation.
- Allocate memory using First Fit, Best Fit, and Worst Fit.
- Release allocated memory using unique allocation IDs.
- Split large free blocks during allocation.
- Merge adjacent free blocks after releasing memory.
- Resize existing memory allocations.
- Move allocations when in-place expansion is not possible.
- Display the complete memory map.
- Show memory usage statistics.
- Detect external fragmentation.
- Handle invalid inputs and allocation failures.

## Concepts Used

- Structures (`struct`)
- Pointers and dynamic memory allocation
- `malloc()`, `realloc()`, and `free()`
- `memcpy()` and `memmove()`
- Arrays and string handling
- Functions and modular programming
- First Fit, Best Fit, and Worst Fit algorithms
- Memory management and fragmentation

## How to Run

### 1. Clone the Repository

```bash
git clone <repository-url>
```

### 2. Open the Project Folder

```bash
cd <project-folder>
```

### 3. Compile the Program

```bash
gcc main.c -o memory_manager
```

### 4. Run the Program

```bash
./memory_manager
```

## Menu Options

The program provides the following options:

1. Allocate Memory
2. Release Memory
3. Resize an Allocation
4. Display Memory Map
5. Display Memory Statistics
6. Change Allocation Strategy
7. Exit

## Example

For a memory pool of 1000 bytes, suppose we allocate:

- 200 bytes
- 300 bytes
- 250 bytes

The remaining memory is 250 bytes.

If we release the first allocation, the memory map is updated and shows the released 200-byte region as free.

The program also merges adjacent free blocks whenever possible.

## Limitations

- The memory pool size must be between 1 and 1,000,000 bytes.
- Each allocation must be greater than 0.
- Allocations require a sufficiently large contiguous free block.
- The program uses a dynamic array for block metadata instead of a linked list.

## Conclusion

This project helped me understand how memory allocation works and how different allocation strategies manage available memory. It also gave me practical experience with pointers, dynamic memory allocation, memory resizing, and fragmentation handling in C.