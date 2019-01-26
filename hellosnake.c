#include <rand.h>
#include <gb/gb.h>
#include <gb/hardware.h>

void initGame();
void updatePlayer();

UINT8 i, j; // Re-usabe loop variables

UINT8 playerX, playerY;

// Background tiles
// 0 = border tiles
// 1 = grass tile
const unsigned char bkgData[] =
{
    0xFE,0xFF,0xFD,0xFF,0xFB,0xFF,0xF7,0xFF,
    0xEF,0xFF,0xDF,0xFF,0xBF,0xFF,0x7F,0xFF,
    0x12,0x00,0x45,0x00,0xA0,0x00,0x0A,0x00,
    0x90,0x00,0x25,0x00,0x42,0x00,0x29,0x00
};

const UINT8 borderTiles[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const UINT8 normalTiles[20] = {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0};


void main() {

    initGame();

    while(1) {
        updatePlayer();

        HIDE_WIN;
        //SHOW_SPRITES;
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

}

void updatePlayer() {
    // Nothing yet
}
