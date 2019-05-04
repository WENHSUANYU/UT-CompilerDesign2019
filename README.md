# UT-CompilerDesign2019
UTaipei Compiler Design 2019 Spring - Scanner project

## Result
Example source code
```
#include <stdio.h>

//This is a sample program file for a C-like simple scanner
int main() {
  /* this is a multiline comment
   Hahaha
   Still another line of comment */
  char c = 'a';
  const char* s = "lolcat";
  int a = 5;
  float num = .2e2;
  if (100 + 3 == 103) {

  } else {

  }

  return 0;
}
```

Tokenized
```
PREP: #include <stdio.h>
SC: //This is a sample program file for a C-like simple scanner
REWD: int
IDEN: main
SPEC: (
SPEC: )
SPEC: {
MC: 
REWD: char
IDEN: c
OPER: =
CHAR: a
SPEC: ;
REWD: const
REWD: char
OPER: *
IDEN: s
OPER: =
STR: lolcat
SPEC: ;
REWD: int
IDEN: a
OPER: =
INTE: 5
SPEC: ;
REWD: float
IDEN: num
OPER: =
FLOT: .2e2
SPEC: ;
REWD: if
SPEC: (
INTE: 100
OPER: +
INTE: 3
OPER: ==
INTE: 103
SPEC: )
SPEC: {
SPEC: }
REWD: else
SPEC: {
SPEC: }
REWD: return
INTE: 0
SPEC: ;
SPEC: }
```
