#include <stdio.h>

#define get_stack() do { \
	printf("%s():\n", __FUNCTION__); \
	__asm__("lea 0x8(%rbp),%rax"); \
	printf("  &(return address): 0x%lx\n", __stack_get()); \
	__asm__("mov %rbp,%rax"); \
	printf("  %%rbp:              0x%lx\n", __stack_get()); \
	__asm__("mov %rsp,%rax"); \
	printf("  %%rsp:              0x%lx\n", __stack_get()); \
} while (0)

static unsigned long __stack_get(void)
{
}

static void func(void)
{
	int i = 0;
	get_stack();
}

int main(int argc, char **argv)
{
	get_stack();
	func();

	return 0;
}

