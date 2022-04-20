// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"
// Port B
#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC)) 
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define GPIO_PORTB_DR8R_R       (*((volatile unsigned long *)0x40005508))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOB      0x00000002
#define DACPINS 0x0F
// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){
	unsigned long volatile delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // activate port B
  delay = SYSCTL_RCGC2_R;    // allow time to finish activating
  GPIO_PORTB_AMSEL_R &= ~DACPINS;      // no analog
  GPIO_PORTB_PCTL_R &= ~0x00000FFF; // regular GPIO function
  GPIO_PORTB_DIR_R |= DACPINS;      // make PB3-0 out
	GPIO_PORTB_DR8R_R |= DACPINS;        // can drive up to 8mA out
  GPIO_PORTB_AFSEL_R &= ~DACPINS;   // disable alt funct on PB3-0
  GPIO_PORTB_DEN_R |= DACPINS;      // enable digital I/O on PB3-0
}
// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
		data = (GPIO_PORTB_DATA_R & ~DACPINS) | data;   
    GPIO_PORTB_DATA_R = data;
}
