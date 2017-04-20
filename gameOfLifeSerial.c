/*  
 Game of life (V2)
 D. Thiebaut
 Heavily adapted from code found in java section at this
 URL:  https://rosettacode.org/wiki/Conway%27s_Game_of_Life#Java
 and ported to C.

 This code works in console mode, computing 3000 generations of
 a game of life provided in a dish that is originally read from a data 
 file.

 To compile and run:

 gcc -o gameOfLifeSerial gameOfLifeSerial.c
 ./gameOfLifeSerial  dishFileName


*/
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// ------------------------------------- MACROS ----------------------------------------
#define esc 27
#define cls() printf("%c[2J",esc)
#define pos(row,col) printf("%c[%d;%dH",esc,row,col)

// ------------------------------------- GLOBALS ----------------------------------------
int   NUMBERROWS;
char  **DISH0;
char  **DISH1;

// --------------------------------- prototypes -----------------------------
void life( char**, char** );
void initDishes( char* );
void clearScreen();
void print( char ** );

// --------------------------------------------------------------------------
// initDishes
// inits the dishes (current and future)
void initDishes( char* fileName ) {
  int i, n;
  char ch, buffer[1000];

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
    fgets( buffer, 1000, f );
    if ( feof( f ) )
      break;
    buffer[999] = '\0';
    DISH0[i] = (char *) malloc( (strlen( buffer) + 1 ) * sizeof( char ) );
    strcpy( DISH0[i], buffer );
    DISH1[i] = (char *) malloc( (strlen( DISH0[i] ) + 1 ) * sizeof( char )  );
    strcpy( DISH1[i], DISH0[i] );
    i++;
  }
  fclose( f );
}


// --------------------------------------------------------------------------
// clearscreen:
void clearScreen() {
  /*
   * brings the cursor home (top-left), so that the next generation will
   * be printed over the current one.
   */
  return;
  char ANSI_CLS[] = "\x1b[2J";
  char ANSI_HOME[] = "\x1b[H";
  printf( "%s%s", ANSI_HOME, ANSI_CLS );

}

// --------------------------------------------------------------------------
// print
void print( char* dish[] ) {
  /*
   * just display all the lines of the array of strings.
   */
  int i;

  for (i=0; i<NUMBERROWS; i++ ) {
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
void  life( char** dish, char** newGen ) {
  /*
   * Given an array of string representing the current population of cells
   * in a petri dish, computes the new generation of cells according to
   * the rules of the game. A new array of strings is returned.
   */
  int i, j, row;
  int rowLength = strlen( dish[0] );
  int dishLength = NUMBERROWS;

  for (row = 0; row < NUMBERROWS; row++) {// each row

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

        for (int j = i - 1; j <= i + 1; j++) {

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


// --------------------------------------------------------------------------
int main( int argc, char* argv[] ) {
  int gens = 3000;      // # of generations
  int i;
  char **dish, **future, **temp;
  char *fileName;
  unsigned long micros = 0;
  clock_t startTime, endTime;

  if ( argc < 2 ) {
    printf( "Syntax: ./GameOfLife2 dishFileName\n\n" );
    return 1;
  }

  //--- get the file name from the command line ---
  fileName = argv[1];

  //--- clear screen ---
  //cls();

  //--- init the global arrays ---
  initDishes( fileName );

  //--- start measuring time ---
  startTime = clock();

  //--- set pointers to them ---
  dish   = DISH0;
  future = DISH1;

  //--- for debugging ---
  //check( dish, future );

  //--- print first generation ---
  //print( dish );          


  //--- iterate over all generations ---
  for ( i = 0; i < gens; i++) {
    
    //printf( "Generation %d\n", i );

    // apply the rules of life to the current population and 
    // generate the next generation.
    life( dish, future );

    // display the new generation (comment this out if you want
    // the last generation, instead)
    //print( dish );

    // add a bit of a delay to better see the visualization
    // remove this part to get full timing.
    //sleep(1);  // 1 sec

    // copy future to dish
    temp = dish;
    dish = future;
    future = temp;
  }

  // display the last generation
  //print(dish);

  //--- measure elapsed time ---
  endTime = clock();
  float seconds = (endTime - startTime)/1000000.0;
  printf( "%1.5f\n", seconds );
}
