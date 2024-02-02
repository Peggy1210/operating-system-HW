/* 
 * file: testparking.c 
 */
#include <8051.h>
#include "preemptive.h"


void print_status(char time, char car, char io, char spot) {
	while(!TI);
        SBUF = time + 'a';
	TI = 0;

        while(!TI);
        SBUF = ':';
	TI = 0;
	
	while(!TI);
        SBUF = car;
	TI = 0;

        while(!TI);
        SBUF = io;
	TI = 0;

	while(!TI);
        SBUF = spot;
	TI = 0;

	while(!TI);
        SBUF = '\n';
	TI = 0;
}

void Parking1() {
        SemaphoreWait(spots_empty, COUNTER(__COUNTER__));
	
	EA = 0;
        // park in
	if (spots[0] == '_') { // spots 0 available
		SemaphoreWait(mutex, COUNTER(__COUNTER__));
		debug = car_name[threadId];
                spots[0] = debug;
		SemaphoreSignal(mutex);
		print_status(now(), car_name[threadId], 'i', '0');
	} else if (spots[1] == '_') { // spots 1 available
		SemaphoreWait(mutex, COUNTER(__COUNTER__));
		debug = car_name[threadId];
                spots[1] = debug;
		SemaphoreSignal(mutex);
		print_status(now(), car_name[threadId], 'i', '1');
	}
	EA = 1;

	delay(3);

	EA = 0;
        // park out
	if (spots[0] == car_name[threadId]) { // car at spot 0
		SemaphoreWait(mutex, COUNTER(__COUNTER__));
		spots[0] = '_';
		SemaphoreSignal(mutex);
		print_status(now(), car_name[threadId], 'o', '0');
	} else if (spots[1] == car_name[threadId]) { // car at spot 1
		SemaphoreWait(mutex, COUNTER(__COUNTER__));
		spots[1] = '_';
		SemaphoreSignal(mutex);
		print_status(now(), car_name[threadId], 'o', '1');
	}
	EA = 1;

	SemaphoreSignal(spots_empty);
        SemaphoreSignal(thread_empty);
	
	ThreadExit();
}


/* 
 * started by the thread bootstrapper as thread-0 and create 5 cars
 */
void main(void) {
        // turn on UART port
        TMOD |= 0x20;
        TH1 = (char) -6;
        SCON = 0x50;
        TR1 = 1;
        TI = 1;

        // Create semaphore
        SemaphoreCreate(mutex, 1);
        SemaphoreCreate(spots_empty, 2);
        SemaphoreCreate(thread_empty, 3);
        
        // initialize data structure
        spots[0] = '_';
        spots[1] = '_';

        // start parking
        // Make car 1
        SemaphoreWait(thread_empty, COUNTER(__COUNTER__));
        thread_created = ThreadCreate(Parking1);
        car_name[thread_created] = '1';

        // Make car 2
        SemaphoreWait(thread_empty, COUNTER(__COUNTER__));
        thread_created = ThreadCreate(Parking1);
        car_name[thread_created] = '2';

        // Make car 3
        SemaphoreWait(thread_empty, COUNTER(__COUNTER__));
        thread_created = ThreadCreate(Parking1);
        car_name[thread_created] = '3';

        // ThreadYield();

        // Make car 4
        SemaphoreWait(thread_empty, COUNTER(__COUNTER__));
        thread_created = ThreadCreate(Parking1);
        car_name[thread_created] = '4';

        // Make car 5
        SemaphoreWait(thread_empty, COUNTER(__COUNTER__));
        thread_created = ThreadCreate(Parking1);
        car_name[thread_created] = '5';

        ThreadExit();
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
