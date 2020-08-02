#include "profiles.h"

void red(led_t* currentKeyLedColors){
    setAllKeysColor(currentKeyLedColors, 0x9c0000);
}

static const uint32_t colorPalette[] = {
    0xFF0000, 0xFF7F00, 0xFFFF00, 0x7FFF00, 
    0x00FF00, 0x00FF7F, 0x00FFFF, 0x007FFF,
    0x0000FF, 0x7F00FF, 0xFF00FF, 0xFF007F,
};
#define LEN(a) (sizeof(a)/sizeof(*a))

void miamiNights(led_t* currentKeyLedColors){
    setAllKeysColor(currentKeyLedColors, 0x00979c);
    setModKeysColor(currentKeyLedColors, 0x9c008f);
}

void rainbowHorizontal(led_t* currentKeyLedColors){
    for (uint16_t i=0; i<NUM_ROW; ++i){
        for (uint16_t j=0; j<NUM_COLUMN; ++j){
            setKeyColor(&currentKeyLedColors[i*NUM_COLUMN+j], colorPalette[i%LEN(colorPalette)]);
        }     
    }
}

void rainbowVertical(led_t* currentKeyLedColors){
    for (uint16_t i=0; i<NUM_COLUMN; ++i){
        for (uint16_t j=0; j<NUM_ROW; ++j){
            setKeyColor(&currentKeyLedColors[j*NUM_COLUMN+i], colorPalette[i%LEN(colorPalette)]);
        }     
    }
}

static uint8_t colAnimOffset = 0;
void animatedRainbow(led_t* currentKeyLedColors){
    for (uint16_t i=0; i<NUM_COLUMN; ++i){
        for (uint16_t j=0; j<NUM_ROW; ++j){
            setKeyColor(&currentKeyLedColors[j*NUM_COLUMN+i], colorPalette[(i + colAnimOffset)%LEN(colorPalette)]);
        }     
    }
    colAnimOffset = (colAnimOffset + 1)%LEN(colorPalette);
}