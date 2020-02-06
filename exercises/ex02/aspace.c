#include <stdio.h>
#include <stdlib.h>

int var1;

void printLocalVariable() {
	int a = 0;
	printf("Address of local variable a is %p\n", &a);
}

int main ()
{
    int var2 = 5;
    void *p = malloc(128);
    char *s = "Hello, World";

    printf ("Address of main is %p\n", main);
    printf ("Address of var1 is %p\n", &var1);
    printf ("Address of var2 is %p\n", &var2);
    printf ("p points to %p\n", p);
    printf ("s points to %p\n", s);

  p = malloc(10);
	printf ("Address of p is now %p\n\n", p);

	printLocalVariable();

	void *b = malloc(10);
	void *c = malloc(10);

	printf ("Address of b is %p\n", b);
	printf ("Address of c is %p\n\n", c);

    return 0;
}
