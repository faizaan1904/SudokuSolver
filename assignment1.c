#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

// #define size_grid 9  // to replace later.

void print_grid(int size, int grid[36][36]) {
  int i, j;
  /* The segment below prints the grid in a standard format. Do not change */
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++)
      printf("%d\t", grid[i][j]);
    printf("\n");
  }
}

void read_grid_from_file(int size, char* ip_file, int grid[36][36]) {
  FILE* fp;
  int i, j;
  fp = fopen(ip_file, "r");
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++) {
      fscanf(fp, "%d", &grid[i][j]);
    }
}

int sqRoot(int N) {
  switch (N) {
  case 1: return 1;
  case 4: return 2;
  case 9: return 3;
  case 16: return 4;
  case 25: return 5;
  case 36: return 6;
  }
}

typedef struct {
  int size;
  int row;
  int col;
  int grid[36][36];
  int num_to_be_checked;
  // int found;
} sudoku_board;

void* col_checker(void* arg) {
  sudoku_board* s = (sudoku_board*)arg;
  int r;
  int* found = malloc((sizeof(int)));
  // printf("Row thread created");
  for (r = 0; r < s->size; r++) {
    // printf("hi\n");
    if (s->grid[r][s->col] == s->num_to_be_checked) {
      *found = 1;
      break;
    }
    else *found = 0;
  }
  return (void*)found;
  // pthread_exit(NULL);
}

void* row_checker(void* arg) {
  sudoku_board* s = (sudoku_board*)arg;
  int c;
  int* found = malloc((sizeof(int)));
  // printf("Row_Checker thread here! %d is the number to be checked.\n", s->num_to_be_checked);
  // printf("Column thread created");
  for (c = 0; c < s->size; c++) {
    // printf("Checking the cell with value %d \n", s->grid[s->row][c]);
    if (s->grid[s->row][c] == s->num_to_be_checked) {
      // printf("%d is found unfortunately \n", s->grid[s->row][c]); // not reaching
      *found = 1;
      break;
    }
    else *found = 0;
  }
  return (void*)found;
  // pthread_exit(NULL);
}

void* box_checker(void* arg) {
  sudoku_board* s = (sudoku_board*)arg;
  int r, c;
  int* found = malloc((sizeof(int)));
  // printf("Sub-box thread created");
  int boxOriginRow = s->row - (s->row % (int)sqRoot(s->size));
  int boxOriginCol = s->col - (s->col % (int)sqRoot(s->size));
  for (int r = 0; r < (int)sqRoot(s->size); r++) {  // box checker
    for (int c = 0; c < (int)sqRoot(s->size); c++) {
      if (s->grid[r + boxOriginRow][c + boxOriginCol] == s->num_to_be_checked) {
        *found = 1;
        break;
      }
      else *found = 0;
    }
    return (void*)found;
    // pthread_exit(NULL);
  }
}


  int checker(int grid[36][36], int row_num, int col_num,
    int num_to_be_checked, int size) {
    // printf("checking\n");
    pthread_t row, col, box;
    sudoku_board s[3];
    int *found_0;
    int *found_1;
    int *found_2;

    // initializing the structures
    for (int h = 0; h < 3; h++) {
      memcpy(s[h].grid, grid, 36*36*sizeof(int)); // FIND A WAY TO COPY grid to s[h].grid
      // copy_array(s[h].grid, grid)
      s[h].size = size;
      s[h].row = row_num;
      s[h].col = col_num;
      s[h].num_to_be_checked = num_to_be_checked;;
      // s[h].found = 0;
    }



    if (pthread_create(&row, NULL, row_checker, (void*)(&s[0]))) {
      printf("Error in pthread_create for row thread");
      exit(-1);
    }
    if (pthread_create(&col, NULL, col_checker, (void*)(&s[1]))) {
      printf("Error in pthread_create for col thread");
      exit(-1);
    }
    if (pthread_create(&box, NULL, box_checker, (void*)(&s[2]))) {
      printf("Error in pthread_create for box thread");
      exit(-1);
    }

    // void* found_0;
    // void* found_1;
    // void* found_2;
    pthread_join(row, (void**) &found_0);
    pthread_join(col, (void**) &found_1);
    pthread_join(box, (void**) &found_2);
    // printf("Threads joined");

    // printf("%d, %d, %d\n", s[0].found);

    if (!(*found_0 + *found_1 + *found_2)) {
      return 1;
    }

    return 0;
  }

  int solve(int grid[36][36], int size) {
    // printf("entered solve\n");
    for (int r = 0; r < size; r++) {
      for (int c = 0; c < size; c++) {
        if (grid[r][c] == 0) {  // empty cell found
          // printf("Empty cell found. Trying to fill some number now. \n");
          for (int possible = 1; possible <= size; possible++) {
            if (checker(grid, r, c, possible, size)) {
              grid[r][c] = possible;  // set cell value

              // printf("grid cell set to %d\n", possible);
              printf("new grid:\n");
              print_grid(size, grid);

              if (solve(grid, size)) return 1;  // correct solution found, return.

              grid[r][c] = 0;  // not correct solution,reset cell,try next possibility
            }
          }
        }
      }
    }
    return 0;  // meep.
  }

  int main(int argc, char* argv[]) {
    int grid[36][36], size, i, j;

    if (argc != 3) {
      printf("Usage: ./sudoku.out grid_size inputfile");
      exit(-1);
    }

    size = atoi(argv[1]);
    read_grid_from_file(size, argv[2], grid);

    // printf("original grid:\n");
    // print_grid(size, grid);

    if (solve(grid, size))
      print_grid(size, grid);
    else
      printf("no solution\n");
  }