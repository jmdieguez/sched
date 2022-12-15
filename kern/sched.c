#include <inc/assert.h>
#include <inc/x86.h>
#include <kern/spinlock.h>
#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/monitor.h>

void sched_halt(void);
int prioridad = 3;
// Choose a user environment to run and run it.

int sched_calls = 0;

int ids[1000];
int ids_index = 0;

void
sched_yield(void)
{
	sched_calls++;

	size_t env_id = 0;

	if (curenv)  // Si no hay curenv, env_id = 0
		env_id = ENVX(
		        curenv->env_id);  // Calculo el index al env_id actual con ENVX(), y tomo el siguiente

// Search through 'envs' for an ENV_RUNNABLE environment in
// circular fashion starting just after the env this CPU was
// last running.  Switch to the first such environment found.
#ifdef C_P
	// for (int j = 3; j > 0; j--){
	int j = 0;
	for (int i = 0; i < NENV; i++) {
		struct TrapFrame *highP;
		const size_t array_index = (env_id + i) % NENV;
		if (&envs[array_index] != NULL &&
		    envs[array_index].env_status == ENV_RUNNABLE &&
		    envs[array_index].env_prioritry > j)
			j = envs[array_index].env_prioritry;

		if (&envs[array_index] != NULL &&
		    envs[array_index].env_status == ENV_RUNNABLE &&
		    envs[array_index].env_prioritry == prioridad) {
			// cprintf("ENV PRIORIDAD: %d ENV_ID:
			// %d\n",envs[array_index].env_prioritry,envs[array_index].env_id);
			// cprintf("LA PRIORIDAD EVALUADA ES: %d\n",prioridad);
			envs_executed[cant_context_switch] = envs[array_index];
			cant_context_switch++;
			envs[array_index].env_count_of_executions =
			        envs[array_index].env_count_of_executions + 1;
			env_run(&envs[array_index]);
		}
	}
	prioridad = j;
	//}
#endif
#ifdef R_R
	for (int i = 0; i < NENV; i++) {
		const size_t array_index =
		        (env_id + i) %
		        NENV;  // Calculo el index del proximo proceso en la iteración.
		               // % NENV para volver al inicio de los procesos
		if (&envs[array_index] != NULL &&
		    envs[array_index].env_status ==
		            ENV_RUNNABLE)  // cuando array_index = NENV.
			env_run(&envs[array_index]);
	}
#endif

	// If no envs are runnable, but the environment previously
	// running on this CPU is still ENV_RUNNING, it's okay to
	// choose that environment.
	if (curenv && curenv->env_status == ENV_RUNNING &&
	    (cpunum() == curenv->env_cpunum))
		env_run(curenv);

	// Never choose an environment that's currently running on
	// another CPU (env_status == ENV_RUNNING). If there are
	// no runnable environments, simply drop through to the code
	// below to halt the cpu.
	sched_halt();
}

// Halt this CPU when there is nothing to do. Wait until the
// timer interrupt wakes it up. This function never returns.
//

void
print_statistics(void)
{
	if (ids_index > 0) {
		cprintf("\nReporting scheduler's statistics: \n\n");
		cprintf("Calls to scheduler: %d \n", sched_calls);
		cprintf("Environment's id scheduling history: \n");
		for (int f = 0; f < ids_index; f++) {
			if (f % 4 == 0)
				cprintf("\n");
			cprintf("[ yield = %3d ] : %5d   ", f, ids[f]);
		}
		cprintf("\n\n");
	}
}
void
sched_halt(void)
{
	int i;

	// For debugging and testing purposes, if there are no runnable
	// environments in the system, then drop into the kernel monitor.
	for (i = 0; i < NENV; i++) {
		if ((envs[i].env_status == ENV_RUNNABLE ||
		     envs[i].env_status == ENV_RUNNING ||
		     envs[i].env_status == ENV_DYING))
			break;
	}

	if (i == NENV) {
		cprintf("CANTIDAD DE CONTEXT SWITCH: %d \n", cant_context_switch);
		for (int j = 0; j < cant_context_switch; j++) {
			cprintf("EL PROCESO FUE EJECUTADO: %d SU PRIORIDAD ES: "
			        "%d - LA CANTIDAD DE CONTEXT_SWITCH DEL "
			        "PROCESO ES: %d LA CANTIDAD DE INTERRUPCIONES "
			        "DEL PROCESO ES: %d \n",
			        envs_executed[j].env_id,
			        envs_executed[j].env_prioritry,
			        envs_executed[j].env_count_of_executions,
			        envs_executed[j].env_count_interruptions);
		}
		// cprintf("No runnable environments in the system!\n");

		while (1)
			monitor(NULL);
	}

	// Mark that no environment is running on this CPU
	curenv = NULL;
	lcr3(PADDR(kern_pgdir));

	// Mark that this CPU is in the HALT state, so that when
	// timer interupts come in, we know we should re-acquire the
	// big kernel lock
	xchg(&thiscpu->cpu_status, CPU_HALTED);

	// Release the big kernel lock as if we were "leaving" the kernel
	unlock_kernel();

	// Once the scheduler has finishied it's work, print statistics on
	// performance. Your code here
	print_statistics();

	// Reset stack pointer, enable interrupts and then halt.
	asm volatile("movl $0, %%ebp\n"
	             "movl %0, %%esp\n"
	             "pushl $0\n"
	             "pushl $0\n"
	             "sti\n"
	             "1:\n"
	             "hlt\n"
	             "jmp 1b\n"
	             :
	             : "a"(thiscpu->cpu_ts.ts_esp0));
}
