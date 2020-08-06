/*
    ===  Lighting Utilities  ===
    This file contains functions useful for coding lighting profiles
*/
#include "light_utils.h"

#define LEN(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define BRT_INTERVAL 51 //1, 3, 5, 15, 17, 51, 85, 255
uint8_t brightness = 204;

static const uint8_t modKeyIDs[] = {0, 13, 14, 28, 40, 41, 42, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69};
static const uint8_t fnLayer1Keys[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, // ~ F1-F12
    16, 24, 25, 26, // Up, PS, HOME, END
    29. 30, 40, 47, 48, // Left, Down, right, PGUP, PGDN
    61, 62, // INSERT, DELETE
    75 // FN1
};
static const uint8_t fnLayer2Keys[] = {
    1, 2, 3, 4, 9, 10, 11, 12, // BLTH1-4 LEDON LEDOFF BRTNDOWN BRTNUP
    76 // FN2
};


/*
 * Set all keys to the same color
 */
void setAllKeysColor(led_t* currentKeyLedColors, uint32_t color){
    led_t rgb = rgbWithBrtModifier(color);

    for (uint16_t i=0; i<NUM_COLUMN * NUM_ROW; ++i){
        currentKeyLedColors[i].red = rgb.red;
        currentKeyLedColors[i].green = rgb.green;
        currentKeyLedColors[i].blue = rgb.blue;
    }
}

/*
 * Set all modifier keys to the same color
 */
void setModKeysColor(led_t* currentKeyLedColors, uint32_t color){
    led_t rgb = rgbWithBrtModifier(color);
    
    for (uint16_t i=0; i<LEN(modKeyIDs); ++i){
        currentKeyLedColors[modKeyIDs[i]].red = rgb.red;
        currentKeyLedColors[modKeyIDs[i]].green = rgb.green;
        currentKeyLedColors[modKeyIDs[i]].blue = rgb.blue;
    }
}

/*
 * Set all FN1 keys to the same color
 */
void setFN1KeysColor(led_t* currentKeyLedColors, uint32_t color){
    led_t rgb = rgbWithBrtModifier(color);
    
    for (uint16_t i=0; i<LEN(fnLayer1Keys); ++i){
        currentKeyLedColors[fnLayer1Keys[i]].red = rgb.red;
        currentKeyLedColors[fnLayer1Keys[i]].green = rgb.green;
        currentKeyLedColors[fnLayer1Keys[i]].blue = rgb.blue;
    }
}

/*
 * Set all FN2 keys to the same color
 */
void setFN1KeysColor(led_t* currentKeyLedColors, uint32_t color){
    led_t rgb = rgbWithBrtModifier(color);
    
    for (uint16_t i=0; i<LEN(fnLayer2Keys); ++i){
        currentKeyLedColors[fnLayer2Keys[i]].red = rgb.red;
        currentKeyLedColors[fnLayer2Keys[i]].green = rgb.green;
        currentKeyLedColors[fnLayer2Keys[i]].blue = rgb.blue;
    }
}

/*
 * Set all keys from a column to the same color
 */
void setColumnKeysColor(led_t* currentKeyLedColors, uint8_t column, uint32_t color){
    led_t rgb = rgbWithBrtModifier(color);

    for (uint8_t i=0; i<NUM_ROW; i++){
        setKeyColor(&currentKeyLedColors[i * NUM_COLUMN + column], color);
    }
}

/*
 * Set all keys from a row to the same color
 */
void setRowKeysColor(led_t* currentKeyLedColors, uint8_t row, uint32_t color){
    led_t rgb = rgbWithBrtModifier(color);

    for (uint8_t i=0; i<NUM_COLUMN; i++){
        setKeyColor(&currentKeyLedColors[row * NUM_COLUMN + i], color);
    }
}

/*
 * Set key to a color
 */
void setKeyColor(led_t *key, uint32_t color){
    led_t rgb = rgbWithBrtModifier(color);
    key->red = rgb.red;
    key->green = rgb.green;
    key->blue = rgb.blue;
}

/*
 * Up the brightness value
 */
void upBrightness(){
    if(brightness != 255) brightness = brightness + BRT_INTERVAL;
}

/*
 * Down the brightness value
 */
void downBrightness(){
    if(brightness != 0) brightness = brightness - BRT_INTERVAL;
}

/*
 * Convert HEX color to RGB-255 with the brightness modifier
 */
inline led_t rgbWithBrtModifier(uint32_t color){
    const uint8_t red = (color >> 16) & 0xFF;
    const uint8_t green = (color >> 8) & 0xFF;
    const uint8_t blue = color & 0xFF;
    double mod = (double)(brightness) / 255;

    led_t led;
    led.red = red * mod;
    led.green = green * mod;
    led.blue = blue * mod;

    return led;
}
