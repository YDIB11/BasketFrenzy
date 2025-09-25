#include "P_Map16x16.h"
#include "controls.h"
#include "background.h"
#include "numbers.h"
#include "basketcourt2.h"
#include "basketfrenzysubbg.h"
#include <nds/arm9/video.h>

u16* mapMemory;
int MapCols;
int MapRows;

//Declare the tiles emptyTile and fullTile as explained in the manual
u8 emptyTile[64] =
{
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
};

u8 fullTile[64] =
{
	255,254,255,255,255,255,254,255,
	254,254,255,255,255,255,254,254,
	255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,
	254,254,255,255,255,255,254,254,
	255,254,255,255,255,255,254,255
};

u8 bombTile[64] =
{
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254
};

u8 overTile[64] =
{
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253
};

void P_Map16x16_configureBG2()
{
    //Initialize Background
    BGCTRL[2] = BG_MAP_BASE(0) | BgSize_B16_256x256;

    //Affine Marix Transformation
    REG_BG2PA = 256;
    REG_BG2PC = 0;
    REG_BG2PB = 0;
    REG_BG2PD = 256;

    //Initialize pointer to the graphic memory
    //mapMemory = ...
    mapMemory = BG_GFX;
}

void P_Map16x16_configureBG0_Sub()
{
	//Configure the Background
	BGCTRL_SUB[0] = BG_32x32 | BG_COLOR_16 | BG_MAP_BASE(24) | BG_TILE_BASE(4);
	//Copy tiles and palette
	dmaCopy(numbersTiles, BG_TILE_RAM_SUB(4), numbersTilesLen);
	dmaCopy(numbersPal, &BG_PALETTE_SUB[128], numbersPalLen);
	dmaCopy(numbersPal, &BG_PALETTE_SUB[144], numbersPalLen);
	BG_PALETTE_SUB[129] = ARGB16(1,31,0,0);
	BG_PALETTE_SUB[145] = ARGB16(1,0,31,0);

	//Set map to 0 (clean)
	int i = 32*32;
	while(i--)
		BG_MAP_RAM_SUB(24)[i] = 0;
}

void P_Map16x16_configureBG2_Sub()
{
	//Configure BG 2 Sub. (Do not forget the BG_BMP_BASE configuration)
    BGCTRL_SUB[2] = BG_BMP_BASE(0) | BgSize_B8_256x256;

    //Affine Marix Transformation
    REG_BG2PA_SUB = 256;
    REG_BG2PC_SUB = 0;
    REG_BG2PB_SUB = 0;
    REG_BG2PD_SUB = 256;


    swiCopy(basketfrenzysubbgPal, BG_PALETTE_SUB, basketfrenzysubbgPalLen);
    swiCopy(basketfrenzysubbgBitmap, BG_GFX_SUB, basketfrenzysubbgBitmapLen);
}

void P_Map16x16_configureBG0()
{
	/*
	 * GRAPHICS III
	 *
	 * This function is called in the initialization of the P_Map16x16 to
	 * configure the Background 0 (on the top).
	 *
	 * This background is going to be used to draw the game field where
	 * the blocks are falling.
	 *
	 * The function does not return anything and it does not have any input
	 * arguments. Background 0 (BG0) should be configured in the tiled mode
	 * as a 32x32 tile grid and using one general color palette of 256
	 * components (256 colors). The pointer to the map 'mapMemory' must be
	 * correctly assigned before returning from the function
	 */

	//Configure BG 0 to represent the game field
	BGCTRL[0] = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(25) | BG_TILE_BASE(4);

	//Copy the empty tile and the full tile to the corresponding RAM location
	//according to the chosen TILE_BASE. If dmaCopy is used, do not forget to
	//cast the destination pointer as a 'byte pointer'
	//Hint: Use the macro BG_TILE_RAM to get the destination address
	dmaCopy(emptyTile, (u8*)BG_TILE_RAM(4), 64);
	dmaCopy(fullTile, (u8*)BG_TILE_RAM(4) + 64, 64);
	dmaCopy(bombTile, (u8*)BG_TILE_RAM(4) + 128, 64);
	dmaCopy(overTile, (u8*)BG_TILE_RAM(4) + 192, 64);

	//Assign components 254 and 255 as explained in the manual
	BG_PALETTE[254] = ARGB16(1,0,0,0);
	BG_PALETTE[255] = ARGB16(1,31,15,0);
	BG_PALETTE[253] = ARGB16(1,31,0,0);
	

	//Set the pointer mapMemory to the RAM location of the chosen MAP_BASE
	//Hint: use the macro BG_MAP_RAM
	mapMemory = (u16*)BG_MAP_RAM(25);
}

void P_Map16x16_configureBG3()
{
	/*GRAPHICS III
	 *
	 * This function is called in the initialization of the P_Map16x16 to
	 * configure the Background 3.
	 *
	 * This background is going to be used to background wall-paper.
	 *
	 * In exercise 2 the Background 3 (BG3) should be configure in rotoscale
	 * mode with a 8 bits of pixel depth. Do not forget to copy the palette
	 * correctly.
	 *
	 * In exercise 5 Background 3 (BG3) should be configured in the tiled mode
	 * as a 32x32 tile grid and using one general color palette of 256
	 * components (256 colors).
	 */

	//Configure BG 3 for the background image as explained before
	BGCTRL[3] = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(26) | BG_TILE_BASE(0);

	//Copy tiles, map and palette in the memory (use swicopy or memcpy)
	swiCopy(basketcourt2Tiles, BG_TILE_RAM(0), basketcourt2TilesLen);
	swiCopy(basketcourt2Map, BG_MAP_RAM(26), basketcourt2MapLen);
	swiCopy(basketcourt2Pal, BG_PALETTE, basketcourt2PalLen);

}

void P_Map16x16_Init( int cols, int rows )
{
    MapCols=cols;
    MapRows=rows;
#ifdef ROTOSCALE
    //Configure BG2
    P_Map16x16_configureBG2();
    // Configure Bottom background
    P_Map16x16_configureBG2_Sub();
#endif

#ifdef TILES
    //Configure BG3 for background image
    P_Map16x16_configureBG3();
    //Configure BG0 for game
    P_Map16x16_configureBG0();
    // Configure Bottom background
    P_Map16x16_configureBG2_Sub();
    // Configure BG0 in the sub engine
    P_Map16x16_configureBG0_Sub();
#endif
}


void SetMap16x16To(int col, int row, int type) {
    int value;
    switch (type) {
        case 1: // Basketball
            value = 1;
            break;
        case 2: // Bomb
            value = 2;
            break;
        case 3: // Game Over (Red)
            value = 3;
            break;
        default: // Empty
            value = 0;
    }

    int index32 = (row * 2) * 32 + (col * 2);
    mapMemory[index32] = value;
    mapMemory[index32 + 1] = value;
    mapMemory[index32 + 32] = value;
    mapMemory[index32 + 32 + 1] = value;
}