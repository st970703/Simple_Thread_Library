/*
============================================================================
Name        : OSA1.1.c
Author      : Robert Sheehan
Version     : 1.0
Description : Single thread implementation.

Editor: En-Yu Mike Lee
UPI: elee353
============================================================================
You have to change the code
so that if multiple threads are created
they will be switched between as each thread finishes.
There is no pre-emption.
After all threads have finished, execution returns to the main function and
the process stops.
You can change the existing code in any way you like
as long as it still works in the same
way.
You will certainly have to change the switcher function.
You should keep all created threads in a circular linked list
(that is what the prev and next pointers are for in the thread struct).
You will need to keep track of the current thread
(the thread currently running), this will initially be the first thread created.
Always insert new threads at the end of the list.
Each thread must have a unique thread identifier.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "littleThread.h"
#include "threads0.c" // rename this for different threads

Thread newThread; // the thread currently being set up
Thread mainThread; // the main thread
struct sigaction setUpAction;

/*
* Switches execution from prevThread to nextThread.
*/
void switcher(Thread prevThread, Thread nextThread) {
	if (prevThread->state == FINISHED) { // it has finished
		printf("\ndisposing %d\n", prevThread->tid);
		free(prevThread->stackAddr); // Wow!
		longjmp(nextThread->environment, 1);
	} else if (setjmp(prevThread->environment) == 0) { // so we can come back here
		prevThread->state = READY;
		nextThread->state = RUNNING;
		printf("scheduling %d\n", nextThread->tid);
		longjmp(nextThread->environment, 1);
	}
}

/*
* Associates the signal stack with the newThread.
* Also sets up the newThread to start running after it is long jumped to.
* This is called when SIGUSR1 is received.
*/
void associateStack(int signum) {
	Thread localThread = newThread; // what if we don't use this local variable?
	localThread->state = READY; // now it has its stack
	if (setjmp(localThread->environment) != 0) { // will be zero if called directly
		(localThread->start)();
		localThread->state = FINISHED;
		switcher(localThread, mainThread); // at the moment back to the main thread
	}
}

/*
* Sets up the user signal handler so that when SIGUSR1 is received
* it will use a separate stack. This stack is then associated with
* the newThread when the signal handler associateStack is executed.
*/
void setUpStackTransfer() {
	setUpAction.sa_handler = (void *) associateStack;
	setUpAction.sa_flags = SA_ONSTACK;
	sigaction(SIGUSR1, &setUpAction, NULL);
}

/*
*  Sets up the new thread.
*  The startFunc is the function called when the thread starts running.
*  It also allocates space for the thread's stack.
*  This stack will be the stack used by the SIGUSR1 signal handler.
*/
Thread createThread(void (startFunc)()) {
	static int nextTID = 0;
	Thread thread;
	stack_t threadStack;

	if ((thread = malloc(sizeof(struct thread))) == NULL) {
		perror("allocating thread");
		exit(EXIT_FAILURE);
	}
	thread->tid = nextTID++;
	thread->state = SETUP;
	thread->start = startFunc;
	if ((threadStack.ss_sp = malloc(SIGSTKSZ)) == NULL) { // space for the stack
		perror("allocating stack");
		exit(EXIT_FAILURE);
	}
	thread->stackAddr = threadStack.ss_sp;
	threadStack.ss_size = SIGSTKSZ; // the size of the stack
	threadStack.ss_flags = 0;
	if (sigaltstack(&threadStack, NULL) < 0) { // signal handled on threadStack
		perror("sigaltstack");
		exit(EXIT_FAILURE);
	}
	newThread = thread; // So that the signal handler can find this thread
	kill(getpid(), SIGUSR1); // Send the signal. After this everything is set.
	return thread;
}

//todo
/*
* Add a scheduler function called scheduler to choose the next thread to run.
* You need to ensure that all thread states are correct.
* e.g. Threads which have completed have their state changed to FINISHED,
* threads which are waiting to run are READY and only the currently executing thread is RUNNING.
*/
scheduler() {}

//todo
/*
* Add a new function printThreadStates which prints out the thread ids
* and the state of each thread in the order they were created.
* The const NUMTHREADS is the number of threads created.
*/
printThreadStates(Thread threads[]) {
	for (int i = 0; i < sizeof(threads); i ++) {
		printf("%i\n", threads[i].tid);
		printf("%s\n", threads[i].state);
	}
}

int main(void) {
	struct thread controller;
	Thread threads[NUMTHREADS];
	mainThread = &controller;
	mainThread->state = RUNNING;
	setUpStackTransfer();
	// create the threads
	for (int t = 0; t < NUMTHREADS; t++) {
		threads[t] = createThread(threadFuncs[t]);
	}
	puts("switching to first thread");
	switcher(mainThread, threads[0]);
	puts("back to the main thread");
	return EXIT_SUCCESS;
}
