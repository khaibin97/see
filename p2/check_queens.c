////////////////////////////////////////////////////////////////////////////////
// Main File:        check_queens.c
// This File:        check_queens.c
// Semester:         CS 354 Fall 2018
//
// Author:           Khai Bin Woon
// Email:            woon2@wisc.edu
// CS Login:         khai@cs.wisc.edu
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h> 
#include <stdlib.h>  
#include <string.h>   

char *COMMA = ",";

/* COMPLETED:
 * Retrieves from the first line of the input file,
 * the number of rows and columns for the board.
 * 
 * fp: file pointer for input file
 * rows: pointer to number of rows
 * cols: pointer to number of columns
 */
void get_dimensions(FILE *fp, int *rows, int *cols) {     
	char *line = NULL; 
	size_t len = 0;
	if (getline(&line, &len, fp) == -1) { 
		printf("Error in reading the file\n");
		exit(1); 
	}

	char *token = NULL; 
	token = strtok(line, COMMA);
	*rows = atoi(token); 

	token = strtok(NULL, COMMA); 
	*cols = atoi(token);
}      


/* 
 * Returns 1 if and only if there exists at least one pair
 * of queens that can attack each other.
 * Otherwise returns 0.
 * 
 * board: heap allocated 2D board
 * rows: number of rows
 * cols: number of columns
 */
int check_queens(int **board, int rows, int cols) {       

  int rw,cl;//for checking square
      
  //iterate each board square
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      if ( *(*(board+i) + j) == 1) //check if there is a queen at "starting square"
      {
        rw = i+1; //initialize the checking square to compare 1 square 
        cl = j+1; //to the right or bottom of "starting square"
        
        while (rw < rows && cl < cols){ //check bounds
        
          if ( *(*(board+i) + j) == *(*(board+rw) + j))//check row
            return 1;
            
          if ( *(*(board+i) + j) == *(*(board+i) + cl))//check column
            return 1;
            
          if ( *(*(board+i) + j) == *(*(board+rw) + cl))//check diagonal
            return 1;
            
          rw++;cl++;//move to next
        }
        
      }//end if
      
    }
    
  }
	return 0;
}     


/*
 * This program prints true if the input file has any pair
 * of queens that can attack each other, and false otherwise
 * 
 * argc: CLA count
 * argv: CLA value 
 */
int main(int argc, char *argv[]) {                

        //Check if number of command-line arguments is correct.
	      if ( argc != 2 ){
             printf("Usage: ./check_queens <input_filename>\n");
             exit(1);
        }

	//Open the file and check if it opened successfully.
	FILE *fp = fopen(*(argv + 1), "r");
	if (fp == NULL) {
		printf("Cannot open file for reading\n");
		exit(1);
	}

        //Declare local variables.
        int rows, cols;

        //Retrieve the board dimensions.
        get_dimensions(fp,&rows,&cols);

        //Dynamically allocates a 2D array of dimensions retrieved above.
	      int **board = malloc(sizeof(int*) * rows);
             if (board == NULL){
               printf("Could not allocate memory for array.\n");
               exit(1);
             }
        for (int i = 0 ; i < rows ; i++)
        {
          *(board + i) = malloc(sizeof(int) * cols); 
            if ( *(board+i) == NULL ){
              printf("Could not allocate memory for array.\n");
              exit(1);
            }  
        }

	//Read the file line by line.
	//Tokenize each line wrt comma to store the values in your 2D array.
	char *line = NULL;
	size_t len = 0;
	char *token = NULL;
	for (int i = 0; i < rows; i++) {

		if (getline(&line, &len, fp) == -1) {
			printf("Error while reading the file\n");
			exit(1);	
		}

		token = strtok(line, COMMA);
		for (int j = 0; j < cols; j++) 
    {
			*(*(board+i)+j) = atoi(token);
			token = strtok(NULL, COMMA);	
		}
	}

	      //Call the function check_queens and print the appropriate
        //output depending on the function's return value.
        int results = check_queens(board,rows,cols);
        if ( results == 1 )
          printf("true\n");
        else
          printf("false\n");
        
	//Free all dynamically allocated memory.
  for (int i = 0; i < rows; i++)
  {
    free(*(board+i));
  }
  free(board); board = NULL;

	//Close the file.
	if (fclose(fp) != 0) {
		printf("Error while closing the file\n");
		exit(1);	
	}

	return 0;
}    
