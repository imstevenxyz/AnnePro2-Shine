/*
    ===  Lighting Utilities  ===
    This file contains functions useful for coding lighting profiles
*/
#include "light_utils.h"

#define LEN(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define BRT_INTERVAL 51 //1, 3, 5, 15, 17, 51, 85, 255
uint8_t brightness = 255;

static const uint8_t modKeyIDs[] = {0, 13, 14, 28, 40, 41, 42, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69};

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
    const uint8_t red = (color >> 16) & 0xFF;
    const uint8_t green = (color >> 8) & 0xFF;
    const uint8_t blue = color & 0xFF;
    
    for (uint16_t i=0; i<LEN(modKeyIDs); ++i){
        currentKeyLedColors[modKeyIDs[i]].red = red;
        currentKeyLedColors[modKeyIDs[i]].green = green;
        currentKeyLedColors[modKeyIDs[i]].blue = blue;
    }

}

/*
 * Set key to a color
 */
void setKeyColor(led_t *key, uint32_t color){
    key->red = (color >> 16) & 0xFF;
    key->green = (color >> 8) & 0xFF;
    key->blue = color & 0xFF;
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
 * Convert to RGB-255 with brightness modifier
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