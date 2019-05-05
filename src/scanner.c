// Copyright 2019 Marco Wang <m.aesophor@gmail.com>
// 
// Implementation note:
// 
// scanner.c is an Ad-Hoc scanner for C-like language which converts a sequence of
// characters into a sequence of tokens.
//
// It contains various functions to get the next token from the input file stream
// (ifstream for abbreviation in the following context). For each token class,
// there's a dedicated tokenizing function.
//
// During tokenization, if the current character is accepted, then we advance
// ifstream's position by one char. If not, we backtrack ifstream's position
// to the last acceptable checkpoint (if possible) or to the original position.
//
// In order to get the next token from ifstream, we'll try all tokenizing functions
// one by one (THEIR ORDER MATTERS!). If a tokenizing function returns true,
// then an acceptable token has been found, and thus we can return immediately.
// Otherwise (if it returns false) we'll have to try the next tokenizing function
// until one finally returns true.
 
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

#define DEFAULT_OUTPUT_FILENAME "output.txt"

enum {
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
};

static void get_next_token(FILE* fin, FILE* fout);

// Lex functions prototypes
static bool scan_sc(FILE* fin, FILE* fout);
static bool scan_mc(FILE* fin, FILE* fout);
static bool scan_prep(FILE* fin, FILE* fout);
static bool scan_spec(FILE* fin, FILE* fout);
static bool scan_rewd(FILE* fin, FILE* fout);
static bool scan_char(FILE* fin, FILE* fout);
static bool scan_str(FILE* fin, FILE* fout);
static bool scan_flot(FILE* fin, FILE* fout);
static bool scan_oper(FILE* fin, FILE* fout);
static bool scan_iden(FILE* fin, FILE* fout);
static bool scan_inte(FILE* fin, FILE* fout);

// Utility functions prototypes
static void ungets(char* s, FILE* fin);
static bool is_newline(char c);
static bool is_whitespace(char c);
static bool is_alphabet(char c);
static bool is_digit(char c);
static bool is_underscore(char c);
static bool is_hex_digit(char c);
static char get_escaped_char(char c);


// Array of lex function pointers.
// get_next_token(FILE* f) will call these functions in the following order.
static bool (*lex[TC_LAST])(FILE* fin, FILE* fout) = {
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

static unsigned int line_number = 1;


static void
get_next_token(FILE* fin, FILE* fout) {
  // Iterate through the array of lexing function pointers.
  // If any lexing function returns true, it means that
  // a suitable token is found, hence we can return at once.
  for (size_t i = 0; i < TC_LAST; i++) {
    if (lex[i](fin, fout)) {
      return;
    }
  }
}


// Identifier
static bool
scan_iden(FILE* fin, FILE* fout) {
  // 第一個字必須是英文字母或底線字元
  // 由英文字母、底線及數字組成, 長度不限
  char c = fgetc(fin);

  if (is_alphabet(c) || is_underscore(c)) {
    char str[IDEN_MAX_LEN] = {0};
    size_t current = 0;

    // Advance cursor
    while (is_alphabet(c) || is_underscore(c) || is_digit(c)) {
      str[current++] = c;
      c = fgetc(fin);
    }
    ungetc(c, fin);
    fprintf(fout, "IDEN: %s\n", str);
    return true;
  } else {
    ungetc(c, fin);
    return false;
  }
}

// Reserved word
static bool
scan_rewd(FILE* fin, FILE* fout) {
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

    if (fgets(buf, sizeof(buf), fin) && !strcmp(buf, rewd)) {
      fprintf(fout, "REWD: %s\n", buf);
      return true;
    } else {
      ungets(buf, fin);
    }
  }
  return false;
}

