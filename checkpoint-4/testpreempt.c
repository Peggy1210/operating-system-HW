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

__data __at (0x27) char buffer[3];
__data __at (0x38) char buffer_size;
__data __at (0x39) char rw_mutex;
__data __at (0x21) char alphabet;
__data __at (0x23) char mutex;
__data __at (0x24) char full;
__data __at (0x25) char empty;
__data __at (0x26) char number;

/* [8 pts] for this function
 * the producer in this test program generates one characters at a
 * time from 'A' to 'Z' and starts from 'A' again. The shared buffer
 * must be empty in order for the Producer to write.
 */

void Producer1(void) {
        /*
         * @@@ [2 pt]
         * initialize producer data structure, and then enter
         * an infinite loop (does not return)
         */

        alphabet = 'A';

        while (1) {
                SemaphoreWait(empty, COUNTER(__COUNTER__));
                SemaphoreWait(mutex, COUNTER(__COUNTER__));
                buffer_size++;
                buffer[buffer_size - 1] = alphabet;

                if(alphabet == 'Z') alphabet = 'A';
                else alphabet++;
                SemaphoreSignal(mutex);
                SemaphoreSignal(full);  
        }
        
}

void Producer2(void) {
        /*
         * @@@ [2 pt]
         * initialize producer data structure, and then enter
         * an infinite loop (does not return)
         */

        number = 48;

        while (1) {
                SemaphoreWait(empty, COUNTER(__COUNTER__));
                SemaphoreWait(mutex, COUNTER(__COUNTER__));
                buffer_size++;
                buffer[buffer_size - 1] = number;
                if(number == 57) number = 48;
                else number++;
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
                buffer_size--;

                buffer[0] = buffer[1];
                buffer[1] = buffer[2];
                buffer[2] = 0;
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

        for(int i=0; i<3; i++) buffer[i] = 0;
        buffer = 0;

        SemaphoreCreate(mutex, 1);
        SemaphoreCreate(full, 0);
        SemaphoreCreate(empty, 1);
        SemaphoreCreate(rw_mutex, 1)

        ThreadCreate(Producer1);
        ThreadCreate(Producer2);        
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
