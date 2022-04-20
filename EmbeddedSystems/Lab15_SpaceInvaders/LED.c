// ******* Required Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5


// ***** 1. Pre-processor Directives Section *****
#include "..//tm4c123gh6pm.h"
#include "LED.h"

// ***** 2. Global Declarations Section *****
#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_PUR_R        (*((volatile unsigned long *)0x40005510))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_LOCK_R       (*((volatile unsigned long *)0x40005520))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define SYSCTL_RCGC2_GPIOB      0x00000002 
#define LED1    0x10 // PB4 0b0001 0000
#define LED2    0x20 // PB5 0b0010 0000
#define LEDS    0x30 //     0b0011 0000
void LED_Init(void){ unsigned long volatile delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // 1) activate clock for Port B
  delay = SYSCTL_RCGC2_R;           		// allow time for clock to start
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTB_AMSEL_R &= ~(LEDS);        // 3) disable analog on PB
  GPIO_PORTB_PCTL_R &= ~0x00000FFF; 		// 4) regular function 
	GPIO_PORTB_DIR_R |= (LEDS);           // 5) PB4, PB5 out
  GPIO_PORTB_AFSEL_R &= ~(LEDS);  		  // 6) disable alt funct on PB4, PB5
  GPIO_PORTB_DEN_R |= (LEDS);         	// 7) enable digital I/O on PB4, PB5
}

void LED_Fire(void){
	GPIO_PORTB_DATA_R |= LED1;
}

void LED_Fire_Off(void){
	GPIO_PORTB_DATA_R &= ~LED1;
}
void LED_CooldownReady(void){
	GPIO_PORTB_DATA_R |= LED2;
}
void LED_CooldownReady_Off(void){
	GPIO_PORTB_DATA_R &= ~LED2;
}
