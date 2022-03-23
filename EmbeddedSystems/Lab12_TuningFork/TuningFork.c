// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// 1. Pre-processor Directives Section
#include "TExaS.h"
#include "..//tm4c123gh6pm.h"
#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
#define GPIO_PORTA_DR8R_R       (*((volatile unsigned long *)0x40004508))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define NVIC_SYS_PRI3_R         (*((volatile unsigned long *)0xE000ED20))  // Sys. Handlers 12 to 15 Priority
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
// 2. Declarations Section
//   Global Variables
unsigned long SW1;
unsigned long toggle;
long prevSW1 = 0;        // previous value of SW1
//   Function Prototypes
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode
// 3. Subroutines Section
// input from PA3, output from PA2, SysTick interrupts
void Sound_Init(void){ 
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x00000001;     // 1) activate clock for Port A
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTA_AMSEL_R &= ~0x0C;      // 3) disable analog on PF
  GPIO_PORTA_PCTL_R &= ~0x00F00000; // 4) regular function
																		// 5) PA3 in, PA2 out 
	GPIO_PORTA_DIR_R &= ~0x08;        //   5.1) PA3 in
  GPIO_PORTA_DIR_R |= 0x04;         //   5.2) PA2 out 
	GPIO_PORTA_DR8R_R |= 0x04;        // can drive up to 8mA out
  GPIO_PORTA_AFSEL_R &= ~0x0C;      // 6) disable alt funct on PF7-0
  GPIO_PORTA_DEN_R |= 0x0C;         // 7) enable digital I/O on PF4-0
	// SysTick interrupt configuration
  NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
  NVIC_ST_RELOAD_R = 90908;     // reload value for f=880Hz (assuming 80MHz)
  NVIC_ST_CURRENT_R = 0;        // any write to current clears it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0               
  NVIC_ST_CTRL_R = 0x00000007;  // enable with core clock and interrupts
  EnableInterrupts();
}
// called at 880 Hz
void SysTick_Handler(void){
	if (toggle)
		GPIO_PORTA_DATA_R ^= 0x04; 
	else
		GPIO_PORTA_DATA_R &= ~0x04;
}
// MAIN: Mandatory for a C Program to be executable
int main(void){// activate grader and set system clock to 80 MHz
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
  Sound_Init();         
  EnableInterrupts();   // enable after all initialization are done
	GPIO_PORTA_DATA_R &= ~0x04;
	toggle = 0;
  while(1){
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
		SW1 = GPIO_PORTA_DATA_R&0x08;
		switch(SW1) {
			case(0x08) : 
				if (prevSW1!=SW1){
					toggle ^= 1;
					prevSW1 = SW1;
				}
				break;
			case (0x00) :
				if (prevSW1 != SW1) {
					prevSW1 = SW1;
				}
				break;
			default : 
				prevSW1 = 0;
				toggle = 0;
		}
  }
}
