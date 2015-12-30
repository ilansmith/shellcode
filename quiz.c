#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void g(void)
{
	printf("In g()!\n");
	exit(0);
}

void f(char *src)
{
	char dst[4];

	strcpy(dst, src);
	dst[3] = '\0';
	printf("dst: %s\n", dst);
}

int main(int argc, char *argv[])
{
	char buf[40] = "abc";

	/* Constraints
	   - May insert code only here
	   - Must not call g() directly
	*/

	f(buf);
	return 0;
}

