#include <nds.h>
#include "SpriteManager.h"
#include "basketnett.h"


// Pointer to the graphic buffer for the sprite
u16* spriteGfx;

void configureSprites() {
    // Set up memory bank for sprite engine
    VRAM_B_CR = VRAM_ENABLE | VRAM_B_MAIN_SPRITE_0x06400000;

    // Initialize sprite engine
    oamInit(&oamMain, SpriteMapping_1D_32, false);

    // Allocate memory for the sprite graphic
    spriteGfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);

    // Copy the palette and graphic data for the sprite
    swiCopy(basketnettPal, SPRITE_PALETTE, basketnettPalLen / 2);
    swiCopy(basketnettTiles, spriteGfx, basketnettTilesLen / 2);
}

void updateSprite(int x, int y) {
    oamSet(&oamMain,              // OAM handler
           0,                     // Sprite index (0 for the first sprite)
           x, y,                  // X and Y coordinates
           0,                     // Priority (0 is the highest)
           0,                     // Palette index (0 by default)
           SpriteSize_32x32,      // Sprite size
           SpriteColorFormat_256Color, // Color format
           spriteGfx,             // Pointer to the graphic
           -1,                    // Affine transform index (-1 for none)
           false,                 // Double size (not applicable without affine)
           false,                 // Hide the sprite (false to display it)
           false,                 // Horizontal flip
           false,                 // Vertical flip
           false                  // Mosaic
    );
    oamUpdate(&oamMain);          // Update the OAM engine
}
