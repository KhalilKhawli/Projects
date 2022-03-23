// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "DAC.h"
// Piano key 0: C generates a sinusoidal DACOUT at 523.251 Hz
// Piano key 1: D generates a sinusoidal DACOUT at 587.330 Hz
// Piano key 2: E generates a sinusoidal DACOUT at 659.255 Hz
// Piano key 3: G generates a sinusoidal DACOUT at 783.991 Hz
// the sound frequency will be (interrupt frequency)/(size of the table)
#define C_Note 4778 	// period = 80MHz / (523.251 Hz * 32)
#define D_Note 4257 	// period = 80MHz / (587.330 Hz * 32)
#define E_Note 3792 	// period = 80MHz / (659.255 Hz * 32)
#define G_Note 3189 	// period = 80MHz / (783.991 Hz * 32)
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);
int main(void){ // Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
// PortE used for piano keys, PortB used for DAC  	
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
  EnableInterrupts();  // enable after all initialization are done
  while(1){
		unsigned long in = Piano_In(); 
		switch(in) {
			case(0x01) : 
				Sound_Tone(C_Note);
				break;
			case(0x02) : 
				Sound_Tone(D_Note);
				break;
			case(0x04) : 
				Sound_Tone(E_Note);
				break;
			case(0x08) : 
				Sound_Tone(G_Note);
				break;
		} 
		delay(10);
	}         
}
// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


