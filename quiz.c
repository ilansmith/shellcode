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
	((unsigned long*)buf)[0] = 0xffffffffff636261; /* dst             */
	((unsigned long*)buf)[1] = 0xffffffffffffffff; /* unused          */
	((unsigned long*)buf)[2] = 0xffffffffffffffff; /* previous rbp    */
	((unsigned long*)buf)[3] = (unsigned long)&g;  /* return address  */
	*(buf + 4*sizeof(unsigned long)) = 0x0;        /* null terminator */

	f(buf);
	return 0;
}

