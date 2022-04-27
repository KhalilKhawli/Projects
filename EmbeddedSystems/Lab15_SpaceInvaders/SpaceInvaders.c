// 0. Documentation Section

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

#include <stdint.h> 
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "Images.h"
#include "ADC.h"
#include "Sound.h"
#include "Switch.h"
#include "LED.h"

// 1. Pre-processor Directives Section
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

#define MAXPLAYERMISSILES 3
#define MAXROW 3
#define MAXCOLUMN 5
#define MINX 2
#define MAXENEMYLASORS 5

// 2. Declarations Section
//****************Globals************************
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
sprite_t playerMissiles[MAXPLAYERMISSILES]; // collection of a missile
sprite_t playerShip;
sprite_t enemy[MAXROW][MAXCOLUMN];
sprite_t enemyLasers[MAXENEMYLASORS];

const unsigned char *enemyRowType[6] = { 
	SmallEnemy30PointA,
	SmallEnemy30PointB,
	SmallEnemy20PointA,
	SmallEnemy20PointB,
	SmallEnemy10PointA,	
	SmallEnemy10PointB};

unsigned short PlayerScore; // unsigned short = 2 bytes 0 - 
unsigned short PlayerLives;
unsigned long Position;    // 12-bit 0 to 4095 sample
unsigned long Semaphore;
unsigned long SW1=0; 			// current value of switch
long prevSW1 = 0;         // previous value of SW1
	
// Function Prototypes
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
void Delay1ms(unsigned long count); // time delay in 0.001 seconds

void SysTick_Init(unsigned long period);	
void GameInit(void);
void Board_Init(void);
	
// Draw Function Prototypes
void GameDraw(void);
void drawPlayerMissile(void);
void drawEnemy(void);
void drawEnemyLasers(void);
void drawPlayerShip(void);
void drawPlayerScore(unsigned short n);	

// Move Function Prototypes
void moveEnemy(void);
void moveEnemyLasers(void);
void movePlayerShip(void);
void movePlayerMissile(void);

// Collison Detection Function Prototypes
unsigned short enemyAtEdge(void);
void checkPlayerFire(void);
void createEnemyLaser(void);
void checkLaserCollisions(void);
void checkMissileCollisions(void);
unsigned short checkCollision(sprite_t sprite1, sprite_t sprite2);

// Game State Function Prototypes
void StartGame(void);
void NewLife(void);
void GameOver(void);
void AllEnemyDead(void);

