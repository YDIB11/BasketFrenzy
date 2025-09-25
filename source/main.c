#include <nds.h>
#include <time.h>
#include <fat.h>

#include "P_Initializer.h"  // For P_InitNDS()
#include "P_GameLogic.h"    // We'll create this file

int main(void)
{
    // Basic DS init (replicates part of your original main)
    fatInitDefault();
    consoleDemoInit();
    P_InitNDS();

    // Seed the random generator 
    srand(time(NULL));

    // Now call the game logic init function:
    //   - Reads score
    //   - Shows the sprite
    //   - Sets up timers
    //   - Etc.
    Gameplay_Init();

    // Hand over control to the main game loop
    //   - Movement, freeze checks, collisions, etc.
    Gameplay_Loop();

    // (Will never reach here if Gameplay_Loop() never exits)
    return 0;
}
