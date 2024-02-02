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

#define time_unit 8

/* 
 * global variables
 */ 
// Semaphores

__data __at (0x20) char car_name[5];
__data __at (0x25) char delays[4];
__data __at (0x2A) char thread_created;
__data __at (0x2B) char spots[2];
__data __at (0x2F) char debug;

// parameters for threads
__data __at (0x30) char savedSP[4];
__data __at (0x34) char threadBitmap;
__data __at (0x35) char threadId;
__data __at (0x36) char threadId_new;
__data __at (0x37) char threadCount;
__data __at (0x38) char tempSP;
__data __at (0x39) char timer;
__data __at (0x29) char timer_;
__data __at (0x3A) char spots_empty;
__data __at (0x3B) char mutex;
__data __at (0x3C) char thread_empty;


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

#define delay(n) \
         { \
            delays[threadId] = n; \
            while(delays[threadId] > 0); \
         }


         
typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);
void myTimer0Handler(void);

void print_char(char n);

// void delay(unsigned char n);
unsigned char now(void);

#endif // __PREEMPTIVE_H__