// Finite State Machine
unsigned long GameState; 
#define stateStartGame      0
#define statePlayGame       1
#define stateNewLife        2
#define stateGameOver       3
#define stateAllEnemyDead   4
// 3. Subroutines Section
// main is mandatory for a C Program to be executable
int main(void){ 
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz

	Random_Init(1); // linear congruential multiplier
  Nokia5110_Init();	
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();      // draw buffer	
	Board_Init();
		
	GameState = stateStartGame;
	StartGame();
	GameInit();

	Semaphore = 0;

	// Game loop
	while(PlayerLives){ 
    while(Semaphore == 0){};
		GameDraw();
		switch(GameState) {	
			case (stateNewLife) :
				playerShip.image[0] = PlayerShip0;
				GameState = statePlayGame;
				break;
			case (stateGameOver) :
				PlayerLives = 0;
				break;
			case (stateAllEnemyDead) :
				AllEnemyDead();
				break;
			default :
				GameState = statePlayGame;
				break; 
		}
    Semaphore = 0; // runs at 30 Hz	
	}	
	GameOver();
	while(1){}
}		
// Initialize SysTick interrupts to trigger at 30 Hz, 33.3 ms
void SysTick_Init(unsigned long period){ // priority 2
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;          
  NVIC_ST_CTRL_R = 0x07; // enable SysTick with core clock and interrupts
}
// executes every 33.3 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 
	movePlayerShip(); // ADC input
	checkPlayerFire(); // plays shoot sound
	movePlayerMissile();
	moveEnemy();
	createEnemyLaser(); // plays shoot sound
	moveEnemyLasers();
	checkMissileCollisions();
	checkLaserCollisions(); 
	
	if (GameState == stateNewLife){
		NewLife();
	} 
	Semaphore = 1;
}
void Board_Init(void){
	Switch_Init();
	LED_Init();
	ADC0_Init();
}
void GameInit(void){
	int missile;
	int laser;
	int row, column;
	
	// Init Scores
	PlayerScore = 0;
	PlayerLives = 3;
	// Init Sound
	EnableInterrupts();
	Sound_Init();
	// Init Speed
	SysTick_Init(80000000/30);
	
	Random_Init(NVIC_ST_CURRENT_R); // linear congruential multiplier
	// Init enemy laser
	for(laser = 0; laser < MAXENEMYLASORS; laser++){
		enemyLasers[laser].x = 0;
		enemyLasers[laser].y = 0;
		enemyLasers[laser].vx = 0;
		enemyLasers[laser].vy = 1;
		enemyLasers[laser].image[0] = Laser0;
		enemyLasers[laser].image[1] = Laser1;
		enemyLasers[laser].life = dead; // inactive
		enemyLasers[laser].w = MISSILEW; // width
		enemyLasers[laser].h = MISSILEH; // height
		enemyLasers[laser].needDraw = 0; // true if need to draw 
	}
	// Init player bullet
	for (missile = 0; missile<MAXPLAYERMISSILES; missile++){
		playerMissiles[missile].x = 0;
		playerMissiles[missile].y = 0;
		playerMissiles[missile].vx = 0;
		playerMissiles[missile].vy = 2;
		playerMissiles[missile].image[0] = Missile0;
		playerMissiles[missile].image[1] = Missile1;
		playerMissiles[missile].life = dead; // inactive
		playerMissiles[missile].w = MISSILEW; // width
		playerMissiles[missile].h = MISSILEH; // height
		playerMissiles[missile].needDraw = 0; // true if need to draw 
	}
	// Init playerShip
	playerShip.x = 32; // x coordinate
	playerShip.y = 47; // y coordinate
	playerShip.vx = 0; // speed/velocity in x direction
	playerShip.vy = 0; // speed/velocity in y direction
	playerShip.image[0] = PlayerShip0; // pointers to images
	playerShip.image[1] = PlayerShip0;
	playerShip.life = alive; // dead/alive
	playerShip.w = PLAYERW; // width
	playerShip.h = PLAYERH; // height
	playerShip.needDraw = 1; // true if need to draw 
	// Init for Aliens
/*
 x[3][4]
       column 1 | column 2 | column 3 | column 4
row 1 | x[0][0] | x[0][1]  | x[0][2]  | x[0][3] |
row 2 | x[1][0] | x[1][1]  | x[1][2]  | x[1][3] |
row	3 | x[2][0] | x[2][1]  | x[2][2]  | x[2][3] |
*/
	for (row=0; row < MAXROW; row++){
		for (column=0; column < MAXCOLUMN; column++){
			enemy[row][column].x = column*(ENEMY10W-2); // x coordinate
	    enemy[row][column].y = row*ENEMY10H+(ENEMY10H-1); // y coordinate
	    enemy[row][column].vx = 1; // speed/velocity in x direction
	    enemy[row][column].vy = 1; // speed/velocity in y direction
	    enemy[row][column].image[0] = enemyRowType[2*row]; //SmallEnemy10PointA; // pointers to images
	    enemy[row][column].image[1] = enemyRowType[2*row+1]; //SmallEnemy10PointB;
			enemy[row][column].life = alive; // dead/alive
			enemy[row][column].w = ENEMY10W; // width
			enemy[row][column].h = ENEMY10H; // height
			enemy[row][column].needDraw = 1; // true if need to draw 
		}
	}
}
void GameDraw(void){
		Nokia5110_ClearBuffer();
		drawPlayerShip();
		drawPlayerMissile();
		drawEnemy();
		drawEnemyLasers();
		Nokia5110_DisplayBuffer();
		drawPlayerScore(PlayerScore);
}
void movePlayerShip(void){
	Position = ADC0_In(); // read data;
	playerShip.x = (int)(MAX_X-PLAYERW)*(float)Position/4095; 
}
void drawPlayerShip(void){
	Nokia5110_PrintBMP(playerShip.x, playerShip.y, playerShip.image[0], 0);
}
void checkPlayerFire(void){int missile = 0;
	// check if fire button is pressed
	int missileFired = 0;
	SW1 = Switch_Input1(); // read in switch value
	if (SW1&&(!prevSW1)){     // detect rising edge 
		// find a bullet thats ready to fire
		while ((missile < MAXPLAYERMISSILES) && (!missileFired)) {
			if (!playerMissiles[missile].needDraw){
				playerMissiles[missile].x = playerShip.x + (PLAYERW/2-1); //offset
				playerMissiles[missile].y = playerShip.y - (PLAYERH/2-1);
				playerMissiles[missile].needDraw = 1;
				playerMissiles[missile].life = alive;				
				missileFired = 1;
				Sound_Shoot();
			}
			missile++;
		}
	}
	prevSW1 = SW1;
}
void moveEnemy(void){	int row, column;
	int enemyAliveCounter = 0;
	// For Music:
	// play this note sequence every step of the enemy;
	// G-2 - F-2 - E-2 - D-2
	if(enemyAtEdge() == 1){
		for (row=0; row < MAXROW; row++){
			for (column=0; column < MAXCOLUMN; column++){
				if (enemy[row][column].needDraw == 1){
					if (enemy[row][column].life == alive){
						enemy[row][column].y += enemy[row][column].vy; // enemy.y = emeny.y + (enemy.speed*enemy.Direction)
						
						enemyAliveCounter++;
						
						if (enemy[row][column].y > MAX_Y-PLAYERH){
							GameState = stateGameOver;
						}
					}		
				}
				enemy[row][column].vx = -enemy[row][column].vx;
			} 
		}
	}else{
		for (row=0; row < MAXROW; row++){
			for (column=0; column < MAXCOLUMN; column++){
				if (enemy[row][column].needDraw == 1){
					if (enemy[row][column].life == alive){
						enemy[row][column].x += enemy[row][column].vx;//enemy[row][column].vx; // enemy.x = emeny.x + (enemy.speed*enemy.Direction)
						enemyAliveCounter++;
					}
				}
			}
		}
	}
	if (enemyAliveCounter == 0){
			GameState = stateAllEnemyDead;
	}
}
unsigned short enemyAtEdge(void){	int row, column;
	for (row=0; row < MAXROW; row++){
		for (column=0; column < MAXCOLUMN; column++){
			if (enemy[row][column].life == alive){ // if Enemy is alive then
				if(enemy[row][column].vx == 1){ // if Enemy is going right +1
					if(enemy[row][column].x + enemy[row][column].vx > (MAX_X-1)-ENEMY10W){ // if Enemy x + step > MaxX then
						return 1;   
					} 
				}else{ 
					if(enemy[row][column].x - enemy[row][column].vx < 2){ // if enemy x - step < minx then
						return 1; // return true 
					} 
				} 
			} 
		}
	}
	return 0;
}
unsigned long FrameCount=0;
void drawEnemy(void){ int row, column;
	for(row=0; row < MAXROW; row++){
		for(column=0; column < MAXCOLUMN; column++){
			if(enemy[row][column].needDraw == 1){
				if(enemy[row][column].life == alive){
					Nokia5110_PrintBMP(enemy[row][column].x, enemy[row][column].y, enemy[row][column].image[FrameCount], 0);
				} else{
					Nokia5110_PrintBMP(enemy[row][column].x, enemy[row][column].y, enemy[row][column].image[0], 0);
					enemy[row][column].needDraw = 0;
				}		
			}
		}
	}
	// FrameCount = (FrameCount+1) % maxNumFrames
  FrameCount = (FrameCount+1)&0x01; // 0,1,0,1,...
}
unsigned short checkCollision(sprite_t sprite1, sprite_t sprite2){
	int32_t sprite1Left = sprite1.x;
	int32_t sprite1Right = sprite1.x + (sprite1.w-1);
	int32_t sprite1Top = sprite1.y;
	int32_t sprite1Bottom = sprite1.y + (sprite1.h-1);
	int32_t sprite2Left = sprite2.x;
	int32_t sprite2Right = sprite2.x + (sprite2.w-1);
	int32_t sprite2Top = sprite2.y;
	int32_t sprite2Bottom = sprite2.y + (sprite2.h-1);
	
	if ((sprite1Left < sprite2Right) &&
			(sprite1Right > sprite2Left) &&
			(sprite1Top < sprite2Bottom) &&
			(sprite1Bottom > sprite2Top))	{
		return 1;
	}else{
		return 0;
	}
}
void checkMissileCollisions(void){ int missile;
	int row, column;
	unsigned short missileHasHitEnemy;
	for(missile = 0; missile<MAXPLAYERMISSILES; missile++){
		if(playerMissiles[missile].life == alive){
			for(row=0; row < MAXROW; row++){
				for(column=0; column < MAXCOLUMN; column++){
					if(enemy[row][column].needDraw == 1) {
						if(enemy[row][column].life == alive){
							missileHasHitEnemy = checkCollision(
								playerMissiles[missile], 
								enemy[row][column]);
						}
						if(missileHasHitEnemy == 1){
							PlayerScore += -10*row+30;
							enemy[row][column].life = dead;
							enemy[row][column].image[0] = SmallExplosion0;
							enemy[row][column].image[1] = SmallExplosion1; // blank screen					
							playerMissiles[missile].life = dead;
							playerMissiles[missile].needDraw = 0;
							Sound_Killed(); // invader killed sound
						}
					}
				}
			}
		}
	}
}
void movePlayerMissile(void){int missile;
	for(missile = 0; missile<MAXPLAYERMISSILES; missile++){
		if(playerMissiles[missile].needDraw){
			if(playerMissiles[missile].life == alive){
				playerMissiles[missile].y = playerMissiles[missile].y - playerMissiles[missile].vy;
				if(playerMissiles[missile].y < 0){
					playerMissiles[missile].life = dead;
					playerMissiles[missile].needDraw = 0;
				}
			}
		}
	}
}
void drawPlayerMissile(void){int missile;
	for(missile = 0; missile<MAXPLAYERMISSILES; missile++){
		if(playerMissiles[missile].needDraw){
			if(playerMissiles[missile].life == alive){
				Nokia5110_PrintBMP(playerMissiles[missile].x, playerMissiles[missile].y, playerMissiles[missile].image[0], 0);
			}
		}
	}
}
void createEnemyLaser(void){int laser=0;
	int row, column;
	
	do {
		if (!enemyLasers[laser].needDraw){
			row = Random32() % MAXROW;
			column = Random32() % MAXCOLUMN;
			if (enemy[row][column].life == alive){
				enemyLasers[laser].x = enemy[row][column].x + ((enemy[row][column].w/2)-1);
				enemyLasers[laser].y = enemy[row][column].y + ((enemy[row][column].h/2)-1);
				enemyLasers[laser].needDraw = 1;
				enemyLasers[laser].life = alive;	
				Sound_Shoot();
			}
		}
		laser++;
	} while (laser < MAXENEMYLASORS);
}
void drawEnemyLasers(void){int laser;
	for(laser = 0; laser < MAXENEMYLASORS; laser++){
		if(enemyLasers[laser].needDraw){
			if(enemyLasers[laser].life == alive){
				Nokia5110_PrintBMP(enemyLasers[laser].x, enemyLasers[laser].y, enemyLasers[laser].image[0], 0);
			}
		}
	}
}
void moveEnemyLasers(void){int laser;
	for (laser = 0; laser < MAXENEMYLASORS; laser++){
		enemyLasers[laser].y += enemyLasers[laser].vy;
		
		if (enemyLasers[laser].y > MAX_Y){
			enemyLasers[laser].life = dead;
			enemyLasers[laser].needDraw = 0;
		}
	}
}
void checkLaserCollisions(void){int laser;
	for (laser = 0; laser < MAXENEMYLASORS; laser++){
		// player has been hit
		if (checkCollision(enemyLasers[laser],playerShip)) {	
			Sound_Explosion();
			GameState = stateNewLife;
		}
	}
}
void drawPlayerScore(unsigned short n){
	Nokia5110_SetCursor(0, 0);
  if(n < 10){
    Nokia5110_OutChar(n+'0'); /* n is between 0 and 9 */
  } else if(n<100){
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  } else if(n<1000){
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  } else if(n<10000){
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  } else {
    Nokia5110_OutChar(n/10000+'0'); /* ten-thousands digit */
    n = n%10000;
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
	// Display Player lives
	Nokia5110_SetCursor(9, 0);
  Nokia5110_OutString("L:");
	Nokia5110_SetCursor(10, 0);
	Nokia5110_OutChar(PlayerLives+'0');
}

void StartGame(void){
	do {
		Nokia5110_Clear();
		Nokia5110_SetCursor(3, 0);
		Nokia5110_OutString("SPACE");
		Nokia5110_SetCursor(2, 1);
		Nokia5110_OutString("INVADERS");
		Nokia5110_SetCursor(0, 3);
		Nokia5110_OutString("Press a key");

		Nokia5110_SetCursor(2, 5);
		Nokia5110_OutString("to start");
		Delay1ms(50);
		SW1 = Switch_Input1(); // read in switch value
	} while(!SW1);
	GameState = statePlayGame;
}

void NewLife(void){int laser; int missile;
	// remove enemy bullets
	for (laser = 0; laser < MAXENEMYLASORS; laser++){
			enemyLasers[laser].life = dead;
			enemyLasers[laser].needDraw = 0;
	}
	// remove player bullets
	for (missile = 0; missile < MAXPLAYERMISSILES; missile++){
		playerMissiles[missile].life = dead;
		playerMissiles[missile].needDraw = 0;
	}
	playerShip.image[0] = BigExplosion0;
	playerShip.image[1] = BigExplosion1;
	PlayerLives--;
}
void GameOver(void){
	Nokia5110_Clear();
  Nokia5110_SetCursor(1, 1);
  Nokia5110_OutString("GAME OVER");
  Nokia5110_SetCursor(1, 2);
  Nokia5110_OutString("Nice try,");
  Nokia5110_SetCursor(1, 3);
  Nokia5110_OutString("Earthling!");
  Nokia5110_SetCursor(2, 4);
  Nokia5110_OutUDec(PlayerScore);
	Delay1ms(50);
}

void AllEnemyDead(void){
	Nokia5110_Clear();
  Nokia5110_SetCursor(1, 1);
  Nokia5110_OutString("You Win!");
	Nokia5110_SetCursor(2, 3);
  Nokia5110_OutString("Score:");
  Nokia5110_SetCursor(2, 5);
  Nokia5110_OutUDec(PlayerScore);
}

void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
void Delay1ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 8000;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
