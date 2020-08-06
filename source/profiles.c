#include "profiles.h"

static const uint8_t fnLayer1Keys[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, // ~ F1-F12
    16, 24, 25, 26, // Up, PS, HOME, END
    29. 30, 40, 47, 48, // Left, Down, right, PGUP, PGDN
    61, 62, // INSERT, DELETE
    75 // FN1
}

static const uint8_t fnLayer2Keys[] = {
    1, 2, 3, 4, 9, 10, 11, 12, // BLTH1-4 LEDON LEDOFF BRTNDOWN BRTNUP
    76 // FN2
}

static const uint32_t colorPalette[] = {
    0xFF0000, 0xFF7F00, 0xFFFF00, 0x7FFF00, 
    0x00FF00, 0x00FF7F, 0x00FFFF, 0x007FFF,
    0x0000FF, 0x7F00FF, 0xFF00FF, 0xFF007F,
};

#define LEN(a) (sizeof(a)/sizeof(*a))

void red(led_t* currentKeyLedColors){
    setAllKeysColor(currentKeyLedColors, 0x9c0000);
}

void miamiNights(led_t* currentKeyLedColors){
    setAllKeysColor(currentKeyLedColors, 0x00979c);
    setModKeysColor(currentKeyLedColors, 0x9c008f);
}

void rainbowHorizontal(led_t* currentKeyLedColors){
    for (uint8_t i=0; i<NUM_ROW; ++i){
        setRowKeysColor(currentKeyLedColors, i, colorPalette[i%LEN(colorPalette)]); 
    }
}

void rainbowVertical(led_t* currentKeyLedColors){
    for (uint8_t i=0; i<NUM_COLUMN; ++i){
        setColumnKeysColor(currentKeyLedColors, i, colorPalette[i%LEN(colorPalette)]);
    }
}

static uint8_t colAnimOffset = 0;
void animatedRainbow(led_t* currentKeyLedColors){
    for (uint8_t i=0; i<NUM_COLUMN; ++i){
        setColumnKeysColor(currentKeyLedColors, i, colorPalette[(i + colAnimOffset)%LEN(colorPalette)]);
    }
    colAnimOffset = (colAnimOffset + 1)%LEN(colorPalette);
}