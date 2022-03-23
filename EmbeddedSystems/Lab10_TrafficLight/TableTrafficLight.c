// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
/*
  PORT |BASE ADDRESS
  PORTA|0x40004000
  PORTB|0x40005000
	PORTC|0x40006000
  PORTD|0x40007000
	PORTE|0x40024000
  PORTF|0x40025000
	
	BIT|CONSTANT
	  7|0x0200 
	  6|0x0100 
		5|0x0080 
		4|0x0040 
		3|0x0020 
		2|0x0010 
		1|0x0008 
		0|0x0004 
*/
// To access bits 3 and 1, 0x0008 + 0x0020 = 0x0028
// To access bits 2-0, 0x0004 + 0x0008 + 0x0010 = 0x001C
#define WALK_LIGHT              (*((volatile unsigned long *)0x40025028)) // PF3,PF1
#define SENSORS                 (*((volatile unsigned long *)0x4002401C)) // PE2-PE0 
#define TRAFFIC_LIGHT           (*((volatile unsigned long *)0x400050FC)) // PB5-PB0
// Port B
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
// Port E
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
// Port F
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
// Initialize SysTick with busy wait running at bus clock.
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
// ***** 2. Global Declarations Section *****
// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Port_Init(void);
// SysTick implementation
void SysTick_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait10ms(unsigned long delay);
// Linked data structure
struct State {
  unsigned long Traffic;
	unsigned long Walk;
  unsigned long Time;  
  unsigned long Next[8];}; 
typedef const struct State STyp;
#define southGreen   0
#define southYellow  1
#define westGreen    2
#define westYellow   3
#define walk         4
#define doNotWalk1   5
#define lightsOff1   6
#define doNotWalk2   7
#define lightsOff2   8
STyp FSM[9]={
 {0x21,0x02,10,{southGreen,southYellow,southGreen,southYellow,southYellow,southYellow,southYellow,southYellow}},
 {0x22,0x02,10,{westGreen,westGreen,westGreen,westGreen,walk,westGreen,walk,westGreen}},
 {0x0C,0x02,10,{westGreen,westGreen,westYellow,westYellow,westYellow,westYellow,westYellow,westYellow}},
 {0x14,0x02,10,{walk,walk,southGreen,southGreen,walk,walk,walk,walk}},
 {0x24,0x08,10,{walk,doNotWalk1,doNotWalk1,doNotWalk1,walk,doNotWalk1,doNotWalk1,doNotWalk1}},
 {0x24,0x00,10,{lightsOff1,lightsOff1,lightsOff1,lightsOff1,lightsOff1,lightsOff1,lightsOff1,lightsOff1}},
 {0x24,0x02,10,{doNotWalk2,doNotWalk2,doNotWalk2,doNotWalk2,doNotWalk2,doNotWalk2,doNotWalk2,doNotWalk2}},
 {0x24,0x00,10,{lightsOff2,lightsOff2,lightsOff2,lightsOff2,lightsOff2,lightsOff2,lightsOff2,lightsOff2}},
 {0x24,0x00,10,{southGreen,westGreen,southGreen,southGreen,southGreen,westGreen,southGreen,southGreen}}};
unsigned long S;  // index to the current state 
unsigned long Input; 
// ***** 3. Subroutines Section *****
int main(void){
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	Port_Init();
	SysTick_Init();
  EnableInterrupts();
	S = westGreen; 
  while(1){
    TRAFFIC_LIGHT = FSM[S].Traffic;  // set car lights
		WALK_LIGHT = FSM[S].Walk;        // set walk lights
    SysTick_Wait10ms(FSM[S].Time);
    Input = SENSORS;     // read sensors
    S = FSM[S].Next[Input];  
  }
}
// Function implementations go at end
// Subroutine to initialize port B, E, F pins for input and output
// PF1 is output RED "don't walk" light and PF3 is output GREEN "walk" light
// PE2 is pedestrian detector
// PE1 is south car detector
// PE0 is west car detector
// PB5-3 are west output traffic lights RED, YELLOW, GREEN 
// PB2-0 are south output traffic lights RED, YELLOW, GREEN
// Inputs: None
// Outputs: None
// Notes: ...
void Port_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x32;           // 1) activate clock for Port B, Port E, Port F
  delay = SYSCTL_RCGC2_R;           // 2) only PC3-0, PD7, and PF0 on the TM4C need to be unlocked
	// PORT F
  GPIO_PORTF_AMSEL_R &= ~0x0A;      // 3) disable analog function on PF3, PF1
  GPIO_PORTF_PCTL_R &= 0x00000000;  // 4) enable regular GPIO
  GPIO_PORTF_DIR_R |= 0x0A;         // 5) outputs on PF3, PF1
  GPIO_PORTF_AFSEL_R &= ~0x0A;      // 6) regular function on PF3, PF1
  GPIO_PORTF_DEN_R |= 0x0A;         // 7) enable digital I/O on PF3, PF1
	// PORT E
  GPIO_PORTE_AMSEL_R &= ~0x07;      // 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= 0x00000000;  // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;        // 5) inputs on PE2-0         
  GPIO_PORTE_AFSEL_R &= ~0x07;      // 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;         // 7) enable digital on PE2-0
	// PORT B
  GPIO_PORTB_AMSEL_R &= ~0x3F;      // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= 0x00000000;  // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;         // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F;      // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;         // 7) enable digital on PB5-0
}
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}
// 10000us equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}