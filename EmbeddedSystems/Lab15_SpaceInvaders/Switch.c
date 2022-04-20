// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****

#include "..//tm4c123gh6pm.h"
#include "Switch.h"

// ***** 2. Global Declarations Section *****
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define SYSCTL_RCGC2_GPIOE      0x00000010
#define PE0                     (*((volatile unsigned long *)0x40024004))
#define PE1                     (*((volatile unsigned long *)0x40024008))
#define SWITCH1  0x01
#define SWITCH2  0x02
#define SWITCHES 0x03
// ***** 3. Subroutines Section *****

// PE0 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1 connected to positive logic momentary switch using 10k ohm pull down resistor

void Switch_Init(void){ unsigned long volatile delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;           		// allow time for clock to start
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTE_AMSEL_R &= ~SWITCHES;      // 3) disable analog on PE
  GPIO_PORTE_PCTL_R &= ~0x0000000F; 		// 4) regular function 
	GPIO_PORTE_DIR_R &= ~SWITCHES;        // 5) PE0,PE1 in
  GPIO_PORTE_AFSEL_R &= ~SWITCHES;      // 6) disable alt funct on PE0,PE1
  GPIO_PORTE_DEN_R |= SWITCHES;         // 7) enable digital I/O on PE0,PE1
}
//------------Switch_Input1------------
// Read and return the status of GPIO Port E bit 0 
// Input: none
// Output: 0x01 if PE0 is high
//         0x00 if PE0 is low
unsigned long Switch_Input1(void){
  return PE0; // return 0x01(pressed) or 0(not pressed)
}
//------------Switch_Input2------------
// Read and return the status of GPIO Port E bit 1 0b0010 
// Input: none
// Output: 0x02 if PE1 is high
//         0x00 if PE1 is low
unsigned long Switch_Input2(void){
  return PE1; // return 0x01(pressed) or 0(not pressed)
}
