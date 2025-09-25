#include <nds.h>
#include <fat.h>
#include <time.h>

#include "P_GameLogic.h"
#include "P_Initializer.h"  // if something here is needed, else remove
#include "P_Score.h"        // for readMaxScore(), updateScore(), etc.
#include "SpriteManager.h"  // for configureSprites(), updateSprite()
#include "P_Map16x16.h"     
#include "P_Audio.h"        
#include "P_GameOver.h"
// -----------------------------------------------------------------
// Same defines, structures, and variables from your original main
// -----------------------------------------------------------------
#define MAX_BALLS 4
#define MAX_BOMBS 2

typedef struct {
    int x;
    int y;
    bool active;
} Ball;

typedef struct {
    int x;
    int y;
    bool active;
} Bomb;

Ball balls[MAX_BALLS];
Bomb bombs[MAX_BOMBS];

// Basket / net location
int netX = (256 - 32) / 2;
int netY = 192 - 32;

// Core game states
bool gameRunning    = true;
bool freezeActive   = false;
int  freezeAvailable= 0;

// -----------------------------------------------------------------
// Forward declarations (private functions) that were in original main
// -----------------------------------------------------------------
void initializeBalls(void);
void initializeBombs(void);
void spawnBall(void);
void spawnBomb(void);
void clearAllBallsAndBombs(void);
void ISR_TIMER0(void);
void ISR_TIMER1(void);
void handleFreeze(void);
void displayGameOver(void);

// -----------------------------------------------------------------
// 1) Gameplay_Init() -- sets up everything but doesn't loop forever
// -----------------------------------------------------------------
void Gameplay_Init(void)
{
    // === Replicate the original main’s setup (after P_InitNDS()) ===

    // Score handling
    readMaxScore();
    displayScore();
    displayMaxScore();

    // Initialize freeze availability
    freezeAvailable = 0;
    displayFreezeAvailability(freezeAvailable);

    // Configure sprites (the basket)
    configureSprites();

    // Initialize balls/bombs, then spawn first ball
    initializeBalls();
    initializeBombs();
    spawnBall();

    // Setup Timer 0 for ball/bomb logic
    TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024 | TIMER_IRQ_REQ;
    TIMER_DATA(0) = TIMER_FREQ_1024(2);
    irqSet(IRQ_TIMER0, ISR_TIMER0);
    irqEnable(IRQ_TIMER0);
}

// -----------------------------------------------------------------
// 2) Gameplay_Loop() -- the main "while(true)" from your old code
// -----------------------------------------------------------------
void Gameplay_Loop(void)
{
    while (true) 
    {

        // Read user input (buttons, stylus, etc.)
        scanKeys();
        int keys = keysHeld();
        // If the game already ended, just wait
        if (!gameRunning) {
            if (keys & KEY_START) { // Restart on Start button press

                // Clear "Game Over" text
                clearGameOverText();
                // Reset game state
                score = 0;
                gameRunning = true;
                freezeAvailable = 0;

                // Reset sprite position to center
                netX = (256 - 32) / 2; // Center horizontally
                netY = 192 - 32;       // Keep fixed vertically

                // Clear and reinitialize game elements
                clearAllBallsAndBombs();
                initializeBalls();
                initializeBombs();
                spawnBall();

                // Reset displays
                displayScore();
                displayMaxScore();
                displayFreezeAvailability(freezeAvailable);

                // Re-enable TIMER0
                TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024 | TIMER_IRQ_REQ;
            }
            swiWaitForVBlank();
            continue;
        }

        // Freeze if 'A' is pressed
        if (keys & KEY_A) {
            handleFreeze();
        }

        // Move basket with D-PAD
        if ((keys & KEY_RIGHT) && (netX < (256 - 32))) netX += 2;
        if ((keys & KEY_LEFT)  && (netX > 0))           netX -= 2;

        // Read stylus for left/right arrow areas or freeze
        touchPosition touch;
        touchRead(&touch);

        bool left_arrow  = false;
        bool right_arrow = false;

        // Basic bounding checks for stylus
        if (touch.py > 25 && touch.py < 80) {
            if      (touch.px >  45 && touch.px < 100) left_arrow  = true;
            else if (touch.px > 155 && touch.px < 210) right_arrow = true;
        } 
        else if (touch.py > 100 && touch.py < 150) {
            if (touch.px > 100 && touch.px < 150) {
                // Press freeze button in stylus area
                handleFreeze();
            }
        }

        if (right_arrow && (netX < (256 - 32))) netX += 2;
        if (left_arrow  && (netX > 0))          netX -= 2;

        // Update basket sprite’s position
        updateSprite(netX, netY);

        // Wait for next frame
        swiWaitForVBlank();
    }
}

// -----------------------------------------------------------------
// The rest: exactly as in your old main.c
// -----------------------------------------------------------------
void initializeBalls(void) {
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].x = 1 + rand() % 14;
        balls[i].y = -1;
        balls[i].active = false;
    }
}

void initializeBombs(void) {
    for (int i = 0; i < MAX_BOMBS; i++) {
        bombs[i].x = -1;
        bombs[i].y = -1;
        bombs[i].active = false;
    }
}

