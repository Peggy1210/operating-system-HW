/*
 * file: preemptive.h
 *
 * this is the include file for the preemptive multithreading
 * package.  It is to be compiled by SDCC and targets the EdSim51 as
 * the target architecture.
 *
 * CS 3423 Fall 2018
 */

#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 4  /* not including the scheduler */
/* the scheduler does not take up a thread of its own */

// converting a C name into an assembly name
#define CNAME(s) _ ## s

// label
#define LABEL(c) c ## $

// counter
#define COUNTER(c) LABEL(c)

// create a counting semaphore s that is initialized to n
#define SemaphoreCreate(s, n) \
         {  \
            s = n; \
         }

// do (busy-)wait() on semaphore s
#define SemaphoreWait(s, label) \
         {  __asm \
            push acc \
   label:   mov acc, CNAME(s) \
            JZ label \
            JB ACC.7, label \
            dec CNAME(s) \
            pop acc \
            __endasm; \
         }

// signal() semaphore s
#define SemaphoreSignal(s) \
         {  __asm \
            inc CNAME(s) \
            __endasm; \
         }
typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);
void myTimer0Handler(void);

#endif // __PREEMPTIVE_H__
