#include "P_Score.h"
#include "numbers.h"
#include <nds.h>
#include <stdio.h>
#include <fat.h>

int score = 0, max_score = 0;

// Macro Definitions (if missing in your codebase)
#ifndef BG_MAP_RAM_SUB
#define BG_MAP_RAM_SUB(base) ((u16*)BG_MAP_RAM((base)))
#endif

#ifndef TILE_PALETTE
#define TILE_PALETTE(p) ((p) << 12)
#endif

void updateScore() {
    score++;
    displayScore();

    if (score > max_score) {
        max_score = score;
        writeMaxScore();
        displayMaxScore();
    }
}

void displayScore() {
    int i, j, number;
    for (i = 5; i > 0; i--) {
        j = i - 1;
        number = 1;
        while (j--) number *= 10; // Calculate power of 10
        number = score / number;
        number %= 10; // Get the current digit to display

        BG_MAP_RAM_SUB(24)[(5 - i + 1) * 32 + 1] = numbersMap[(10 - number) * 2] | TILE_PALETTE(9);
        BG_MAP_RAM_SUB(24)[(5 - i + 1) * 32 + 2] = numbersMap[(10 - number) * 2 + 1] | TILE_PALETTE(9);
    }
}

void displayMaxScore() {
    int i, j, number;
    for (i = 5; i > 0; i--) {
        j = i - 1;
        number = 1;
        while (j--) number *= 10; // Calculate power of 10
        number = max_score / number;
        number %= 10; // Get the current digit to display

        BG_MAP_RAM_SUB(24)[1 + (23 - i) * 32] = numbersMap[(10 - number) * 2] | TILE_PALETTE(8);
        BG_MAP_RAM_SUB(24)[1 + (23 - i) * 32 + 1] = numbersMap[(10 - number) * 2 + 1] | TILE_PALETTE(8);
    }
}

void displayFreezeAvailability(int available) {
    int i, j, number;
    int baseX = 100 / 8; // Convert 100px to tile units
    int baseY = (100 / 8) + 7; // Adjust to show 20px lower than the original position

    for (i = 1; i >= 0; i--) { 
        j = i;
        number = 1;
        while (j--) number *= 10; // Calculate power of 10
        number = available / number;
        number %= 10; // Get the current digit to display

        // Adjust indexing logic to match score/high score
        BG_MAP_RAM_SUB(24)[(baseY - i) * 32 + baseX] = numbersMap[(10 - number) * 2] | TILE_PALETTE(8);
        BG_MAP_RAM_SUB(24)[(baseY - i) * 32 + baseX + 1] = numbersMap[(10 - number) * 2 + 1] | TILE_PALETTE(8);
    }
}





void readMaxScore() {
    FILE* file = fopen("/BasketGameRecord.txt", "r");
    if (file != NULL) {
        fscanf(file, "%i\n", &max_score);
        fclose(file);
    }
}

void writeMaxScore() {
    FILE* file = fopen("/BasketGameRecord.txt", "w+");
    if (file != NULL) {
        fprintf(file, "%i\n", max_score);
        fclose(file);
    }
}
