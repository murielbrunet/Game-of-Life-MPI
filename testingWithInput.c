/*  
  Generalized for N processes
  Muriel Brunet
  Parallel and Distributed Computing
  CSC 352 Smith College 
  Spring 2017


 Game of life
 D. Thiebaut
 This is the MPI version of GameOfLife.c, for MPI, for 2 Processes,
 or tasks.
 This version works only for 2 tasks.

 It hasn't been optimized.  Each task works on one half of the dish
 arrays where the generations evolve, but actually maintain a full
 array in memory.
 
 The two tasks communicate with each other after each generation 
 and exchange 2 rows, each.  

 This code works in console mode, displaying successive generations
 of the game of life on the screen, and clearing the screen between
 each one.

 The initial pattern is defined in the array PATTERN.

 To compile and run, several different variants:

 mpicc.mpich -o testingWithInput testingWithInput.c
 mpiexec.mpich -n 2 ./testingWithInput dish.txt

*/
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "mpi.h"

// ------------------------------------- MACROS ----------------------------------------
// #define NUMBERROWS 28
#define esc 27
#define cls() printf("%c[2J",esc)
#define pos(row,col) printf("%c[%d;%dH",esc,row,col)

// ------------------------------------- GLOBALS ----------------------------------------
int   NUMBERROWS;
char  **DISH0;
char  **DISH1;
// char  *PATTERN[NUMBERROWS] = {
//   "                                                                                  ",
//   "   #                                                                              ",
//   " # #                                            ###                               ",
//   "  ##                                                                              ",
//   "                                                                                  ",
//   "                                                      #                           ",
//   "                                                    # #                           ",
//   "                                                     ##                           ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "             #                                                                    ",
//   "           # #                                                                    ",
//   "            ##                                                                    ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  ",
//   "                                                                                  " 
// };
int ROWSIZE = (int) strlen( "                                                                                  ") + 1;

//------------------------------- prototypes --------------------------------
void life( char**, char**, int , int, int );
void initDishes( char* );
void print( char **, int, int, int );
// void combineFinalDishes( );
// --------------------------------------------------------------------------
// initDishes
// inits the dishes (current and future)
void initDishes( char* fileName ) {
  int i, n;
  char ch, buffer[10000];

  //--- read # of lines first ---
  FILE *f = fopen( fileName, "r" );
  int noLines = 0;
  while ( !feof( f ) ) {
    ch = fgetc( f );
    if (ch == '\n') noLines++;
  }
  fclose( f );

  //--- set global ---
  NUMBERROWS = noLines;
  //printf( "number of lines = %d\n", noLines );

  //--- initialize DISH arrays ---
  DISH0 = (char **) malloc( noLines * sizeof( char* ) );
  DISH1 = (char **) malloc( noLines * sizeof( char* ) );

  //--- read file again and initialize arrays ---
  f = fopen( fileName, "r" );
  i = 0;
  while ( !feof( f ) ) {
    fgets( buffer, 10000, f );
    if ( feof( f ) )
      break;
    buffer[9999] = '\0';
    DISH0[i] = (char *) malloc( (strlen( buffer) + 1 ) * sizeof( char ) );
    strcpy( DISH0[i], buffer );
    DISH1[i] = (char *) malloc( (strlen( DISH0[i] ) + 1 ) * sizeof( char )  );
    strcpy( DISH1[i], DISH0[i] );
    i++;
  }
  fclose( f );
}

// --------------------------------------------------------------------------
// print
void print( char* dish[], int rank, int firstRow, int lastRow ) {
  int i;

  printf("RANK %d--------------------------------\n", rank);

  for (i=firstRow; i<=lastRow; i++ ) {
    if ( dish[i] == NULL ) continue;
    pos( i, 0 );
    printf( "%s\n", dish[i] );
  }

}

// --------------------------------------------------------------------------
void check( char** dish, char** future ) {
  int i, j, k, l;
  l = sizeof( dish )/sizeof( dish[0] );
  printf( "length of dish = %d\n", l );

  for ( i=0; i<l; i++ ) {
    k = strlen( dish[i] );
    printf( "%d %s\n", k, dish[i] );
  }
  printf( "\n\n" );

  l = sizeof( future )/sizeof( future[0] );
  printf( "length of future = %d\n", l );

  for ( i=0; i<l; i++ ) {
    k = strlen( future[i] );
    printf( "%d %s\n", k, future[i] );
  }
  printf( "\n\n" );

}

// --------------------------------------------------------------------------
void  life( char** dish, char** newGen, int rank, int lowerRow, int upperRow ) {
  /*
   * Given an array of string representing the current population of cells
   * in a petri dish, computes the new generation of cells according to
   * the rules of the game. A new array of strings is returned.
   */
  int i, j, k, row;
  int rowLength = (int) strlen( dish[0] );
  int dishLength = NUMBERROWS;

  for (row = lowerRow; row < upperRow; row++) {// each row
    
    if ( dish[row] == NULL )
      continue;

    for ( i = 0; i < rowLength; i++) { // each char in the
                                       // row

      int r, j, neighbors = 0;
      char current = dish[row][i];

      // loop in a block that is 3x3 around the current cell
      // and count the number of '#' cells.
      for ( r = row - 1; r <= row + 1; r++) {

        // make sure we wrap around from bottom to top
        int realr = r;
        if (r == -1)
          realr = dishLength - 1;
        if (r == dishLength)
          realr = 0;

        for ( j = i - 1; j <= i + 1; j++) {

          // make sure we wrap around from left to right
          int realj = j;
          if (j == -1)
            realj = rowLength - 1;
          if (j == rowLength)
            realj = 0;

          if (r == row && j == i)
            continue; // current cell is not its
          // neighbor
          if (dish[realr][realj] == '#')
            neighbors++;
        }
      }

      if (current == '#') {
        if (neighbors < 2 || neighbors > 3)
          newGen[row][i] =  ' ';
        else
          newGen[row][i] = '#';
      }

      if (current == ' ') {
        if (neighbors == 3)
          newGen[row][i] = '#';
        else
          newGen[row][i] = ' ';
      }
    }
  }
}

