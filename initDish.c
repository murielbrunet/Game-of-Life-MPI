/*
  Edited by Muriel Brunet
  creates dish with 10,000 lines
  prints them out to output file dish.txt

 initDish.c
 D. Thiebaut

 Demo program for generating a dish with N lines,
 where N is provided by the user, and where a pattern
 of cell is repeated many times until N lines have 
 been created in the dish array.
 The function createDish returns an array of N strings
 where the strings are repetitions of the pattern stored
 in the array pattern[].

 To compile and run:
 
     gcc -o initDish initDish.c
     ./initDish

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* pattern[] = { "                                                                                ",
                    "   #                                                                            ",
                    " # #                                            ###                             ",
                    "  ##                                                                            ",
                    "                                                                                ",
                    "                                                      #                         ",
                    "                                                    # #                         ",
                    "                                                     ##                         ",
                    "                                                                                ",
                    "                                                                                ",
                    "                                                                                ",
                    "                                                                                ",
                    "             #                                                                  ",
                    "           # #                                                                  ",
                    "            ##                                                                  ",
                    "                                                                                ",
                    "                                                                                ",
                    "                                                                                " };


// createDish()
// input:  N: the number of lines we want in dish.
// output: dish: an array of N lines, each line being taken
//         from the array pattern, in a rotating manner.
char** createDish( int N ) {
  int i;
  int patternLength = sizeof( pattern )/sizeof( char * );
  char **dish;

  //--- create an array of N strings ---
  dish = (char **) malloc( N * sizeof( char * ) );

  //--- create N strings and "attach" them to an entry in        ---
  //--- dish.  Copy one of the lines from pattern into the newly ---
  //--- created string.                                          ---
  for ( i=0; i<N; i++ ) {
    dish[i] = (char *) malloc( ( strlen( pattern[0] )+1 )*sizeof( char ) );
    strcpy( dish[i], pattern[i % patternLength ] );
  }

  //--- return the fully populated array of N strings ---
  return dish;
}


// =================================================================
// MAIN:
// Creates a dish array of 40 lines and displays it.
// =================================================================

int main( int argc, char** argv ) {
  int i, N, length;
  char** dish;
  FILE *fp;

  //--- open file to save dish to ---
  fp = fopen("dish.txt", "w");

  //--- define the number of lines wanted ---
  N = 10000;

  //--- create an array of N lines ---
  dish = createDish( N );

  //--- print to output file ---
  for (i=0; i < N; i++ ) 
    fprintf( fp, "%s\n", dish[i] );

  fclose(fp);
}