// Integer
static bool
scan_inte(FILE* fin, FILE* fout) {
  char buf[INTE_MAX_LEN] = {0};
  size_t current = 0;

  // 0 -> decimal 0
  // 234 -> decimal 234
  // 0xff -> hex
  // 023 -> octal
  char c = fgetc(fin);
  buf[current++] = c;

  if (is_digit(c)) {
    if (c == '0') { // hex, octal or decimal 0
      c = fgetc(fin);
      buf[current++] = c;
      if (c == 'x' || c == 'X') {
        // Must have at least one hex digit
        c = fgetc(fin);
        buf[current++] = c;
        if (!is_hex_digit(c)) { // (hex) first char after 0x is invalid, e.g., 0xp
          ungets(&buf[current] - 2, fin);
          fprintf(fout, "INTE: 0\n");
          return true;
        } else { // (hex) first char after 0x is valid, e.g., 0xff, 0xffp
          do {
            c = fgetc(fin);
            buf[current++] = c;
          } while (is_hex_digit(c));
          ungetc(c, fin);
          buf[current - 1] = 0x00;
          fprintf(fout, "INTE: %s\n", buf);
          return true;
        }
      } else if (c >= '0' && c <= '7') { // (octal) first char after 0 is valid
        do {
          c = fgetc(fin);
          buf[current++] = c;
        } while (c >= '0' && c <= '7');
        ungetc(c, fin);
        buf[current - 1] = 0x00;
        fprintf(fout, "INTE: %s\n", buf);
        return true;
      } else { // (octal / dec 0) first char after 0 is invalid
        ungetc(c, fin);
        fprintf(fout, "INTE: 0\n");
        return true;
      }
    } else { // c >= '1' && c <= '9'
      do {
        c = fgetc(fin);
        buf[current++] = c;
      } while (is_digit(c));
      ungetc(c, fin);
      buf[current - 1] = 0x00;
      fprintf(fout, "INTE: %s\n", buf);
      return true;
    } 
  } else {
    ungetc(c, fin);
    return false;
  }
}

// Float
static bool
scan_flot(FILE* fin, FILE* fout) {
  // (+|-|lambda) (D*.D+ | D+.D*) (lambda | ((E|e) (+|-|lambda) D+))
  char buf[FLOT_MAX_LEN] = {0};
  size_t current = 0;

  // A single '+' or '-' at the beginning is optional
  char c = fgetc(fin);
  if (c == '+' || c == '-') {
    buf[current++] = c;
  } else {
    ungetc(c, fin);
  }

  c = fgetc(fin);
  buf[current++] = c;

  // Match (D*.D+ | D+.D*)
  if (is_digit(c)) { // D+.D*
    // Keep reading until a decimal point is found
    do {
      c = fgetc(fin);
      buf[current++] = c;
    } while (is_digit(c));
    
    // c should be a decimal point
    if (c != '.') {
      // Let scan_inte() takes care of it
      ungets(buf, fin);
      return false;
    }

    do {
      c = fgetc(fin);
      buf[current++] = c;
    } while (is_digit(c));
  } else if (c == '.') { // D*.D+
    c = fgetc(fin);
    buf[current++] = c;
    if (is_digit(c)) {
      do {
        c = fgetc(fin);
        buf[current++] = c;
      } while (is_digit(c));
    } else {
      ungets(buf, fin);
      return false;
    }
  } else {
    ungets(buf, fin);
    return false;
  }

  // one char before E|e
  // because -1 is 'E' or 'e' (or maybe some other char...)
  // so -2 is the last accepted state (we'll cache the pointer here)
  char* checkpoint = &buf[current - 2]; 

  // Match (lambda | ((E|e) (+|-|lambda) D+))
  if (c != 'E' && c != 'e') { // lambda
    ungetc(c, fin); // backtrack
    buf[--current] = 0x00;
    fprintf(fout, "FLOT: %s\n", buf);
    return true;
  } else { // (+|-|lambda) D+
    c = fgetc(fin);

    if (c == '+' || c == '-') {
      buf[current++] = c;
      c = fgetc(fin);
    }

    if (is_digit(c)) {
      while (is_digit(c)) {
        buf[current++] = c;
        c = fgetc(fin);
      }
      ungetc(c, fin);
      fprintf(fout, "FLOT: %s\n", buf);
      return true;
    } else {
      // Backtrack to the last accepted state, and
      // clear all data after checkpoint in reverse order
      // e.g., 3.e -> we want to wipe 'e' and leave "3." there
      ungetc(c, fin);
      char* ptr = &buf[current - 1];
      while (ptr > checkpoint) {
        ungetc(*ptr, fin);
        *(ptr--) = 0x00;
      }
      fprintf(fout, "FLOT: %s\n", buf);
      return true;
    }
  }
}

