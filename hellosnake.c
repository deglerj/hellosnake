#include <rand.h>
#include <gb/gb.h>
#include <gb/hardware.h>

#define STOPPED 0
#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

void initGame();
void pollKeys();
void updatePlayer();

UINT8 i; // Re-usabe loop variable

UINT8 playerX, playerY;

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
const unsigned char sprites[] =
{
    0x24,0x24,0x5E,0x7E,0xAF,0xDB,0x8F,0xFF,
    0x4E,0x7E,0x3C,0x3C,0x00,0x18,0x00,0x24
};

void main() {

    initGame();

    while(1) {
        pollKeys();
        updatePlayer();

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

    set_sprite_data(0, 1, sprites);
    set_sprite_tile(0,0);
    playerX = 64;
    playerY = 64;
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

void updatePlayer() {
    switch(direction) {
        case DOWN:
            playerY++;
            if(playerY == 145) {
                playerY = 144;
            }
            break;
        case UP:
            playerY--;
            if(playerY == 23) {
                playerY = 24;
            }
            break;
        case RIGHT:
            playerX++;
            if(playerX == 153) {
                playerX = 152;
            }
            break;
        case LEFT:
            playerX--;
            if(playerX == 15) {
                playerX = 16;
            }
            break;
    }


    set_sprite_tile(0,0);
    move_sprite(0,playerX,playerY);
}
