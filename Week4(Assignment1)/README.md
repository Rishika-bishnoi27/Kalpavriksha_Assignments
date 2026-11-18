# Sonar Image Processing

## Problem Description
This program generates a random matrix of sonar intensity values, rotates it 90 degrees clockwise, and applies a 3×3 smoothing filter to reduce sudden changes in values.

## Approach

### 1. Matrix Generation
- Take the matrix size `N` as input, between 2 and 10.
- Generate random integer values between 0 and 255.
- Display the original matrix.

### 2. Rotate the Matrix
- Rotate the matrix 90 degrees clockwise.
- Swap pointers instead of swapping the actual values.
- Perform the rotation in-place without creating another matrix.

### 3. Apply the Smoothing Filter
- Replace each cell with the average of itself and its valid neighboring cells within a 3×3 area.
- Handle edge and corner cells by including only the neighbors that exist.
- Use two temporary row buffers to store values and avoid using already updated values.
- Update the matrix in-place.

## Pointer Usage
The program uses pointer arithmetic to access and update matrix elements instead of array indexing.

## Memory Management
- Allocate memory dynamically for the matrix and its elements.
- Use only two temporary row buffers for smoothing.
- Free all allocated memory before the program ends.

## Input Format
Enter an integer between 2 and 10 representing the matrix size.

## Output Format
The program displays:
1. The original randomly generated matrix.
2. The matrix after 90-degree clockwise rotation.
3. The matrix after applying the 3×3 smoothing filter.

## How to Run
Compile the program using GCC:

```bash
gcc sonar.c -o sonar
```

Run the program:

```bash
./sonar
```

## Constraints
- Matrix size: 2 ≤ N ≤ 10.
- Each intensity value is between 0 and 255.
- No additional N×N matrix is used.
- Pointer arithmetic is used for matrix access.
- The smoothing operation uses O(N) extra space for two temporary row buffers.

## Conclusion
This program demonstrates pointer arithmetic, dynamic memory allocation, in-place matrix rotation, and smoothing using limited extra memory.