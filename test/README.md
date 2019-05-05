# Scanner Unit Test
`REWD`, `SPEC` and `OPER` are not covered in unit tests.

## Preview
1. SC (Single-line Comment)
```
// A single line comment
//a Single line comment
```
```
1	SC	// A single line comment
2	SC	//a Single line comment
```

2. MC (Multi-line Comment)
```
/* Lol
 * multiline
 */
/* Single line*/
/* Runaway
 * comment
```
```
1-3	MC
4	MC
5-6	MC	ERROR: missing */
```

3. PREP (Preprocessor Directive)
```
#include<stdio.h>
#include <stdio.h>
# include <stdlib.h>
#    include   <stdlib.h>
  # include  <stdlib.h>
#include"stdlib.h"
#include "stdlib.h"
# include "stdio.h"
#   include  "stdlib.h"
#include <stdlib.h
#include "stdlib.h
```
```
1	PREP	#include<stdio.h>
2	PREP	#include <stdio.h>
3	PREP	# include <stdlib.h>
4	PREP	#    include   <stdlib.h>
5	PREP	# include  <stdlib.h>
6	PREP	#include"stdlib.h"
7	PREP	#include "stdlib.h"
8	PREP	# include "stdio.h"
9	PREP	#   include  "stdlib.h"
11	PREP	#include <stdlib.h	ERROR: missing >
12	PREP	#include "stdlib.h	ERROR: missing "
```

4. IDEN (Identifier)
```
some_interesting_var
v
aa66
_wow
lol_cat
__hi__
a_ha__
25aaa
```
```
1	IDEN	some_interesting_var
2	IDEN	v
3	IDEN	aa66
4	IDEN	_wow
5	IDEN	lol_cat
6	IDEN	__hi__
7	IDEN	a_ha__
8	INTE	25
8	IDEN	aaa
```

5. FLOT (Float Literal)
```
.5
3.
3.5E+5
3.5e5
3.5e-5
3.5e50
1.0e-n
```
```
1	FLOT	.5
2	FLOT	3.
3	FLOT	3.5E+5
4	FLOT	3.5e5
5	FLOT	3.5e-5
6	FLOT	3.5e50
7	FLOT	1.0
7	IDEN	e
7	OPER	-
7	IDEN	n
```

6. INTE (Integer LIteral)
```
0
123
0x0
0x00
0x0f
0xff
0xcafebabe
0xdeadbeef
00
07
0234
0056
0x
08
09
```
```
1	INTE	0
2	INTE	123
3	INTE	0x0
4	INTE	0x00
5	INTE	0x0f
6	INTE	0xff
7	INTE	0xcafebabe
8	INTE	0xdeadbeef
9	INTE	00
10	INTE	07
11	INTE	0234
12	INTE	0056
14	INTE	0
14	IDEN	x
15	INTE	0
15	INTE	8
16	INTE	0
16	INTE	9
```

7. CHAR (Char Literal)
```
'c'
'a'
'\n'
'\t'
'widechar'
'o
```
```
1	CHAR	c
2	CHAR	a
3	CHAR	

4	CHAR		
5	CHAR	widechar
7	CHAR	o	ERROR: missing '
```

8. STR (String Literal)
```
"hello world"
"newline here\nwow"
"contains\ttab\ts"
"multi-line\
string\ttab here\
very cool"
```
```
1	STR	hello world
2	STR	newline here
wow
3	STR	contains	tab	s
4-6	STR	multi-linestring	tab herevery cool
```
