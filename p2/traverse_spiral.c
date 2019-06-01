////////////////////////////////////////////////////////////////////////////////
// Main File:        traverse_spiral.c
// This File:        traverse_spiral.c
// Semester:         CS 354 Fall 2018
//
// Author:           Khai Bin Woon  
// Email:            woon2@wisc.edu  
// CS Login:         khai@cs.wisc.edu
//
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *COMMA = ","; 

// Structure representing Matrix
// size: Dimension of the square (size*size)
// array: 2D array of integers
typedef struct _Square {  
    int size;
    int **array;
} Square;

/* 
 * Retrieves from the first line of the input file,
 * the size of the square matrix.
 * 
 * fp: file pointer for input file
 * size: pointer to size of the square matrix
 */
void get_dimensions(FILE *fp, int *size) {
    char *line = NULL; 
	  size_t len = 0;
	  if (getline(&line, &len, fp) == -1) { 
		  printf("Error in reading the file\n");
		  exit(1); 
	  }
    
    *size = atoi(line);
}

/* 
 * Traverses a given layer from the square matrix
 * 
 * array: Heap allocated 2D square matrix
 * size: pointer to size of square
 * layer: layer number to be traversed
 * op: pointer to the output file
 */
void traverse_layer(int **array, int size, int layer, FILE *op) {       
	int row_num, col_num;
	int i,j;
	
	//corner case: size is odd & the layer is last so only one entry to print
        if(size % 2 == 1 && layer == (size + 1) / 2 - 1){
                fprintf(op, "%d\n", *(*(array + layer) + layer));
                return;
        }
  
  //Traverses all but the last number for each side/layer, prevent repetition
	//Traverse upper row from left to right 
	row_num = layer;
	for(j = layer; j <= size-layer-2; j++) { 
      fprintf(op, "%d ", *(*(array + row_num) + j) );
	}
  
	//Traverse right column from top to bottom
	col_num = size-layer-1;
	for(i = layer; i <= size-layer-2; i++) {
	    fprintf(op, "%d ", *(*(array + i) + col_num) );
	}

	//Traverse lower row from right to left
	row_num = size-layer-1;
	for(j = size-layer-1; j >= layer+1; j--) {
		  fprintf(op, "%d ", *(*(array + row_num) + j) );
	}

	//Traverse left column from bottom to top
	col_num = layer;
	for(i = size-layer-1; i >= layer+1; i--) {
      fprintf(op, "%d ", *(*(array + i) + col_num) );
	}
}


/* 
 * Traverses the square matrix spirally
 * 
 * square: pointer to a structure that contains 2D square matrix
 * op: pointer to the output file
 */
void traverse_spirally(Square *square, FILE *op) {       
	int size = square->size; 
	int num_layers = 0;   
	num_layers = size/2; 
	if(size%2 == 1) {
		num_layers++;
	}
	 
	int i;
	for(i = 0; i < num_layers; i++) {
		traverse_layer(square->array, size, i, op);
	}
}

/* 
 * This program reads a square matrix from the input file
 * and outputs its spiral order traversal to the output file
 *
 * argc: CLA count
 * argv: CLA value
 */
int main(int argc, char *argv[]) {                  

	//Check if number of arguments is correct
	if (argc != 3){
     fprintf(stderr, "Usage: ./traverse_spiral <input_filename> <output_filename>\n");
     exit(1);
  }
  
	//Open the file and check if it opened successfully
	FILE *ifp = fopen(*(argv + 1), "r");
	if (ifp == NULL) {
		fprintf(stderr, "Cannot open file for reading\n");
		exit(1);
	}
  
	//Call the function get_dimensions to retrieve size of the square matrix
	int size;
  get_dimensions(ifp, &size);

	//Dynamically allocate a 2D array as per the retrieved dimensions
  int **m;
  m = malloc(sizeof(int*) * size);
  if (m == NULL){
    fprintf(stderr, "Cannot allocate memory for array\n");
    exit(1);
  }
  for (int i = 0; i < size; i++){
    *(m + i) = malloc(sizeof(int) * size);
    if (*(m + i) == NULL){
      fprintf(stderr, "Cannot allocate memory for array\n");
      exit(1);
    }
  }


	//Read the file line by line by using the function getline as used in get_dimensions
	//Tokenize each line with comma to store the values in the square matrix
  char *line = NULL;
  size_t len = 0;
  char *token = NULL;
  
  for (int i = 0; i < size; i++){
  
    if (getline(&line, &len, ifp) == -1){
      fprintf(stderr, "Error while reading file\n");
      exit(1);
    }
    
    token = strtok(line,COMMA);
    for (int j = 0; j < size; j++){
      *(*(m+i) + j) = atoi(token);
      token = strtok(NULL, COMMA);
    }
  }
  


	//Create a structure and initialize its size and array fields appropriately
  Square *square = malloc(sizeof(Square));
  if( square == NULL){
    fprintf(stderr, "Cannot allocate memory for struct\n");
    exit(1);
  }
  square->size = size;
  square->array = m; m=NULL;
  

	//Open the output file
  FILE *ofp = fopen(*(argv + 2), "w");
  if (ofp == NULL) {
    fprintf(stderr, "Cannot open file for output\n");
    fclose(ifp);
    exit(1);
  }

	//Call the function traverse_spirally
  traverse_spirally(square,ofp);

	//Free the dynamically allocated memory
  for (int i = 0; i < size; i++){
    free(*((square->array)+i));
  }
  free(square->array);
  free(square);square=NULL;
  
	fclose(ifp);//Close the input file
	fclose(ofp);//Close the output file
 
	return 0; 
}     
