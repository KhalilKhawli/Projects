// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the edX Lab 15
// In order for other students to play your game
// 1) You must leave the hardware configuration as defined
// 2) You must not add/remove any files from the project
// 3) You must add your code only this this C file
// I.e., if you wish to use code from sprite.c or sound.c, move that code in this file
// 4) It must compile with the 32k limit of the free Keil

// April 10, 2014
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include <stdint.h>
#include "Images.h"
#include "Switch.h"

//unsigned short enemyAtEdge(void);
void Create_playerBullet(unsigned long switches);

// *************************** Capture image dimensions out of BMP**********
#define BUNKERW     ((unsigned char)Bunker0[18])
#define BUNKERH     ((unsigned char)Bunker0[22])
#define ENEMY30W    ((unsigned char)SmallEnemy30PointA[18])
#define ENEMY30H    ((unsigned char)SmallEnemy30PointA[22])
#define ENEMY20W    ((unsigned char)SmallEnemy20PointA[18])
#define ENEMY20H    ((unsigned char)SmallEnemy20PointA[22])
#define ENEMY10W    ((unsigned char)SmallEnemy10PointA[18])
#define ENEMY10H    ((unsigned char)SmallEnemy10PointA[22])
#define ENEMYBONUSW ((unsigned char)SmallEnemyBonus0[18])
#define ENEMYBONUSH ((unsigned char)SmallEnemyBonus0[22])
#define LASERW      ((unsigned char)Laser0[18])
#define LASERH      ((unsigned char)Laser0[22])
#define MISSILEW    ((unsigned char)Missile0[18])
#define MISSILEH    ((unsigned char)Missile0[22])
#define PLAYERW     ((unsigned char)PlayerShip0[18])
#define PLAYERH     ((unsigned char)PlayerShip0[22])

#define MIN_X 2

signed long xDirection;
signed long yDirection;
// unsigned long enemySpeed;

// Modules:
//  Game Engine:
//		Nokia5110
//    	Images
// 			Sprites
//    ADC sampled at 30 Hz
//    Sound
//    Switch
//    LEDs
typedef enum {dead, alive} status_t;
struct sprite {
	int32_t x; // x coordinate
	int32_t y; // y coordinate
	int32_t vx, vy; // velocity 
	const unsigned char *image[2]; // pointers to images
	status_t life; // dead/alive
	int32_t w; // width
	int32_t h; // height
	uint32_t needDraw; // true if need to draw 
};
typedef struct sprite sprite_t;

struct State {
  unsigned long x;      // x coordinate
  unsigned long y;      // y coordinate
  const unsigned char *image[2]; // two pointers to images
  long life;            // 0=dead, 1=alive
};         
typedef struct State STyp;
STyp Enemy[5];
STyp Player;

//sprite_t playerBullet;

/*

// check if the fire button is pressed 
void Create_playerBullet(unsigned long switches){
	// Missile Init
	playerBullet.x = 0;
  playerBullet.y = 0;
  playerBullet.image[0] = Laser0;
  playerBullet.image[1] = Laser1;
	playerBullet.vx = 0;
	playerBullet.vy = 2;
	// if pressed make a bullet appear
	// at the player ship
	if (switches){
		playerBullet.x = Player.x;
		playerBullet.y = Player.y;
	}
	// move the bullet up the screen
	playerBullet.y = playerBullet.y - playerBullet.vy;
	
	// Draw playerBullet
	Nokia5110_ClearBuffer();
	Nokia5110_PrintBMP(playerBullet.x, playerBullet.y, playerBullet.image[0], 0);
	Nokia5110_DisplayBuffer();
}


*/


void Sprites_Enemy_Init(void){ int i;
	xDirection = 1; 
	yDirection = 1; 
	// enemySpeed = 1;
  for(i=0;i<5;i++){
    Enemy[i].x = 12*i;
    Enemy[i].y = 10;
    Enemy[i].image[0] = SmallEnemy30PointA;
    Enemy[i].image[1] = SmallEnemy30PointB;
    Enemy[i].life = 1;
   }
}
/*
void Sprites_Enemy_Move(void){ int i;
	if (enemyAtEdge() == 1){
		for(i=0;i<5;i++){
			if(Enemy[i].life > 0){
				Enemy[i].y += yDirection; // move to down
			}
		}
		xDirection = -xDirection;	
	}else{ 
		for(i=0;i<5;i++){
			if(Enemy[i].life > 0){
				Enemy[i].x += xDirection; // move to right
			}
		}
	}
}
unsigned long FrameCount=0;
void Sprites_Enemy_Draw(void){ int i;
  Nokia5110_ClearBuffer();
  for(i=0;i<5;i++){
    if(Enemy[i].life > 0){
     Nokia5110_PrintBMP(Enemy[i].x, Enemy[i].y, Enemy[i].image[FrameCount], 0);
    }
  }
  Nokia5110_DisplayBuffer();      // draw buffer
  FrameCount = (FrameCount+1)&0x01; // 0,1,0,1,...
}
unsigned short enemyAtEdge(void){ int i;
  for(i=0;i<5;i++){	// for each enemy
		if(Enemy[i].life > 0){  // if Enemy is alive then
			if(xDirection == 1){ // if Enemy is going right +1
				if(Enemy[i].x + enemySpeed > MAX_X){ // if Enemy x + step > MaxX then
					return 1;   
				} 
			}else{ 
				if(Enemy[i].x - enemySpeed < MIN_X){ // if enemy x - step < minx then
					return 1; // return true 
				} 
			} 
		} 
 } 
	return 0;
}
*/
void Sprites_PlayerShip_Init(void){
	Player.x = 32;
  Player.y = 47;
  Player.image[0] = PlayerShip0;
  Player.image[1] = PlayerShip0;
  Player.life = 1;
}
void Sprites_PlayerShip_Move(unsigned long ADC){
	Player.x = (int)(MAX_X-PLAYERW)*(float)ADC/4095; 
}
void Sprites_PlayerShip_Draw(void){
  Nokia5110_ClearBuffer();
	if(Player.life>0){ 
		Nokia5110_PrintBMP(Player.x, Player.y, Player.image[0], 0);
	}
  Nokia5110_DisplayBuffer();      // draw buffer
}
