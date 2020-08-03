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

void upBrightness(){
    if(brightness != 255) brightness = brightness + BRT_INTERVAL;
}

void downBrightness(){
    if(brightness != 0) brightness = brightness - BRT_INTERVAL;
}

led_t rgbWithBrtModifier(uint32_t color){
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

/*
hsb_t hexToHsb(uint32_t colorHex){
    hsb_t hsb;
    const double r = (double)((colorHex >> 16) & 0xFF)/255;
    const double g = (double)((colorHex >> 8) & 0xFF)/255;
    const double b = (double)(colorHex & 0xFF)/255;

    double cmax = max(r, b, g);
    double cmin = min(r, b, g);
    double diff = cmax - cmin;

    if(cmax == r){
        hsb.hue = 60 * (dmod((g-b)/diff,6));
    }else if(cmax == g){
        hsb.hue = 60 * (((b-r)/diff)+2);
    }else if(cmax == b){
        hsb.hue = 60 * (((r-g)/diff)+4);
    }

    if(cmax == 0){
        hsb.saturation = 0;
    }else{
        hsb.saturation = (diff / cmax) * 100;
    }

    hsb.brightness = cmax * 100;
    return hsb;
}

#include <math.h>

double dmod(double x, double y) {
    return x - (int)(x/y) * y;
}

double max(double a, double b, double c) {
   return ((a > b)? (a > c ? a : c) : (b > c ? b : c));
}
double min(double a, double b, double c) {
   return ((a < b)? (a < c ? a : c) : (b < c ? b : c));
}

led_t hsbToRgb(hsb_t hsb)
{
    double c = 0.0, m = 0.0, x = 0.0;
    led_t rgb;

    c = hsb.brightness * hsb.saturation;
    x = c * (1.0 - fabs(dmod(hsb.hue / 60.0, 2) - 1.0));
    m = hsb.brightness - c;
    if (hsb.hue >= 0.0 && hsb.hue < 60.0)
    {
        rgb.red = (uint8_t)(c+m);
        rgb.green = (uint8_t)(x+m);
        rgb.blue = (uint8_t)(m);
    }
    else if (hsb.hue >= 60.0 && hsb.hue < 120.0)
    {
        rgb.red = (uint8_t)(x+m);
        rgb.green = (uint8_t)(c+m);
        rgb.blue = (uint8_t)(m);
    }
    else if (hsb.hue >= 120.0 && hsb.hue < 180.0)
    {
        rgb.red = (uint8_t)(m);
        rgb.green = (uint8_t)(c+m);
        rgb.blue = (uint8_t)(x+m);
    }
    else if (hsb.hue >= 180.0 && hsb.hue < 240.0)
    {
        rgb.red = (uint8_t)(m);
        rgb.green = (uint8_t)(x+m);
        rgb.blue = (uint8_t)(c+m);
    }
    else if (hsb.hue >= 240.0 && hsb.hue < 300.0)
    {
        rgb.red = (uint8_t)(x+m);
        rgb.green = (uint8_t)(m);
        rgb.blue = (uint8_t)(c+m);
    }
    else if (hsb.hue >= 300.0 && hsb.hue < 360.0)
    {
        rgb.red = (uint8_t)(c+m);
        rgb.green = (uint8_t)(m);
        rgb.blue = (uint8_t)(x+m);
    }
    else
    {
        rgb.red = (uint8_t)(m);
        rgb.green = (uint8_t)(m);
        rgb.blue = (uint8_t)(m);
    }

    rgb.red = rgb.red * 255;
    rgb.blue = rgb.blue * 255;
    rgb.green = rgb.green * 255;

    return rgb;
}
    
led_t hsbToRgb(hsb_t in)
{
    double      hh, p, q, t, ff;
    long        i;
    led_t         out;

    if(in.saturation <= 0.0) {       // < is bogus, just shuts up warnings
        out.red = in.brightness*255;
        out.green = in.brightness*255;
        out.blue = in.brightness*255;
        return out;
    }
    hh = in.hue;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.brightness * (1.0 - in.saturation);
    q = in.brightness * (1.0 - (in.saturation * ff));
    t = in.brightness * (1.0 - (in.saturation * (1.0 - ff)));

    switch(i) {
    case 0:
        out.red = in.brightness * 255;
        out.green = t;
        out.blue = p;
        break;
    case 1:
        out.red = q * 255;
        out.green = in.brightness * 255;
        out.blue = p * 255;
        break;
    case 2:
        out.red = p * 255;
        out.green = in.brightness * 255;
        out.blue = t * 255;
        break;
    case 3:
        out.red = p * 255;
        out.green = q * 255;
        out.blue = in.brightness * 255;
        break;
    case 4:
        out.red = t * 255;
        out.green = p * 255;
        out.blue = in.brightness * 255;
        break;
    case 5:
    default:
        out.red = in.brightness * 255;
        out.green = p * 255;
        out.blue = q * 255;
        break;
    }
    return out;     
}

*/