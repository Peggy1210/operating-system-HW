/* 
 * file: testcoop.c 
 */
#include <8051.h>
#include "preemptive.h"

#define TRUE 1
#define FALSE 0


/* 
 * @@@ [2pt] 
 * declare your global variables here, for the shared buffer 
 * between the producer and consumer.  
 * Hint: you may want to manually designate the location for the 
 * variable.  you can use
 *        __data __at (0x30) type var; 
 * to declare a variable var of the type
 */ 

__data __at (0x38) char buffer;
__data __at (0x39) char bufAvail;
__data __at (0x21) char alphabet;
__data __at (0x23) char mutex;
__data __at (0x24) char full;
__data __at (0x25) char empty;


/* [8 pts] for this function
 * the producer in this test program generates one characters at a
 * time from 'A' to 'Z' and starts from 'A' again. The shared buffer
 * must be empty in order for the Producer to write.
 */

void Producer(void) {
        /*
         * @@@ [2 pt]
         * initialize producer data structure, and then enter
         * an infinite loop (does not return)
         */

        alphabet = 'A';

        while (1) {
                SemaphoreWait(empty, COUNTER(__COUNTER__));
                SemaphoreWait(mutex, COUNTER(__COUNTER__));
                buffer = alphabet;
                bufAvail = 1;

                if(alphabet == 'Z') alphabet = 'A';
                else alphabet++;
                SemaphoreSignal(mutex);
                SemaphoreSignal(full);  
        }
        
}


/* [10 pts for this function]
 * the consumer in this test program gets the next item from
 * the queue and consume it and writes it to the serial port.
 * The Consumer also does not return.
 */

void Consumer(void) {
        /* @@@ [2 pt] initialize Tx for polling */
        TMOD |= 0x20;
        TH1 = -6;
        SCON = 0x50;
        TR1 = 1;

        while (1) {
                SemaphoreWait(full, COUNTER(__COUNTER__));
                SemaphoreWait(mutex, COUNTER(__COUNTER__));
                SBUF = buffer;
                bufAvail = 0;
                SemaphoreSignal(mutex);
                SemaphoreSignal(empty);

                while(!TI);
                TI = 0;
        }
        
}


/* [5 pts for this function]
 * main() is started by the thread bootstrapper as thread-0.
 * It can create more thread(s) as needed:
 * one thread can act as producer and another as consumer.
 */

void main(void) {
        /* 
         * @@@ [1 pt] initialize globals 
         * @@@ [4 pt] set up Producer and Consumer.
         * Because both are infinite loops, there is no loop
         * in this function and no return.
         */

        buffer = 0;
        bufAvail = 0;

        SemaphoreCreate(mutex, 1);
        SemaphoreCreate(full, 0);
        SemaphoreCreate(empty, 1);

        ThreadCreate(Producer);
        Consumer();
}


void _sdcc_gsinit_startup(void) {
        __asm
                ljmp  _Bootstrap
        __endasm;
}


void _mcs51_genRAMCLEAR(void) {}
void _mcs51_genXINIT(void) {}
void _mcs51_genXRAMCLEAR(void) {}

void timer0_ISR(void) __interrupt(1) {
        __asm
                ljmp _myTimer0Handler
        __endasm;
}
