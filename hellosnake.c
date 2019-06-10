#include <rand.h>
#include <gb/gb.h>
#include <gb/hardware.h>
#include <gb/drawing.h>

#define STOPPED 0
#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

#define FIELD_WIDTH 18
#define FIELD_HEIGHT 16

#define MAX_PLAYER_LENGTH 100
#define SPRITE_SIZE 8

void initGame();
void pollKeys();
void movePlayer();
void drawPlayer();
UINT8 mapPlayerXToFieldCoords(UINT8 playerX);
UINT8 mapPlayerYToFieldCoords(UINT8 playerY);
UINT8 getPlayerCoordsIndex(UINT8 offset);

UINT8 i, j, k; // Re-usable variables

UINT8 playerLength = 40;
UINT8 playerCoords[MAX_PLAYER_LENGTH + 1][2];
UINT8 playerCoordsIndex = 0;

UINT8 xToFieldCoordsCache[FIELD_WIDTH];
UINT8 yToFieldCoordsCache[FIELD_HEIGHT];

UINT8 vFrameCount = -1;

UINT8 direction = STOPPED;

// Background tiles
// 0 = blank
// 1 = border
// 2 = head down
// 3 = head left
// 4 = head up
// 5 = head right
// 6 = body 1
// 7 = body 2
// 8 = tail down
// 9 = tail left
// 10 = tail up
// 11 = tail right
const unsigned char bkgData[] =
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xFF,0xFF,0xFD,0x83,0xC1,0xBF,0xC5,0xBF,
  0xCD,0xBF,0xDD,0xBF,0x81,0xFF,0xFF,0xFF,
  0x00,0xFF,0x7E,0x81,0x7E,0x81,0x5A,0x81,
  0x7E,0xA5,0x3C,0xC3,0x00,0x7E,0x18,0x18,
  0x00,0x3F,0x1E,0x61,0x36,0x51,0xBE,0xC1,
  0xBE,0xC1,0x36,0x51,0x1E,0x61,0x00,0x3F,
  0x18,0x18,0x00,0x7E,0x3C,0xC3,0x7E,0xA5,
  0x5A,0x81,0x7E,0x81,0x7E,0x81,0x00,0xFF,
  0x00,0xFC,0x78,0x86,0x6C,0x8A,0x7D,0x83,
  0x7D,0x83,0x6C,0x8A,0x78,0x86,0x00,0xFC,
  0x00,0xFF,0x7E,0x81,0x7E,0x81,0x7E,0x81,
  0x7E,0x81,0x7E,0x81,0x7E,0x81,0x00,0xFF,
  0xFF,0xFF,0x81,0xFF,0x81,0xFF,0x81,0xFF,
  0x81,0xFF,0x81,0xFF,0x81,0xFF,0xFF,0xFF,
  0x00,0xFF,0x7E,0x81,0x7E,0x81,0x7E,0x81,
  0x7E,0x81,0x3C,0xC3,0x00,0x7E,0x00,0x00,
  0x00,0x3F,0x1E,0x61,0x3E,0x41,0x3E,0x41,
  0x3E,0x41,0x3E,0x41,0x1E,0x61,0x00,0x3F,
  0x00,0x00,0x00,0x7E,0x3C,0xC3,0x7E,0x81,
  0x7E,0x81,0x7E,0x81,0x7E,0x81,0x00,0xFF,
  0x00,0xFC,0x78,0x86,0x7C,0x82,0x7C,0x82,
  0x7C,0x82,0x7C,0x82,0x78,0x86,0x00,0xFC
};

UINT8 singleTile[1] = {0};
const UINT8 borderRowTiles[20] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
const UINT8 normalRowTiles[20] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};


// Sprite tiles
// 0 = blank
unsigned char spriteData[] =
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


void main() {
    initGame();
    while(1) {
        pollKeys();

        if(vFrameCount == 5) {
            movePlayer();
            drawPlayer();
            vFrameCount = 0;
        }
        else {
            vFrameCount++;
        }

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

    set_bkg_data(0, 12, bkgData); // Store background data in BGK VRAM

    // Draw initial background
    set_bkg_tiles(0, 0, 20, 1, borderRowTiles);
    for(i = 1; i != FIELD_HEIGHT + 1; i++) {
        set_bkg_tiles(0, i, 20, 1, normalRowTiles);
    }
    set_bkg_tiles(0, FIELD_HEIGHT + 1, 20, 1, borderRowTiles);

    // Start player at center coords
    for(i = 0; i != playerLength; i++) {
        playerCoords[getPlayerCoordsIndex(i)][0] = FIELD_WIDTH / 2;
        playerCoords[getPlayerCoordsIndex(i)][1] = (FIELD_HEIGHT / 2) - i;
    }

    // Precalculate coord mappings
    for(i = 0; i != FIELD_WIDTH; i++) {
        xToFieldCoordsCache[i] = 16 + (i * SPRITE_SIZE);
    }
    for(i = 0; i != FIELD_HEIGHT; i++) {
        yToFieldCoordsCache[i] = 24 + (i * SPRITE_SIZE);
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

    // Move coords array index to next position
    playerCoordsIndex--;
    if(playerCoordsIndex == -1) {
        playerCoordsIndex = MAX_PLAYER_LENGTH;
    }

    // Move head
    k = getPlayerCoordsIndex(1);
    switch(direction) {
        case DOWN:
            playerCoords[playerCoordsIndex][0] = playerCoords[k][0];
            playerCoords[playerCoordsIndex][1] = playerCoords[k][1] + 1;
            break;
        case UP:
            playerCoords[playerCoordsIndex][0] = playerCoords[k][0];
            playerCoords[playerCoordsIndex][1] = playerCoords[k][1] - 1;
            break;
        case RIGHT:
            playerCoords[playerCoordsIndex][0] = playerCoords[k][0] + 1;
            playerCoords[playerCoordsIndex][1] = playerCoords[k][1];
            break;
        case LEFT:
            playerCoords[playerCoordsIndex][0] = playerCoords[k][0] - 1;
            playerCoords[playerCoordsIndex][1] = playerCoords[k][1];
            break;
    }


}

void drawPlayer() {
    // Clear last tails posotion
    singleTile[0] = 0;
    k = getPlayerCoordsIndex(playerLength);
    if(playerCoords[k][0] != 0) {
        set_bkg_tiles(playerCoords[k][0], playerCoords[k][1], 1, 1, singleTile);
    }

    // Draw player tiles
    for(j = playerLength - 1; j != -1; j--) {
        if(j == 0) {
            singleTile[0] = 2;
        }
        else if(j == playerLength - 1) {
            singleTile[0] = 10;
        }
        else if(j & 1) {
            singleTile[0] = 7;
        }
        else {
            singleTile[0] = 6;
        }
        k = getPlayerCoordsIndex(j);
        set_bkg_tiles(playerCoords[k][0], playerCoords[k][1], 1, 1, singleTile);
    }
}

UINT8 mapPlayerXToFieldCoords(UINT8 playerX) {
    return xToFieldCoordsCache[playerX];
}

UINT8 mapPlayerYToFieldCoords(UINT8 playerY) {
    return yToFieldCoordsCache[playerY];
}

UINT8 getPlayerCoordsIndex(UINT8 offset) {
    if(playerCoordsIndex + offset > MAX_PLAYER_LENGTH) {
        return playerCoordsIndex + offset - MAX_PLAYER_LENGTH - 1;
    }
    else if(playerCoordsIndex + offset < 0) {
        return playerCoordsIndex + offset + MAX_PLAYER_LENGTH + 1;
    }
    else {
        return playerCoordsIndex + offset;
    }
}