// char** combineFinalDishes( int sectionSize , int firstRow, int lastRow){
//   int i;

//   //--- if manager, receive all the computed dish pieces from each process ---
//   //--- combine these dish pieces into a single array ---
//   if( rank==0 ){
//     //--- copy processes' computation one row at a time to finalDish ---    
//     for(i = firstRow; i < sectionSize; i++){
//       MPI_Recv( finalDish[i],  ROWSIZE, MPI_CHAR,    nextProcess,     0,  MPI_COMM_WORLD, &status );
//     }
//   } else {
//     //--- send computation one row at a time to manager ---
//     for(i = firstRow; i <= lastRow; i++){
//       //          buffer   #items   item-size src/dest  tag          world  
//       MPI_Send( future[i],  ROWSIZE, MPI_CHAR,    0,     0,  MPI_COMM_WORLD ); 
//     }
//   }
//   return finalDish;
// }

// --------------------------------------------------------------------------
int main( int argc, char* argv[] ) {
  int gens = 30;      // # of generations
  int i, nextProcess, prevProcess;
  int sizeOfSection, lastRow, firstRow;
  int n;                // # of processes/tasks
  char **dish, **future, **temp, **finalDish;
  char* fileName;

  //--- MPI Variables ---
  int noTasks = 0;
  int rank = 0;
  MPI_Status status;   // required variable for receive routines

  //--- initialize MPI ---
  MPI_Init( &argc, &argv );

  //--- get number of tasks, and make sure it's 2 ---
  MPI_Comm_size( MPI_COMM_WORLD, &noTasks );
  if( noTasks % 2 != 0){
    printf( "Number of Processes/Tasks must be even.  Number = %d\n\n", noTasks );
    MPI_Finalize();
    return 1;
  } else {
    n = noTasks; // number of tasks/processes
  }

  if ( argc < 2 ) {
    printf( "Syntax: ./GameOfLife2 dishFileName\n\n" );
    return 1;
  }

  //--- get the file name from the command line ---
  fileName = argv[1];


  //--- get rank ---
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );

  //--- init the dishes as half of the original problem ---
  initDishes( fileName );

  dish   = DISH0;
  future = DISH1;

  //check( dish, future );

  //--- clear screen ---
  cls();

  sizeOfSection = NUMBERROWS/n;
  firstRow =  sizeOfSection * rank; //give rows based on rank
  lastRow = firstRow + sizeOfSection; //assign rows based on rank

  // display the new generation
  print( dish, rank, firstRow, lastRow );

  if(rank == 0){
    prevProcess = n - 1;
  } else {
    prevProcess = rank - 1;
  }

  if(rank == n - 1){ // if it's the last process
    nextProcess = 0;
  } else { 
    nextProcess = rank + 1;
  }


  // iterate over all generations
  for ( i = 0; i < gens; i++) {
    
    pos( 33+rank, 0 );
    //printf( "Rank %d: Generation %d\n", rank, i );

    // apply the rules of life to the current population and 
    // generate the next generation.
    life( dish, future, rank , firstRow, lastRow );

    //--- if rank is odd, then send ---
    //--- if rank is even, then receive ---
    if(rank % 2 == 0){ //even
    //          buffer              #items  item-size     src/dest tag          world  
      MPI_Send( future[firstRow],   ROWSIZE, MPI_CHAR,    prevProcess,     0,  MPI_COMM_WORLD );
      MPI_Send( future[lastRow-1],  ROWSIZE, MPI_CHAR,    nextProcess,     0,  MPI_COMM_WORLD );
      MPI_Recv( future[lastRow-1],  ROWSIZE, MPI_CHAR,    prevProcess,     0,  MPI_COMM_WORLD, &status );
      MPI_Recv( future[firstRow],   ROWSIZE, MPI_CHAR,    nextProcess,     0,  MPI_COMM_WORLD, &status );
    } else { //odd
      MPI_Recv( future[firstRow],   ROWSIZE, MPI_CHAR,    nextProcess,     0,  MPI_COMM_WORLD, &status );
      MPI_Recv( future[lastRow-1],  ROWSIZE, MPI_CHAR,    prevProcess,     0,  MPI_COMM_WORLD, &status );
      MPI_Send( future[lastRow-1],  ROWSIZE, MPI_CHAR,    nextProcess,     0,  MPI_COMM_WORLD );
      MPI_Send( future[firstRow],   ROWSIZE, MPI_CHAR,    prevProcess,     0,  MPI_COMM_WORLD );
    }


    // copy future to dish
    temp = dish;
    dish = future;
    future = temp;
  }

  //--- display the last generation ---
  print(dish, rank, firstRow, lastRow);

  //--- close MPI ---
  pos( 30+rank, 0 );
  //printf( "Process %d done.  Exiting\n\n", rank );
  MPI_Finalize();

  return 0;
}