#ifndef _GET_STACK_ARGS_H
#define _GET_STACK_ARGS_H

#include <stdio.h>

extern void *ret_addr;
extern void *rbp;
extern void *rsp;

#define copy_regs() do { \
	__asm__("mov 0x8(%%rbp), %0" : "=r"(ret_addr)); \
	__asm__("mov %%rbp, %0" : "=r"(rbp)); \
	__asm__("mov %%rsp, %0" : "=r"(rsp)); \
} while (0)

#define get_stack() do { \
	copy_regs(); \
	printf("%s():\n", __FUNCTION__); \
	printf("  *($rbp+8):   %#018lx (return addr)\n", \
		(unsigned long)ret_addr); \
	printf("  $rbp:       %#018lx\n", (unsigned long)rbp); \
	printf("  ...\n"); \
	printf("  $rsp:       %#018lx\n", (unsigned long)rsp); \
} while (0)

#define get_stack_var(_var_) do { \
	copy_regs(); \
	printf("%s():\n", __FUNCTION__); \
	printf("%-16s", "  *($rbp+8):"); \
	printf("%#018lx (return addr)\n", (unsigned long)ret_addr); \
	printf("%-16s", "  $rbp:"); \
	printf("%#018lx\n", (unsigned long)rbp); \
	printf("  ...\n"); \
	printf("%-16s", "  &" # _var_ ":"); \
	printf("%#018lx (variable on stack)\n", (unsigned long)&_var_); \
	printf("%-16s", "  $rsp:"); \
	printf("%#018lx\n", (unsigned long)rsp); \
} while (0)
#endif

