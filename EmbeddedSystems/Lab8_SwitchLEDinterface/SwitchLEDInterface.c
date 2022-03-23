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
#include "TExaS.h"
#include "tm4c123gh6pm.h"

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
#define SWITCH 0x01
#define LED    0x02


// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Switch_Init(void); 
unsigned long Switch_Input(void);
void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);
void delay(unsigned long msec);
// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz

	Switch_Init();
	LED_Init();
	LED_Off();
  EnableInterrupts();           // enable interrupts for the grader
	
  while(1){
	unsigned long switches = Switch_Input();
		// if (status == SWITCH){
		if (switches){		
		LED_Toggle();
			delay(100);
		}	else {
			LED_On();
		} 
  }
}

void Switch_Init(void){ unsigned long volatile delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;           		// allow time for clock to start
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTE_AMSEL_R &= ~SWITCH;        // 3) disable analog on PF
  GPIO_PORTE_PCTL_R &= ~0x0000000F; 		// 4) regular function 
	GPIO_PORTE_DIR_R &= ~SWITCH;          // 5) PE0 in
  GPIO_PORTE_AFSEL_R &= ~SWITCH;      	// 6) disable alt funct on PE0
  GPIO_PORTE_DEN_R |= SWITCH;         	// 7) enable digital I/O on PE0
}

//------------Switch_Input------------
// Read and return the status of GPIO Port A bit 5 
// Input: none
// Output: 0x01 if PE0 is high
//         0x00 if PE0 is low
unsigned long Switch_Input(void){
  return PE0; // return 0x01(pressed) or 0(not pressed)
}

void LED_Init(void){ unsigned long volatile delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;           		// allow time for clock to start
	// only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTE_AMSEL_R &= ~LED;       // 3) disable analog on PF
  GPIO_PORTE_PCTL_R &= ~0x000000F0; // 4) regular function 
	GPIO_PORTE_DIR_R |= LED;         	// 5) PE0 in
  GPIO_PORTE_AFSEL_R &= ~LED;       // 6) disable alt funct on PE0
  GPIO_PORTE_DEN_R |= LED;         	// 7) enable digital I/O on PE0
}

// Make PE1 high
void LED_On(void){
  GPIO_PORTE_DATA_R |= LED;
}

// Make PE1 low
void LED_Off(void){
  GPIO_PORTE_DATA_R &= ~LED;
}

// Toggle PE1
void LED_Toggle(void){
  GPIO_PORTE_DATA_R ^= LED;
}

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
