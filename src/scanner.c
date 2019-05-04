// Copyright 2019 Marco Wang <m.aesophor@gmail.com>
// 
// Implementation note:
// 
// scanner.c converts a sequence of characters into a sequence of tokens.
//
// It contains various functions to get the next token from the input file stream
// (ifstream for abbreviation in the following context). For each token class,
// there's a dedicated tokenizing function.
//
// During tokenization, if current character is accepted, then we advance
// ifstream's position by one char. If not accepted, we keep backtracking
// ifstream's position by one char until it reaches the original postion
// or an accepted state is reached.
//
// In order to get the next token from ifstream, we'll try all tokenizing functions
// one by one. If after executing a tokenizing function, the position of ifstream
// remains the same, then we'll try another one until all functions have been tried.
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define IDEN_MAX_LEN 32
#define REWD_MAX_LEN 9
#define INTE_MAX_LEN 12
#define OPER_MAX_LEN 3
#define SC_MAX_LEN 256
#define MC_MAX_LEN 256

char fpeek(FILE* f) {
  char c = fgetc(f);
  ungetc(c, f);
  return c;
}

void ungets(char* s, FILE* f) {
  for (int i = strlen(s) - 1; i >= 0; i--) {
    ungetc(s[i], f);
  }
}


bool is_newline(char c) {
  return c == 0xd || c == 0xa;
}

bool is_whitespace(char c) {
  return c == ' ' || c == '\t' || is_newline(c);
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
    char str[IDEN_MAX_LEN] = {0};
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
    printf("IDEN: %s\n", str);
    return true;
  } else {
    return false;
  }
}

// Reserved word
bool scan_rewd(FILE* f) {
  static const char rewds[][REWD_MAX_LEN] = {
    "if", "else", "while", "for", "do", "switch", "case", "default",
    "continue", "int", "float", "double", "char", "break", "static",
    "extern", "auto", "register", "sizeof", "union", "struct", "enum",
    "return", "goto", "const"
  };
  static const size_t rewds_size = sizeof(rewds) / sizeof(rewds[0]);
  
  for (size_t i = 0; i < rewds_size; i++) {
    const char* rewd = rewds[i];
    const size_t rewd_size = strlen(rewd);

    char buf[rewd_size + 1];
    memset(buf, 0x00, rewd_size + 1);

    if (fgets(buf, sizeof(buf), f) && !strcmp(buf, rewd)) {
      printf("REWD: %s\n", buf);
      return true;
    } else {
      ungets(buf, f);
    }
  }
  return false;
}

// Integer
bool scan_inte(FILE* f) {
  char c = fpeek(f);

  if (is_digit(c)) {
    char str[INTE_MAX_LEN] = {0};
    size_t current = 0;

    // Advance cursor
    do {
      c = fgetc(f);
      str[current++] = c;
    } while (is_digit(c));

    // Backtrack
    ungetc(c, f);
    str[current - 1] = 0x00;
    printf("INTE: %s\n", str);
    return true;
  } else {
    return false;
  }
}

// Operator
bool scan_operator(FILE* f) {
  static const char opers[][OPER_MAX_LEN] = {
    ">>", "<<", "++", "--", "+=", "-=", "*=", "/=", "%=", "&&", "||",
    "->", "==", ">=", "<=", "!=",
    "+", "-", "*", "/", "=", ",", "%", "!", "&", "[", "]", "|", "^",
    ".", ">", "<", ":", "?"
  };
  static const size_t opers_size = sizeof(opers) / sizeof(opers[0]);
  
  for (size_t i = 0; i < opers_size; i++) {
    const char* oper = opers[i];
    const size_t oper_size = strlen(oper);

    char buf[oper_size + 1];
    memset(buf, 0x00, oper_size + 1);

    if (fgets(buf, sizeof(buf), f) && !strcmp(buf, oper)) {
      printf("OPER: %s\n", buf);
      return true;
    } else {
      ungets(buf, f);
    }
  }
  return false;
}

// Special symbol
bool scan_spec(FILE* f) {
  char c = fpeek(f);

  if (c == '{' || c == '}' || c == '(' || c ==')' || c ==';') {
    printf("SPEC: %c\n", c);
    fgetc(f);
    return true;
  } else {
    return false;
  }
}

// Single line comment
bool scan_sc(FILE* f) {
  static const char* sc_symbol = "//";
  char buf[strlen(sc_symbol) + 1];
  memset(buf, 0x00, sizeof(buf));

  fgets(buf, sizeof(buf), f);
  if (!strcmp(sc_symbol, buf)) {
    ungets(buf, f); // put // back to ifstream
    char content[SC_MAX_LEN] = {0};
    size_t current = 0;

    // Read until newline or EOF
    char c = 0x00;
    do {
      c = fgetc(f);
      content[current++] = c;
    } while (!is_newline(c) && c != EOF);
    content[current - 1] = 0x00;
    printf("SC: %s\n", content);
    return true;
  } else {
    ungets(buf, f);
    return false;
  }
}

// Multi line comment
bool scan_mc(FILE* f) {
  static const char* mc_opening_symbol = "/*";
  char buf[strlen(mc_opening_symbol) + 1];
  memset(buf, 0x00, sizeof(buf));

  fgets(buf, sizeof(buf), f);
  if (!strcmp(mc_opening_symbol, buf)) {
    // Read until */ is seen
    char c = 0x00;
    do {
      c = fgetc(f);
      if (c == '*') {
        c = fgetc(f);
        if (c == '/') {
          printf("MC: \n");
          return true;
        }
      }
    } while (c != EOF);
    printf("ERROR: missing */\n");
    return true;
  } else {
    ungets(buf, f);
    return false;
  }
}


bool get_next_token(FILE* f) {
  bool result = scan_sc(f);
  if (result) return true;

  result = scan_mc(f);
  if (result) return true;

  result = scan_spec(f);
  if (result) return true;

  result = scan_rewd(f);
  if (result) return true;

  result = scan_operator(f);
  if (result) return true;
  
  result = scan_iden(f);
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
