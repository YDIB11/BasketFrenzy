#include <nds.h>
#include "P_GameOver.h"
#include "SpriteManager.h"   // for updateSprite()
#include "P_Map16x16.h"      // for SetMap16x16To(...)

// Displays the big "GAME OVER" text
void displayGameOver(void) 
{
    // "O"
    SetMap16x16To(0,3, 3);
    SetMap16x16To(0,4, 3);
    SetMap16x16To(0,5, 3);
    SetMap16x16To(0,6, 3);
    SetMap16x16To(0,7, 3);
    SetMap16x16To(2,3, 3);
    SetMap16x16To(2,4, 3);
    SetMap16x16To(2,5, 3);
    SetMap16x16To(2,6, 3);
    SetMap16x16To(2,7, 3);
    SetMap16x16To(1,3, 3);
    SetMap16x16To(1,7, 3);

    // "V"
    SetMap16x16To(4,3, 3);
    SetMap16x16To(4,4, 3);
    SetMap16x16To(4,5, 3);
    SetMap16x16To(4,6, 3);
    SetMap16x16To(5,7, 3);
    SetMap16x16To(6,3, 3);
    SetMap16x16To(6,4, 3);
    SetMap16x16To(6,5, 3);
    SetMap16x16To(6,6, 3);

    // "E"
    SetMap16x16To(8,3, 3);
    SetMap16x16To(8,4, 3);
    SetMap16x16To(8,5, 3);
    SetMap16x16To(8,6, 3);
    SetMap16x16To(8,7, 3);
    SetMap16x16To(9,7, 3);
    SetMap16x16To(10,7,3);
    SetMap16x16To(9,3, 3);
    SetMap16x16To(10,3,3);
    SetMap16x16To(9,5, 3);

    // "R"
    SetMap16x16To(12,3,3);
    SetMap16x16To(12,4,3);
    SetMap16x16To(12,5,3);
    SetMap16x16To(12,6,3);
    SetMap16x16To(12,7,3);
    SetMap16x16To(13,3,3);
    SetMap16x16To(13,5,3);
    SetMap16x16To(14,4,3);
    SetMap16x16To(14,6,3);
    SetMap16x16To(14,7,3);

    // Move basket sprite out of the way
    updateSprite((256 - 32) / 2, 192 - 32);

    // Stop main timer
    TIMER0_CR = 0;
}

// Clears the "GAME OVER" text area
void clearGameOverText(void)
{
    // We assume "GAME OVER" occupies columns 0..14, rows 3..7
    for (int x = 0; x < 16; x++) {
        for (int y = 3; y <= 7; y++) {
            SetMap16x16To(x, y, 0); // Clear with tile '0'
        }
    }
}
