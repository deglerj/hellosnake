#include <rand.h>
#include <gb/gb.h>
#include <gb/hardware.h>

#define STOPPED 0
#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

#define MAX_LENGTH 10 // TODO rename
#define BODY_PART_DISTANCE 5
#define COORD_COUNT 50 // MAX_LENGTH * BODY_PART_DISTANCE

void initGame();
void pollKeys();
void movePlayer();
void drawPlayer();
UINT8 getBodyPartCoordsIndex(UINT8);

UINT8 i, j; // Re-usable counter variables

UINT8 playerLength = 5;
UINT8 playerCoords[COORD_COUNT][2]; // x, y
UINT8 playerCoordsIndex = 0; // TODO rename
UINT8 playerCoordsLastIndex = 0;

UINT8 direction = STOPPED;

// Background tiles
// 0 = border tiles
// 1 = grass tile
const unsigned char bkgData[] =
{
    0xFF,0xFF,0xFD,0x83,0xC1,0xBF,0xC5,0xBF,
    0xCD,0xBF,0xDD,0xBF,0x81,0xFF,0xFF,0xFF,
    0x12,0x00,0x45,0x00,0xA0,0x00,0x0A,0x00,
    0x90,0x00,0x25,0x00,0x42,0x00,0x29,0x00
};

const UINT8 borderTiles[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const UINT8 normalTiles[20] = {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0};

// Sprites
// 0 = snake head
// 1 = dark body
// 2 = light body
const unsigned char sprites[] =
{
    0x25,0x24,0x5E,0x7E,0xAF,0xDB,0x8F,0xFF,
    0x4E,0x7E,0x3C,0x3C,0x00,0x18,0x00,0x24,
    0x3C,0x3C,0x42,0x7E,0x81,0xFF,0x81,0xFF,
    0x81,0xFF,0x81,0xFF,0x42,0x7E,0x3C,0x3C,
    0x3C,0x3C,0x7E,0x42,0xFF,0x81,0xFF,0x81,
    0xFF,0x81,0xFF,0x81,0x7E,0x42,0x3C,0x3C
};

void main() {

    initGame();

    while(1) {
        pollKeys();
        movePlayer();
        drawPlayer();

        HIDE_WIN;
        SHOW_SPRITES;
        SHOW_BKG;

        wait_vbl_done();
    }
  }

void initGame() {
    DISPLAY_ON;

    NR52_REG = 0x8F; // Sound on
    NR51_REG = 0x11; // Enable sound channels
    NR50_REG = 0x77; // Turn volume to max

    initrand(DIV_REG); // Seed randomizer

    set_bkg_data(0, 2, bkgData); // Store background data in BGK VRAM

    // Draw background (first and last row uses border tiles, rest normal tiles)
    set_bkg_tiles(0, 0, 20, 1, borderTiles);
    for(i = 1; i != 17; i++) {
        set_bkg_tiles(0, i, 20, 1, normalTiles);
    }
    set_bkg_tiles(0, 17, 20, 1, borderTiles);

    set_sprite_data(0, 3, sprites);

    set_sprite_tile(4,2);
    set_sprite_tile(3,1);
    set_sprite_tile(2,2);
    set_sprite_tile(1,1);
    set_sprite_tile(0,0);

    for(i = 0; i != COORD_COUNT; i++) {
        playerCoords[i][0] = 83;
        playerCoords[i][1] = 84 - i;
    }
}

void pollKeys() {
    if(joypad() & J_DOWN) {
        direction = DOWN;
    }
    else if(joypad() & J_UP) {
        direction = UP;
    }
    else if(joypad() & J_RIGHT) {
        direction = RIGHT;
    }
    else if(joypad() & J_LEFT) {
        direction = LEFT;
    }
}

void movePlayer() {
    if(direction == STOPPED) {
        return;
    }

    // Move coordinates array index
    playerCoordsLastIndex = playerCoordsIndex;
    if(playerCoordsIndex == 0) {
        playerCoordsIndex = COORD_COUNT - 1;
    }
    else {
        playerCoordsIndex--;
    }

    // Copy coordinates (one of the will be modified based on the direction)
    playerCoords[playerCoordsIndex][0] = playerCoords[playerCoordsLastIndex][0];
    playerCoords[playerCoordsIndex][1] = playerCoords[playerCoordsLastIndex][1];

    // Move head
    switch(direction) {
        case DOWN:
            playerCoords[playerCoordsIndex][1] = playerCoords[playerCoordsLastIndex][1] + 1;
            break;
        case UP:
            playerCoords[playerCoordsIndex][1] = playerCoords[playerCoordsLastIndex][1] - 1;
            break;
        case RIGHT:
            playerCoords[playerCoordsIndex][0] = playerCoords[playerCoordsLastIndex][0] + 1;
            break;
        case LEFT:
            playerCoords[playerCoordsIndex][0] = playerCoords[playerCoordsLastIndex][0] - 1;
            break;
    }

}

void drawPlayer() {
    for(i = 0; i !=  playerLength; i++) {
        j = getBodyPartCoordsIndex(i);
        move_sprite(i, playerCoords[j][0], playerCoords[j][1]);
    }
}

UINT8 getBodyPartCoordsIndex(UINT8 partNo) {
    if(partNo == 0) {
        return playerCoordsIndex;
    }

    return (playerCoordsIndex + (partNo * BODY_PART_DISTANCE)) % COORD_COUNT; //TODO optimize
}
