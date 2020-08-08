#ifndef LIGHT_UTILS_H
#define LIGHT_UTILS_H

#include "board.h"
#include "hal.h"

/*
    Structs
*/
typedef struct {
    uint8_t red, green, blue;
} led_t;

/*
    Function Signatures
*/
void setAllKeysColor(led_t* currentKeyLedColors, uint32_t color);
void setModKeysColor(led_t* currentKeyLedColors, uint32_t color);
void setFN1KeysColor(led_t* currentKeyLedColors, uint32_t color);
void setFN2KeysColor(led_t* currentKeyLedColors, uint32_t color);
void setColumnKeysColor(led_t* currentKeyLedColors, uint8_t column, uint32_t color);
void setRowKeysColor(led_t* currentKeyLedColors, uint8_t row, uint32_t color);
void setKeyColor(led_t *key, uint32_t color);
void upBrightness(void);
void downBrightness(void);
led_t rgbWithBrtModifier(uint32_t color);

#endif