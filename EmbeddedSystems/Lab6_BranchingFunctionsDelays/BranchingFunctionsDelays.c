// BranchingFunctionsDelays.c Lab 6
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to 
// toggle an LED while a button is pressed and 
// turn the LED on when the button is released.  
// This lab will use the hardware already built into the LaunchPad.
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// built-in connection: PF0 connected to negative logic momentary switch, SW2
// built-in connection: PF1 connected to red LED
// built-in connection: PF2 connected to blue LED
// built-in connection: PF3 connected to green LED
// built-in connection: PF4 connected to negative logic momentary switch, SW1

#include "TExaS.h"

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

#define PF4                     (*((volatile unsigned long *)0x40025040))
#define PF3                     (*((volatile unsigned long *)0x40025020))
#define PF2                     (*((volatile unsigned long *)0x40025010))
#define PF1                     (*((volatile unsigned long *)0x40025008))
#define PF0                     (*((volatile unsigned long *)0x40025004))

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(unsigned long time);

int main(void){ 
	unsigned long volatile delay;
  TExaS_Init(SW_PIN_PF4, LED_PIN_PF2);  // activate grader and set system clock to 80 MHz
  // initialization goes here
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // 1) Turn on the clock for Port F
	delay = SYSCTL_RCGC2_R;					      // delay to allow clock to stabilize 
  GPIO_PORTF_AMSEL_R &= 0x00;           // 2) Clear the PF4 and PF2 bits in Port F AMSEL to disable analog
  GPIO_PORTF_PCTL_R &= 0x00000000;      // 3) Clear the PF4 and PF2 bit fields in Port F PCTL to configure as GPIO
  GPIO_PORTF_DIR_R &= ~0x10;            // 4.1) PF4 input,
  GPIO_PORTF_DIR_R |= 0x0E;             // 4.2) PF3,2,1 output 
  GPIO_PORTF_AFSEL_R &= 0x00;           // 5) Clear the PF4 and PF2 bits in Port F AFSEL to disable alternate functions
  GPIO_PORTF_DEN_R |= 0x14;             // 6) Set the PF4 and PF2 bits in Port F DEN to enable digital
  GPIO_PORTF_PUR_R |= 0x10;             // 7) Set the PF4 bit in Port F PUR to activate an internal pullup resistor
  
	GPIO_PORTF_DATA_R |= 0x04;         // Set the PF2 bit in Port F DATA so the LED is initially ON
	
  EnableInterrupts();           // enable interrupts for the grader
	
  while(1){
    // body goes here		
		Delay100ms(1);							// Delay about 100 ms
		// Read the switch and test if the switch is pressed
		// In = GPIO_PORTF_DATA_R&0x10;   // read PF4 into Sw1	
		if(PF4 == 0x00){  					// If PF4=0 (the switch is pressed), also (GPIO_PORTF_DATA_R&0x10)
    GPIO_PORTF_DATA_R ^= 0x04;  // toggle PF2 (flip bit from 0 to 1, or from 1 to 0)		
		}
		else{ 											// If PF4=1 (the switch is not pressed),
		GPIO_PORTF_DATA_R |= 0x04; //  set PF2, so LED is ON
		}
  }
}


void Delay100ms(unsigned long time){
  unsigned long i;
  while(time > 0){
    i = 1333333;  // this number means 100ms
    while(i > 0){
      i = i - 1;
    }
    time = time - 1; // decrements every 100 ms
  }
}