// Piano.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano
// December 29, 2014
// Port E bits 3-0 have 4 piano keys
#include "Piano.h"
#include "..//tm4c123gh6pm.h"
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOE      0x00000010
// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void){ 
  unsigned long volatile delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;           		// allow time for clock to start
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTE_AMSEL_R &= ~0x0F;      		// 3) disable analog on PF
  GPIO_PORTE_PCTL_R &= ~0x00000FFF; 		// 4) regular function 
	GPIO_PORTE_DIR_R &= ~0x0F;        		// 5) PE3-0 in
  GPIO_PORTE_AFSEL_R &= ~0x0F;      		// 6) disable alt funct on PE3-0
  GPIO_PORTE_DEN_R |= 0x0F;         		// 7) enable digital I/O on PE3-0
}
// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void){ 
  return (GPIO_PORTE_DATA_R&0x0F); // remove this, replace with input
}
