#include <8051.h>
#include "preemptive.h"


/*
 * saving the context of the current thread
 */
#define SAVESTATE \
        { __asm \
          push acc \
          push b \
          push dpl \
          push dph \
          push psw \
         __endasm; \
         savedSP[threadId] = SP; \
        }


/*
 * restoring the context of the current thread
 */
#define RESTORESTATE \
         { SP = savedSP[threadId]; \
           __asm \
           pop psw \
           pop dph \
           pop dpl \
           pop b \
           pop acc \
          __endasm; \
         }


extern void main(void);


/*
 * Bootstrap is jumped to by the startup code to make the thread for
 * main, and restore its context so the thread can run.
 */
void Bootstrap(void) {
      TMOD = 0;  // timer 0 mode 0
      IE = 0x82;  // enable timer 0 interrupt; keep consumer polling
                  // EA  -  ET2  ES  ET1  EX1  ET0  EX0
      TR0 = 1; // set bit TR0 to start running timer 0
      
      // Initialize timer
      timer = 0;
      timer_ = 0;

      threadBitmap = 0x00;
      threadCount = 0;

      // Create a thread for main
      threadId = ThreadCreate(main);

      RESTORESTATE;
}


/*
 * creates a thread data structure so it is ready to be restored (context switched in)
 */
ThreadID ThreadCreate(FunctionPtr fp) __critical {
      EA = 0;
      fp;
      // check to see we have not reached the max #threads
      if (threadCount >= MAXTHREADS) return -1;
      // otherwise, find a thread ID that is not in use,
      if      ((threadBitmap & 0x01) == 0x00) { threadId_new = 0; threadBitmap |= 1; }
      else if ((threadBitmap & 0x02) == 0x00) { threadId_new = 1; threadBitmap |= 2; }
      else if ((threadBitmap & 0x04) == 0x00) { threadId_new = 2; threadBitmap |= 4; }
      else if ((threadBitmap & 0x08) == 0x00) { threadId_new = 3; threadBitmap |= 8; }
      
      // increment #threads
      threadCount += 1;

      tempSP = SP;
      SP = 0x3F + 16 * threadId_new;

      __asm \
            PUSH DPL \
            PUSH DPH \
      __endasm;

      __asm \
            CLR A \
            PUSH ACC \
            PUSH ACC \
            PUSH ACC \
            PUSH ACC \
      __endasm;

      PSW = threadId_new << 3;
      __asm
            PUSH PSW \
      __endasm;

      savedSP[threadId_new] = SP;
      SP = tempSP;

      EA = 1;

      return threadId_new;
}


/*
 * called by a running thread to yield control to another thread
 * It saves the context of the current running thread,
 * picks another thread (and set the current thread ID to it),
 * if any, and then restores its state.
 */
void ThreadYield(void) __critical {
      SAVESTATE;

      // Update timer & delays
      timer_ += 1;
      if (timer_% time_unit == 0) timer += 1;
         
      do {
            if(threadId < 3) 
               threadId++;
            else 
               threadId = 0;
            if(threadBitmap & (1 << threadId)) break;
      } while (1);
      RESTORESTATE;
}


/*
 * called by the thread's own code to terminate itself.
 * It will never return; instead, it switches context to another thread.
 */
void ThreadExit(void) __critical {
      // TODO: clear the bit for the current thread from the bit mask
      if      (threadId == 0) { threadBitmap -= 1; }
      else if (threadId == 1) { threadBitmap -= 2; }
      else if (threadId == 2) { threadBitmap -= 4; }
      else if (threadId == 3) { threadBitmap -= 8; }

      if (threadBitmap & 1) { threadId = 0; }
      else if (threadBitmap & 2) { threadId = 1; }
      else if (threadBitmap & 4) { threadId = 2; }
      else if (threadBitmap & 8) { threadId = 3; }
      
      else { while(1); }

      RESTORESTATE;
}

void myTimer0Handler(void) {
   EA = 0;
   SAVESTATE;

   __asm
		MOV A, R0
		PUSH ACC
		MOV A, R1
		PUSH ACC
		MOV A, R2
		PUSH ACC
		MOV A, R3
		PUSH ACC
		MOV A, R4
		PUSH ACC
		MOV A, R5
		PUSH ACC
		MOV A, R6
		PUSH ACC
		MOV A, R7
		PUSH ACC
	__endasm;

   // Update timer & delays
   timer_ += 1;
   if(timer_ % time_unit == 0) {
         timer++;
         if(delays[0] > 0) delays[0]--;
         if(delays[1] > 0) delays[1]--;
         if(delays[2] > 0) delays[2]--;
         if(delays[3] > 0) delays[3]--;
   }

   do {
         // threadId = (threadId + 1) % threadCount;
         if(threadId < 3) 
            threadId++;
         else
            threadId = 0;

         if      (threadId == 0) { if (threadBitmap & 1) break; }
		   else if (threadId == 1) { if (threadBitmap & 2) break; }
		   else if (threadId == 2) { if (threadBitmap & 4) break; }
		   else if (threadId == 3) { if (threadBitmap & 8) break; }   

   } while (1);

   __asm
		POP ACC
		MOV R7, A
		POP ACC
		MOV R6, A
		POP ACC
		MOV R5, A
		POP ACC
		MOV R4, A
		POP ACC
		MOV R3, A
		POP ACC
		MOV R2, A
		POP ACC
		MOV R1, A
		POP ACC
		MOV R0, A
	__endasm; 

   RESTORESTATE;

   EA = 1;

   __asm
      reti
   __endasm;
}

/*
 * delays the thread by n time units
 */
// void delay(unsigned char n) {
//    delays[threadId] = n;
//    while(delays[threadId] > 0);
// }

/*
 * returns the current time
 * (i.e. number of time units that has elapsed since the system reboots)
 */
unsigned char now(void) {
   return timer;
}