void spawnBall(void) {
    for (int i = 0; i < MAX_BALLS; i++) {
        if (!balls[i].active) {
            balls[i].x = 1 + rand() % 14;
            balls[i].y = -1;
            balls[i].active = true;
            break;
        }
    }
}

void spawnBomb(void) {
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (!bombs[i].active) {
            bombs[i].x = 1 + rand() % 14;
            bombs[i].y = -1;
            bombs[i].active = true;
            break;
        }
    }
}

void clearAllBallsAndBombs(void) {
    for (int i = 0; i < MAX_BALLS; i++) {
        if (balls[i].active) {
            SetMap16x16To(balls[i].x, balls[i].y, false);
            balls[i].active = false;
            balls[i].y = -1;
        }
    }
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (bombs[i].active) {
            SetMap16x16To(bombs[i].x, bombs[i].y, false);
            bombs[i].active = false;
            bombs[i].y = -1;
        }
    }
}

void ISR_TIMER0() {
    if (!gameRunning || freezeActive) return;

    static int ballSpawnTimer = 0;
    static int bombSpawnTimer = 0;
    static int ballSpawnInterval = 5;
    static int bombSpawnInterval = 10;

    // Move active balls
    for (int i = 0; i < MAX_BALLS; i++) {
        if (!balls[i].active) continue;

        // Remove old tile
        if (balls[i].y >= 0) {
            SetMap16x16To(balls[i].x, balls[i].y, false);
        }
        // Move down
        balls[i].y++;

        // Check if hits the basket row
        if (balls[i].y == 11) {
            int basketLeft  = netX / 16;
            int basketRight = basketLeft + 2;

            if (balls[i].x >= basketLeft && balls[i].x < basketRight) {
                Audio_PlaySound(SFX_SWISH);
                updateScore();
                balls[i].active = false;

                // If score multiple of 5 => unlock freeze
                if (score % 5 == 0) {
                    freezeAvailable = 1;
                    displayFreezeAvailability(freezeAvailable);
                }
                continue;
            }
        }

        // If it passes the basket row
        if (balls[i].y == 12) {
            gameRunning = false;
            clearAllBallsAndBombs();
            displayGameOver();
            return;
        }

        // Otherwise draw new tile
        if (balls[i].y < 12) {
            SetMap16x16To(balls[i].x, balls[i].y, 1); 
            // '1' means the "basketball" tile 
            // (in your original code it was "true," 
            //  but we’ve mapped it to 1 for clarity)
        }
    }

    // Move active bombs
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (!bombs[i].active) continue;

        // Remove old tile
        if (bombs[i].y >= 0) {
            SetMap16x16To(bombs[i].x, bombs[i].y, false);
        }
        // Move down
        bombs[i].y++;

        // Check if hits basket row
        if (bombs[i].y == 11) {
            int basketLeft  = netX / 16;
            int basketRight = basketLeft + 2;

            if (bombs[i].x >= basketLeft && bombs[i].x < basketRight) {
                Audio_PlaySound(SFX_EXPLOSION);
                gameRunning = false;
                clearAllBallsAndBombs();
                displayGameOver();
                return;
            }
        }

        // If it passes the basket row
        if (bombs[i].y == 12) {
            bombs[i].active = false;
            bombs[i].y = -1;
        } else {
            SetMap16x16To(bombs[i].x, bombs[i].y, 2); 
            // '2' = bomb tile
        }
    }
    if (score < 5) {
        ballSpawnInterval = 5;  // spawn balls slower
        bombSpawnInterval = 10; // spawn bombs slower
    }
    else if (score < 10) {
        ballSpawnInterval = 4; 
        bombSpawnInterval = 8;
    }
    else if (score < 20) {
        ballSpawnInterval = 3; 
        bombSpawnInterval = 6;
    }
    else {
        ballSpawnInterval = 2; 
        bombSpawnInterval = 4;  // spawn quickly at very high score
    }
    // Spawn new ball or bomb
    ballSpawnTimer++;
    bombSpawnTimer++;
    if (ballSpawnTimer >= ballSpawnInterval) {
        ballSpawnTimer = 0;
        spawnBall();
        Audio_PlaySound(SFX_CLUNK);
    }
    if (bombSpawnTimer >= bombSpawnInterval) {
        bombSpawnTimer = 0;
        spawnBomb();
    }
}

void ISR_TIMER1() {
    TIMER1_CR = 0;
    TIMER0_CR |= TIMER_ENABLE;
    freezeActive = false;
}

void handleFreeze() {
    if (freezeActive || freezeAvailable == 0) return;

    freezeActive = true;
    freezeAvailable = 0;
    displayFreezeAvailability(freezeAvailable);

    // Disable timer => all balls/bombs freeze
    TIMER0_CR &= ~TIMER_ENABLE;

    // Start Timer 1 for freeze duration (2 seconds)
    TIMER1_CR   = TIMER_ENABLE | TIMER_DIV_1024 | TIMER_IRQ_REQ;
    TIMER_DATA(1)= TIMER_FREQ_1024(2);
    irqSet(IRQ_TIMER1, ISR_TIMER1);
    irqEnable(IRQ_TIMER1);
}
