// Copyright 2019 Marco Wang <m.aesophor@gmail.com>
// 
// Implementation note:
// 
// scanner.c converts a sequence of characters into a sequence of tokens.
//
// It contains various functions to get the next token from the input stream
// (istream for abbreviation in the following context). For each token class,
// there's a dedicated tokenizing function.
//
// During tokenization, if current character is accepted, then we advance
// istream's position by one char. If not accepted, we keep backtracking
// istream's position by one char until it reaches the original postion
// or an accepted state is reached.
//
// In order to get the next token from istream, we'll try all tokenizing functions
// one by one. If after executing a tokenizing function, the position of istream
// remains the same, then we'll try another one until all functions have been tried.
 
#include <stdio.h>
#include <stdlib.h>

#include "token.h"


int fpeek(FILE* fstream) {
  int c = fgetc(fstream);
  ungetc(c, fstream);
  return c;
}



int main(int argc, char* args[]) {
  if (argc < 2) {
    printf("usage: \n");
    return EXIT_SUCCESS;
  }

  // Open the specified file from args[1]
  FILE* fin = fopen(args[1], "r");
  if (fin == NULL) {
    perror("Fatal error");
    return EXIT_FAILURE;
  }


  fclose(fin);
  return EXIT_SUCCESS;
}
