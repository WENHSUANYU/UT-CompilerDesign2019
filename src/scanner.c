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
// or the last acceptable state.
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
#define FLOT_MAX_LEN 64 // not sure @_@
#define CHAR_MAX_LEN 256
#define STRING_MAX_LEN 256
#define OPER_MAX_LEN 3
#define SC_MAX_LEN 256
#define PREP_MAX_LEN 128

typedef enum {
  TC_SC,   // single-line comment
  TC_MC,   // multi-line comment
  TC_PREP, // preprocessor directive
  TC_SPEC, // special symbol
  TC_REWD, // reserved word
  TC_CHAR, // char literal
  TC_STR,  // string literal
  TC_FLOT, // float
  TC_OPER, // operator
  TC_IDEN, // identifier
  TC_INTE, // interger literal
  TC_LAST
} TokenClass;

// Lex functions prototypes
bool scan_sc(FILE* f);
bool scan_mc(FILE* f);
bool scan_prep(FILE* f);
bool scan_spec(FILE* f);
bool scan_rewd(FILE* f);
bool scan_char(FILE* f);
bool scan_str(FILE* f);
bool scan_flot(FILE* f);
bool scan_oper(FILE* f);
bool scan_iden(FILE* f);
bool scan_inte(FILE* f);

void get_next_token();

// Utility functions prototypes
void ungets(char* s, FILE* f);
bool is_newline(char c);
bool is_whitespace(char c);
bool is_alphabet(char c);
bool is_digit(char c);
bool is_underscore(char c);


// Array of lex function pointers.
// get_next_token(FILE* f) will call these functions in the following order.
static bool (*lex[TC_LAST])(FILE* f) = {
  [TC_SC]   = scan_sc,
  [TC_MC]   = scan_mc,
  [TC_PREP] = scan_prep,
  [TC_SPEC] = scan_spec,
  [TC_REWD] = scan_rewd,
  [TC_CHAR] = scan_char,
  [TC_STR]  = scan_str,
  [TC_FLOT] = scan_flot,
  [TC_OPER] = scan_oper,
  [TC_IDEN] = scan_iden,
  [TC_INTE] = scan_inte
};

