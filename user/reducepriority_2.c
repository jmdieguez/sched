#include <inc/lib.h>

volatile int counter;

void
umain(int argc, char **argv)
{
	envid_t envid = sys_getenvid();
    sys_decrement_priority(envid);
    sys_decrement_priority(envid);
    for (size_t i = 0; i < 1000; i++)
    {
        /* code */
    }
    //sys_yield();
    //sys_yield();
    //int priority = sys_get_priority(envid);
    //cprintf("SOY reducepriority_2 MI PID %d La PRIORIDAD es: %d\n",envid,priority);
}