// Char literal
static bool
scan_char(FILE* fin, FILE* fout) {
  char c = fgetc(fin);

  if (c == '\'') {
    char buf[CHAR_MAX_LEN] = {0};
    size_t current = 0;

    c = fgetc(fin);
    while (c != '\'' && !is_newline(c)) {
      buf[current++] = c;
      c = fgetc(fin);
    }

    // If nothing is in single quotes, print error message and return.
    if (strlen(buf) == 0) {
      fprintf(fout, "CHAR: ERROR: expected at least one char literal\n");
      return true;
    }

    if (c == '\'') {
      fprintf(fout, "CHAR: %s\n", buf);
    } else {
      fprintf(fout, "CHAR: %s ERROR: missing '\n", buf);
    }
    return true;
  } else {
    ungetc(c, fin);
    return false;
  }
}

// String literal
static bool
scan_str(FILE* fin, FILE* fout) {
  char buf[CHAR_MAX_LEN] = {0};
  size_t current = 0;

  char c = fgetc(fin);
  if (c == '"') {
    // Read until the other " or newline
    c = fgetc(fin);
    while (c != '"' && !is_newline(c)) {
      if (c == '\\') {
        c = fgetc(fin);
        if (c == '\n') { // multi-line string
          // Read until next non-whitespace char
          do {
            c = fgetc(fin);
          } while (is_whitespace(c));
        } else { // escape this char
          c = get_escaped_char(c);
        }
      }
      buf[current++] = c;
      c = fgetc(fin);
    }

    if (c == '"') {
      fprintf(fout, "STR: %s\n", buf);
    } else {
      fprintf(fout, "STR: %s ERROR: missing \"\n", buf);
    }
    return true;
  } else {
    ungetc(c, fin);
    return false;
  }
}

// Operator
static bool
scan_oper(FILE* fin, FILE* fout) {
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

    if (fgets(buf, sizeof(buf), fin) && !strcmp(buf, oper)) {
      fprintf(fout, "OPER: %s\n", buf);
      return true;
    } else {
      ungets(buf, fin);
    }
  }
  return false;
}

// Special symbol
static bool
scan_spec(FILE* fin, FILE* fout) {
  char c = fgetc(fin);

  if (c == '{' || c == '}' || c == '(' || c ==')' || c ==';') {
    fprintf(fout, "SPEC: %c\n", c);
    return true;
  } else {
    ungetc(c, fin);
    return false;
  }
}

// Single line comment
static bool
scan_sc(FILE* fin, FILE* fout) {
  static const char* sc_symbol = "//";
  char buf[strlen(sc_symbol) + 1];
  memset(buf, 0x00, sizeof(buf));

  fgets(buf, sizeof(buf), fin);
  if (!strcmp(sc_symbol, buf)) {
    ungets(buf, fin); // put // back to ifstream
    char content[SC_MAX_LEN] = {0};
    size_t current = 0;

    // Read until newline or EOF
    char c = 0x00;
    do {
      c = fgetc(fin);
      content[current++] = c;
    } while (!is_newline(c) && c != EOF);
    content[current - 1] = 0x00;
    fprintf(fout, "SC: %s\n", content);
    return true;
  } else {
    ungets(buf, fin);
    return false;
  }
}

// Multi line comment
static bool
scan_mc(FILE* fin, FILE* fout) {
  char buf[strlen("/*") + 1];
  memset(buf, 0x00, sizeof(buf));

  fgets(buf, sizeof(buf), fin);
  if (!strcmp("/*", buf)) {
    // Read until */ is seen
    char c = 0x00;
    do {
      c = fgetc(fin);
      if (c == '*') {
        c = fgetc(fin);
        if (c == '/') {
          fprintf(fout, "MC: \n");
          return true;
        }
      }
    } while (c != EOF);
    fprintf(fout, "MC: ERROR: missing */\n");
    return true;
  } else {
    ungets(buf, fin);
    return false;
  }
}

