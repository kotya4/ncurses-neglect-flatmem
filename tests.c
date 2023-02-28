/* inconstant enum

const static int cc = 10;
enum {
	a,
	b,
	c = cc, // expression must have a constant value, enumerator value for ‘c’ is not an integer constant
};

enum {
	a=1, // error: redeclaration
};

int main()
{
	return a;
}

*/

#include <stdio.h>

//typedef int size_t;
char *str_cat(char *a, const char *b)
{
	// neglect(!a || !b);
	size_t i;
	for(i=0;a[i];++i);
	printf("%lu\n",i);
	for(;*b;++b)
	{
		printf(">%c\n",*b);
		a[i++]=*b;
	}
	a[i]=0;
	return a;
}


int main()
{
	char a[20]="hello \0ORLD";
	printf("%s\n",a);
	const char *b="world";
	str_cat(a,b);
	printf("%s\n",a);
	for(int i=0; i<20; ++i)
	{
		printf(" %c ", a[i]);
	}
	printf("\n");
	for(int i=0; i<20; ++i)
	{
		printf(" %2x", a[i]);
	}
	printf("\n");
	return 0;
}

