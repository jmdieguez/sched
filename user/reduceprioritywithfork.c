// Concurrent version of prime sieve of Eratosthenes.
// Invented by Doug McIlroy, inventor of Unix pipes.
// See http://swtch.com/~rsc/thread/.
// The picture halfway down the page and the text surrounding it
// explain what's going on here.
//
// Since NENVS is 1024, we can print 1022 primes before running out.
// The remaining two environments are the integer generator at the bottom
// of main and user/idle.

#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	int i, id;

	// fork the first prime process in the chain
    envid_t parent = sys_getenvid();
    sys_decrement_priority(parent);
	if ((id = fork()) < 0)
		panic("fork: %e", id);
	if (id == 0){
        envid_t hijo = sys_getenvid();
        int priorityHijo = sys_get_priority(hijo);
        cprintf("La PRIORIDAD PRIMER HIJO es: %d su PID %d \n",priorityHijo,sys_getenvid());
        int id2;
        if ((id2 = fork()) < 0)
		    panic("fork: %e", id2);
        if (id2 == 0){
            envid_t nieto = sys_getenvid();
            int prioritynieto = sys_get_priority(nieto);
            cprintf("La PRIORIDAD NIETO: %d su PID %d \n",prioritynieto,sys_getenvid());
        }
    }
    else{
        int priority = sys_get_priority(parent);
        cprintf("La PRIORIDAD de padre es: %d su PID %d \n",priority,sys_getenvid());
    }
	
}