// Preprocessor directive
static bool
scan_prep(FILE* fin, FILE* fout) {
  char buf[PREP_MAX_LEN] = {0};
  size_t current = 0;

  char c = fgetc(fin);
  if (c == '#') { // #
    buf[current++] = c;

    // Skip whitespaces between # and include
    c = fgetc(fin);
    while (is_whitespace(c)) {
      buf[current++] = c;
      c = fgetc(fin);
    }
    ungetc(c, fin);

    // Try to get "include" from ifstream
    fgets(buf + current, strlen("include") + 1, fin);

    // Copy "include" to buf (if found)
    if (!strcmp(buf + current, "include")) {
      strcpy(buf + current, "include");
      current += strlen("include");
    } else {
      fprintf(fout, "PREP: %s ERROR: expected \"include\"\n", buf);
      ungets("include", fin);
      return false;
    }

    // Skip whitespaces between include and < or "
    c = fgetc(fin);
    while (is_whitespace(c)) {
      buf[current++] = c;
      c = fgetc(fin);
    }

    // Determine the closing symbol
    buf[current++] = c;
    char closing_symbol = 0x00;
    if (c == '<') { // <
      closing_symbol = '>';
    } else if (c == '"') {
      closing_symbol = '"';
    } else {
      ungetc(c, fin);
      fprintf(fout, "PREP: %s ERROR: expected < or \"\n", buf);
      return true;
    }

    // Read until the closing symbol or newline
    do {
      c = fgetc(fin);
      buf[current++] = c;
    } while (c != closing_symbol && !is_newline(c));

    if (c == closing_symbol) {
      fprintf(fout, "PREP: %s\n", buf);
    } else {
      fprintf(fout, "PREP: %s ERROR: missing %c\n", buf, closing_symbol);
    }
    return true;
  } else {
    ungetc(c, fin);
    return false;
  }
}


// Utility functions
static void
ungets(char* s, FILE* fin) {
  for (int i = strlen(s) - 1; i >= 0; i--) {
    ungetc(s[i], fin);
  }
}

static bool
is_newline(char c) {
  return c == 0xd || c == 0xa;
}

static bool
is_whitespace(char c) {
  return c == ' ' || c == '\t' || is_newline(c);
}

static bool
is_alphabet(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static bool
is_digit(char c) {
  return c >= '0' && c <= '9';
}

static bool
is_underscore(char c) {
  return c == '_';
}

static bool
is_hex_digit(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static char
get_escaped_char(char c) {
  // According to: https://en.wikipedia.org/wiki/Escape_sequences_in_C
  switch (c) {
    case 'a':
      return 0x07;
    case 'b':
      return 0x08;
    case 'e':
      return 0x1b;
    case 'f':
      return 0x0c;
    case 'n':
      // Only maps to 0xa in memory. Translation to 0xd 0xa on DOS/Windows
      // happens when writing to a file or stdout.
      return 0x0a;
    case 'r':
      return 0x0d;
    case 't':
      return 0x09;
    case 'v':
      return 0x0b;
    case '\\':
      return 0x5c;
    case '\'':
      return 0x27;
    case '"':
      return 0x22;
    case '?':
      return 0x3f;
    default:
      return c;
  }
}


int
main(int argc, char* args[]) {
  if (argc <= 1 || argc >= 4) {
    printf("usage: %s <input file> <output file>\n", args[0]);
    return EXIT_SUCCESS;
  }

  // Open input file from args[1]
  FILE* fin = fopen(args[1], "r");
  if (!fin) {
    perror("Fatal error");
    return EXIT_FAILURE;
  }

  // Open output file
  const char* output_filename = DEFAULT_OUTPUT_FILENAME;
  if (argc == 3) {
    output_filename = args[2];
  }
  FILE* fout = fopen(output_filename, "w");
  if (!fout) {
    perror("Fatal error");
    return EXIT_FAILURE;
  }


  // Main tokenizing loop
  char c = 0x00;

  do {
    // if successful, FILE position will be advanced
    get_next_token(fin, fout);

    // One character lookahead.
    c = fgetc(fin);
    ungetc(c, fin);

    // If it is a whitespace (space, tab, or newline),
    // then just advance it.
    if (is_whitespace(c)) {
      line_number += (is_newline(c)) ? 1 : 0;
      c = fgetc(fin);
      continue;
    }
  } while (c != EOF);

  fclose(fin);
  fclose(fout);
  printf("Output has been written to: %s\n", output_filename);
  return EXIT_SUCCESS;
}
