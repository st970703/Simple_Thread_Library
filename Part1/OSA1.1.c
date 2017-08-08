/*
============================================================================
Name        : OSA1.c
Author      : Robert Sheehan
Version     : 1.0
Description : Single thread implementation.

Editor: En-Yu Mike Lee
UPI: elee353
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "littleThread.h"
#include "threads1.c" // rename this for different threads

Thread newThread; // the thread currently being set up
Thread mainThread; // the main thread
struct sigaction setUpAction;
const char* state_t[] = { "SETUP", "RUNNING", "READY", "FINISHED" };
const char* state_t_lower[] = {  "setup", "running", "ready", "finished" };
Thread threads[100];

//todo
/*
* Add a new function printThreadStates which prints out the thread ids
* and the state of each thread in the order they were created.
* The const NUMTHREADS is the number of threads created.
*/
void printThreadStates(Thread *threads, int length) {
	printf("\nThread States\n");
	printf("=============\n");

	for (int i = 0; i < length; i ++) {
		int state_no = threads[i]->state;
		printf("threadID: %i state:%s\n", threads[i]->tid, state_t_lower[state_no]);
	}
	printf("\n");
}

//todo
/*
* Add a scheduler function called scheduler to choose the next thread to run.
* You need to ensure that all thread states are correct.
* e.g. Threads which have completed have their state changed to FINISHED,
* threads which are waiting to run are READY and only the currently executing thread is RUNNING.
*/
Thread scheduler(Thread thread) {
	int lastNode = 0;
	if (thread->prev->tid == thread->tid) {
		if (thread->next->tid == thread->tid) {
			lastNode = 1;
		}
	}

	if (lastNode){
		return mainThread;
	} else {
		return thread->next;
	}
}

/*
* Switches execution from prevThread to nextThread.
*/
void switcher(Thread prevThread, Thread nextThread) {
	if (prevThread->state == FINISHED) { // it has finished
		//remove this finished thread by changing links
		prevThread->prev->next = prevThread->next;
		prevThread->next->prev = prevThread->prev;

		nextThread->state = RUNNING;
		printf("\ndisposing %d\n", prevThread->tid);
		free(prevThread->stackAddr); // Wow!
		longjmp(nextThread->environment, 1);
	} else if (setjmp(prevThread->environment) == 0) { // so we can come back here
		prevThread->state = READY;
		nextThread->state = RUNNING;

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
		printThreadStates(threads, NUMTHREADS);
		(localThread->start)();
		localThread->state = FINISHED;

		Thread nextThread = scheduler(localThread);
		switcher(localThread, nextThread); // at the moment back to the main thread
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

/*
* Sets up the doubly circular linked  list
*/
Thread setupLinkedList(Thread *threads, int length) {
	for (int i = 0; i < length; i++) {
		if (i == 0) {
			// first node
			threads[i]->prev = threads[length-1];
			threads[i]->next = threads[i+1];
		} else if (i == length-1) {
			// last node
			threads[i]->prev = threads[i-1];
			threads[i]->next = threads[0];
		} else {
			// middle nodes
			threads[i]->prev = threads[i-1];
			threads[i]->next = threads[i+1];
		}
	}
}

int main(void) {
	struct thread controller;
	mainThread = &controller;
	mainThread->state = RUNNING;
	setUpStackTransfer();
	// create the threads
	for (int t = 0; t < NUMTHREADS; t++) {
		threads[t] = createThread(threadFuncs[t]);
	}

	setupLinkedList(threads, NUMTHREADS);

	//test
	printThreadStates(threads, NUMTHREADS);
	//
	puts("switching to first thread");
	switcher(mainThread, threads[0]);
	puts("\nback to the main thread");

	//test
	printThreadStates(threads, NUMTHREADS);
	//
	return EXIT_SUCCESS;
}
