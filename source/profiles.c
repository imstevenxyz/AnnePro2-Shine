#include "profiles.h"

void red(led_t* currentKeyLedColors){
    setAllKeysColor(currentKeyLedColors, 0x9c0000);
}

static const uint32_t colorPalette[] = {0x9c0000, 0x9c9900, 0x1f9c00, 0x00979c, 0x003e9c, 0x39009c, 0x9c008f};
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