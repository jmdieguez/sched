#include <inc/lib.h>

volatile int counter;

void
umain(int argc, char **argv)
{
	int i, j;
	int seen;
	envid_t parent = sys_getenvid();
	cprintf("La sys_getenvid \n");
    int priority = sys_get_priority(parent);
    cprintf("La PRIORIDAD es: %d\n",priority);
}

