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
#include <stdbool.h>

#include "token.h"


int fpeek(FILE* f) {
  char c = fgetc(f);
  ungetc(c, f);
  return c;
}


bool is_whitespace(char c) {
  return c == ' ' || c == '\t' || c == 0xd || c == 0xa;
}

bool is_alphabet(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool is_digit(char c) {
  return c >= '0' && c <= '9';
}

bool is_underscore(char c) {
  return c == '_';
}

// Identifier
bool scan_iden(FILE* f) {
  // 第一個字必須是英文字母或底線字元
  // 由英文字母、底線及數字組成, 長度不限
  char c = fpeek(f);

  if (is_alphabet(c) || is_underscore(c)) {
    char str[32] = {0};
    size_t current = 0;
    // Advance cursor
    do {
      c = fgetc(f);
      str[current++] = c;
    } while (is_alphabet(c) || is_underscore(c) || is_digit(c));

    // Backtrack
    ungetc(c, f);
    str[current - 1] = 0x00;
    //printf("current offset: %ld\t", ftell(f));
    printf("IDEN\t%s\n", str);
    return true;
  } else {
    return false;
  }
}

// Reserved word
bool scan_rewd(FILE* f) {
  
}

bool scan_inte(FILE* f) {
  char c = fpeek(f);

  if (is_digit(c)) {
    char str[12] = {0};
    size_t current = 0;
    // Advance cursor
    do {
      c = fgetc(f);
      str[current++] = c;
    } while (is_digit(c));

    // Backtrack
    ungetc(c, f);
    str[current - 1] = 0x00;
    printf("INTE\t%s\n", str);
    return true;
  } else {
    return false;
  }
}

// Special symbol
bool scan_spec(FILE* f) {
  char c = fpeek(f);

  if (c == '{' || c == '}' || c == '(' || c ==')' || c ==';') {
    printf("SPEC\t%c\n", c);
    fgetc(f);
    return true;
  } else {
    return false;
  }
}


bool get_next_token(FILE* f) {
  bool result = scan_iden(f);
  if (result) return true;

  result = scan_spec(f);
  if (result) return true;

  result = scan_inte(f);
  if (result) return true;

  return false;
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

  char c = 0x00;
  do {
    get_next_token(fin);
    c = fpeek(fin);
    if (is_whitespace(c)) {
      c = fgetc(fin);
      continue;
    }
  } while (c != EOF);

  fclose(fin);
  return EXIT_SUCCESS;
}
