#ifndef TOKEN_H_
#define TOKEN_H_

// TokenClass
typedef enum {
  IDEN, // Identifier
  REWD, // Reserved word
  INTE, // Integer
  FLOT, // Float
  CHAR, // Char literal
  STR,  // String literal
  OPER, // Operator
  SPEC, // Special Symbol
  SC,   // Single-line comment
  MC,   // Multi-line comment
  PREP  // Preprocessor directive
} TokenClass;


// Token
typedef struct {
  unsigned int line_number;
  TokenClass token_class;
	char* content;
} Token;

#endif // TOKEN_H_
