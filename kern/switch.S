/*
 * Your code here for context_switch
 *
 * It should restores the register values in the Trapframe with the 'iret' instruction.
 * This exits the kernel and starts executing some environment's code.
 *
 * This function does not return.
 */

.globl context_switch;
context_switch:
	movl 	4(%esp),%esp
	popal
	popl 	%es
	popl 	%ds
	addl 	$0x8,%esp
	iret