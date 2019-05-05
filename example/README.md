# Examples

## 01.c
```
//This is a sample program file for a C-like simple scanner
main()
{
/* this is a multiline comment
  Hahaha
  Still another line comment */
int i, j = 0;
for (I = 0; I = 100; i++)
  printf("%d\n", i);
}
#include <stdio.h>
```

```
1	SC	//This is a sample program file for a C-like simple scanner
2	IDEN	main
2	SPEC	(
2	SPEC	)
3	SPEC	{
4-6	MC
7	REWD	int
7	IDEN	i
7	OPER	,
7	IDEN	j
7	OPER	=
7	INTE	0
7	SPEC	;
8	REWD	for
8	SPEC	(
8	IDEN	I
8	OPER	=
8	INTE	0
8	SPEC	;
8	IDEN	I
8	OPER	=
8	INTE	100
8	SPEC	;
8	IDEN	i
8	OPER	++
8	SPEC	)
9	IDEN	printf
9	SPEC	(
9	STR	%d

9	OPER	,
9	IDEN	i
9	SPEC	)
9	SPEC	;
10	SPEC	}
11	PREP	#include <stdio.h>
```

## 02.c
```
main() {
  if (1 < 2) a = 1.0 else a = 1.0e-n;
  subr('aa', "aaaaaa
              aaaaaa");
  /* That's
}
```

```
1	IDEN	main
1	SPEC	(
1	SPEC	)
1	SPEC	{
2	REWD	if
2	SPEC	(
2	INTE	1
2	OPER	<
2	INTE	2
2	SPEC	)
2	IDEN	a
2	OPER	=
2	FLOT	1.0
2	REWD	else
2	IDEN	a
2	OPER	=
2	FLOT	1.0
2	IDEN	e
2	OPER	-
2	IDEN	n
2	SPEC	;
3	IDEN	subr
3	SPEC	(
3	CHAR	aa
3	OPER	,
4	STR	aaaaaa	ERROR: missing "
4	IDEN	aaaaaa
5	STR	);	ERROR: missing "
5-6	MC	ERROR: missing */
```

## 03.c
```
#  include   <stdio.h>

//This is a sample program file for a C-like simple scanner
int main() {
  /* this is a multiline comment
   Hahaha
   Still another line of comment */
  char c = 'a';
  const char* s = "lolcatwow\nboi";
  int a = 0xcafebabe;
  float num = .2e2;
  if (100 + 3 == 103) {

  } else {

  }

  return 0;
}
```

```
1	PREP	#  include   <stdio.h>
3	SC	//This is a sample program file for a C-like simple scanner
4	REWD	int
4	IDEN	main
4	SPEC	(
4	SPEC	)
4	SPEC	{
5-7	MC
8	REWD	char
8	IDEN	c
8	OPER	=
8	CHAR	a
8	SPEC	;
9	REWD	const
9	REWD	char
9	OPER	*
9	IDEN	s
9	OPER	=
9	STR	lolcatwow
boi
9	SPEC	;
10	REWD	int
10	IDEN	a
10	OPER	=
10	INTE	0xcafebabe
10	SPEC	;
11	REWD	float
11	IDEN	num
11	OPER	=
11	FLOT	.2e2
11	SPEC	;
12	REWD	if
12	SPEC	(
12	INTE	100
12	OPER	+
12	INTE	3
12	OPER	==
12	INTE	103
12	SPEC	)
12	SPEC	{
14	SPEC	}
14	REWD	else
14	SPEC	{
16	SPEC	}
18	REWD	return
18	INTE	0
18	SPEC	;
19	SPEC	}
```