void get_next_token(FILE* f) {
  // Iterate through the array of lexing function pointers.
  // If any lexing function returns true, it means that
  // a suitable token is found, hence we can return at once.
  for (size_t i = 0; i < TC_LAST; i++) {
    if (lex[i](f)) {
      return;
    }
  }
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
  char c = fgetc(f);

  if (is_alphabet(c) || is_underscore(c)) {
    char str[IDEN_MAX_LEN] = {0};
    size_t current = 0;

    // Advance cursor
    while (is_alphabet(c) || is_underscore(c) || is_digit(c)) {
      str[current++] = c;
      c = fgetc(f);
    }
    ungetc(c, f);
    printf("IDEN: %s\n", str);
    return true;
  } else {
    ungetc(c, f);
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
  char c = fgetc(f);

  if (is_digit(c)) {
    char buf[INTE_MAX_LEN] = {0};
    size_t current = 0;

    // Advance cursor
    while (is_digit(c)) {
      buf[current++] = c;
      c = fgetc(f);
    }
    ungetc(c, f);
    printf("INTE: %s\n", buf);
    return true;
  } else {
    ungetc(c, f);
    return false;
  }
}

// Float
bool scan_flot(FILE* f) {
  char buf[FLOT_MAX_LEN] = {0};
  size_t current = 0;

  // A single '+' or '-' at the beginning is optional
  char c = fgetc(f);
  if (c == '+' || c == '-') {
    buf[current++] = c;
  } else {
    ungetc(c, f);
  }

  c = fgetc(f);
  buf[current++] = c;

  // Match (D*.D+ | D+.D*)
  if (is_digit(c)) { // D+.D*
    // Keep reading until a decimal point is found
    do {
      c = fgetc(f);
      buf[current++] = c;
    } while (is_digit(c));
    
    // c should be a decimal point
    if (c != '.') {
      // Let scan_inte() takes care of it
      ungets(buf, f);
      return false;
    }

    do {
      c = fgetc(f);
      buf[current++] = c;
    } while (is_digit(c));
  } else if (c == '.') { // D*.D+
    c = fgetc(f);
    buf[current++] = c;
    if (is_digit(c)) {
      do {
        c = fgetc(f);
        buf[current++] = c;
      } while (is_digit(c));
    } else {
      ungets(buf, f);
      return false;
    }
  } else {
    ungets(buf, f);
    return false;
  }

  // one char before E|e
  // because -1 is 'E' or 'e' (or maybe some other char...)
  // so -2 is the last accepted state (we'll cache the pointer here)
  char* checkpoint = &buf[current - 2]; 

  // Match (lambda | ((E|e) (+|-|lambda) D+))
  if (c != 'E' && c != 'e') { // lambda
    ungetc(c, f); // backtrack
    buf[--current] = 0x00;
    printf("FLOT: %s\n", buf);
    return true;
  } else { // (+|-|lambda) D+
    c = fgetc(f);

    if (c == '+' || c == '-') {
      buf[current++] = c;
      c = fgetc(f);
    }

    if (is_digit(c)) {
      while (is_digit(c)) {
        buf[current++] = c;
        c = fgetc(f);
      }
      ungetc(c, f);
      printf("FLOT: %s\n", buf);
      return true;
    } else {
      // Backtrack to the last accepted state, and
      // clear all data after checkpoint in reverse order
      // e.g., 3.e -> we want to wipe 'e' and leave "3." there
      ungetc(c, f);
      char* ptr = &buf[current - 1];
      while (ptr > checkpoint) {
        ungetc(*ptr, f);
        *(ptr--) = 0x00;
      }
      printf("FLOT: %s\n", buf);
      return true;
    }
  }
}

// Char literal
bool scan_char(FILE* f) {
  char c = fgetc(f);

  if (c == '\'') {
    char buf[CHAR_MAX_LEN] = {0};
    size_t current = 0;

    c = fgetc(f);
    while (c != '\'' && !is_newline(c)) {
      buf[current++] = c;
      c = fgetc(f);
    }

    // If nothing is in single quotes, print error message and return.
    if (strlen(buf) == 0) {
      printf("ERROR: expected at least one char literal\n");
      return true;
    }

    if (c == '\'') {
      printf("CHAR: %s\n", buf);
    } else {
      printf("ERROR: missing '\n");
      ungetc(c, f); // backtrack
      ungets(buf, f);
    }
    return true;
  } else {
    ungetc(c, f);
    return false;
  }
}

// String literal
bool scan_str(FILE* f) {
  char c = fgetc(f);

  if (c == '"') {
    char buf[CHAR_MAX_LEN] = {0};
    size_t current = 0;

    // Read until the other " or newline
    c = fgetc(f);
    while (c != '"' && !is_newline(c)) {
      buf[current++] = c;
      c = fgetc(f);
    }

    if (c == '"') {
      printf("STR: %s\n", buf);
    } else {
      printf("ERROR: missing \"\n");
      ungetc(c, f); // backtrack
      ungets(buf, f);
    }
    return true;
  } else {
    ungetc(c, f);
    return false;
  }
}

// Operator
bool scan_oper(FILE* f) {
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
  char c = fgetc(f);

  if (c == '{' || c == '}' || c == '(' || c ==')' || c ==';') {
    printf("SPEC: %c\n", c);
    return true;
  } else {
    ungetc(c, f);
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
  static const char* mc_closing_symbol = "*/";
  char buf[strlen(mc_opening_symbol) + 1];
  memset(buf, 0x00, sizeof(buf));

  fgets(buf, sizeof(buf), f);
  if (!strcmp(mc_opening_symbol, buf)) {
    // Read until */ is seen
    char c = 0x00;
    do {
      c = fgetc(f);
      if (c == mc_closing_symbol[0]) { // *
        c = fgetc(f);
        if (c == mc_closing_symbol[1]) { // /
          printf("MC: \n");
          return true;
        }
      }
    } while (c != EOF);
    printf("ERROR: missing %c%c\n", mc_closing_symbol[0], mc_closing_symbol[1]);
    return true;
  } else {
    ungets(buf, f);
    return false;
  }
}

// Preprocessor directive
bool scan_prep(FILE* f) {
  char buf[PREP_MAX_LEN] = {0};
  size_t current = 0;

  char c = fgetc(f);
  if (c == '#') { // #
    buf[current++] = c;

    // Skip whitespaces between # and include
    c = fgetc(f);
    while (is_whitespace(c)) {
      buf[current++] = c;
      c = fgetc(f);
    }
    ungetc(c, f);

    // Try to get "include" from ifstream
    fgets(buf + current, strlen("include") + 1, f);

    // Copy "include" to buf (if found)
    if (!strcmp(buf + current, "include")) {
      strcpy(buf + current, "include");
      current += strlen("include");
    } else {
      printf("ERROR: expected \"include\"\n");
      ungets("include", f);
      return false;
    }

    // Skip whitespaces between include and < or "
    c = fgetc(f);
    while (is_whitespace(c)) {
      buf[current++] = c;
      c = fgetc(f);
    }

    // Determine the closing symbol
    buf[current++] = c;
    char closing_symbol = 0x00;
    if (c == '<') { // <
      closing_symbol = '>';
    } else if (c == '"') {
      closing_symbol = '"';
    } else {
      ungetc(c, f);
      printf("ERROR: expected < or \"\n");
      return true;
    }

    // Read until the closing symbol or newline
    do {
      c = fgetc(f);
      buf[current++] = c;
    } while (c != closing_symbol && !is_newline(c));

    if (c == closing_symbol) {
      printf("PREP: %s\n", buf);
    } else {
      printf("ERROR: missing %c\n", closing_symbol);
    }
    return true;
  } else {
    ungetc(c, f);
    return false;
  }
}



int main(int argc, char* args[]) {
  if (argc < 2) {
    printf("usage: %s <c source file>\n", args[0]);
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
    c = fgetc(fin);
    ungetc(c, fin);
    if (is_whitespace(c)) {
      c = fgetc(fin);
      continue;
    }
  } while (c != EOF);

  fclose(fin);
  return EXIT_SUCCESS;
}
