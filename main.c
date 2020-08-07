#include "board.h"
#include "hal.h"
#include "ch.h"
#include "string.h"
#include "ap2_qmk_led.h"
#include "light_utils.h"
#include "profiles.h"

/*
 * Function declarations
 */
static void columnCallback(GPTDriver* driver);
static void animationCallback(GPTDriver* driver);
inline void sPWM(uint8_t cycle, uint8_t currentCount, uint8_t start, ioline_t port);
void executeMsg(msg_t msg);
void switchProfile(void);
void executeProfile(void);
void disableLeds(void);
void ledSet(void);
void ledRowSet(void);

#define LED_ACTIVE_ON_START TRUE
#define REFRESH_FREQUENCY   200
#define ANIMATION_TIMER_FREQUENCY   60

/*
 * Active profiles
 * Add profiles from source/profiles.h in the profile array
 */
typedef void (*profile)( led_t* );
profile profiles[5] = {miamiNights, rainbowHorizontal, rainbowVertical, red, animatedRainbow};
static uint8_t currentProfile = 0;
static uint8_t amountOfProfiles = sizeof(profiles)/sizeof(profile);

static bool bootStatus = true;

led_t currentKeyLedColors[NUM_COLUMN * NUM_ROW];
ioline_t ledColumns[NUM_COLUMN] = {
    LINE_LED_COL_1, 
    LINE_LED_COL_2, 
    LINE_LED_COL_3, 
    LINE_LED_COL_4, 
    LINE_LED_COL_5, 
    LINE_LED_COL_6, 
    LINE_LED_COL_7, 
    LINE_LED_COL_8, 
    LINE_LED_COL_9, 
    LINE_LED_COL_10,
    LINE_LED_COL_11,
    LINE_LED_COL_12,
    LINE_LED_COL_13,
    LINE_LED_COL_14
};

ioline_t ledRows[NUM_ROW * 3] = {
    LINE_LED_ROW_1_R,
    LINE_LED_ROW_1_G,
    LINE_LED_ROW_1_B,
    LINE_LED_ROW_2_R,
    LINE_LED_ROW_2_G,
    LINE_LED_ROW_2_B,
    LINE_LED_ROW_3_R,
    LINE_LED_ROW_3_G,
    LINE_LED_ROW_3_B,
    LINE_LED_ROW_4_R,
    LINE_LED_ROW_4_G,
    LINE_LED_ROW_4_B,
    LINE_LED_ROW_5_R,
    LINE_LED_ROW_5_G,
    LINE_LED_ROW_5_B,
};

/*
 * Serial configuration
 */
static const SerialConfig usart1Config = {
    .speed = 115200
};

/*
 * Column multiplex configuration
 */
static const GPTConfig bftm0Config = {
    .frequency = NUM_COLUMN * REFRESH_FREQUENCY * 2 * 16,
    .callback = columnCallback
};

/*
 * Animation configuration
 */
static const GPTConfig lightAnimationConfig = {
    .frequency = ANIMATION_TIMER_FREQUENCY,
    .callback = animationCallback
};

/*
 * Thread 1.
 */
THD_WORKING_AREA(waThread1, 128);
THD_FUNCTION(Thread1, arg) {
    (void)arg;
     
    while(true){
        msg_t msg;
        msg = sdGet(&SD1);
        if(msg >= MSG_OK){
            executeMsg(msg);
        }
    }
}

/*
 * Execute action based on a message
 */
void executeMsg(msg_t msg){
    switch (msg) {
        case CMD_LED_ON:
            switchProfile();
            break;
        case CMD_LED_OFF:
            disableLeds();
            break;
        case CMD_LED_BRT_DOWN:
            downBrightness();
            executeProfile();
            break;
        case CMD_LED_BRT_UP:
            upBrightness();
            executeProfile();
            break;
        case CMD_POST_INIT:
            bootStatus = false;
            if(LED_ACTIVE_ON_START) executeProfile();
            break;
        case CMD_LED_SET:
            ledSet();
            break;
        default:
            break;
    }
}

/*
 * Switch to next profile and execute it
 */
void switchProfile(){
    currentProfile = (currentProfile+1)%amountOfProfiles;
    executeProfile();
}

/*
 * Execute current profile
 */
void executeProfile(){
    chSysLock();
    profiles[currentProfile](currentKeyLedColors);
    palSetLine(LINE_LED_PWR);
    chSysUnlock();
}

/*
 * Turn off all leds
 */
void disableLeds(){
    currentProfile = (currentProfile+amountOfProfiles-1)%amountOfProfiles;
    palClearLine(LINE_LED_PWR);
}

static uint8_t commandBuffer[64];

void ledSet(){
    uint8_t setMsg = sdGet(&SD1);
    switch (setMsg){
        case SET_KEY:
            break;
        case SET_COL:
            break;
        case SET_ROW:
            break;
        case SET_FN1:
            break;
        case SET_FN2:
            break;
        case SET_MOD:
            break;
        default:
            break;
    }

    size_t bytesRead;
    bytesRead = sdReadTimeout(&SD1, commandBuffer, 5, 10000);
    if(bytesRead == 5){
        if(commandBuffer[0] < NUM_ROW || commandBuffer[1] < NUM_COLUMN){
            uint32_t color = ((uint32_t)commandBuffer[4] << 16 | (uint16_t)commandBuffer[3] << 8 | (uint8_t)commandBuffer[2]);
            setKeyColor(&currentKeyLedColors[commandBuffer[0] * NUM_COLUMN + commandBuffer[1]], color);
        }
    }else{
        executeProfile();
    }
}

void ledRowSet(){
    size_t bytesRead;
    bytesRead = sdReadTimeout(&SD1, commandBuffer, sizeof(uint16_t) * NUM_COLUMN + 1, 1000);
    if(bytesRead >= sizeof(uint16_t) * NUM_COLUMN + 1){
        if(commandBuffer[0] < NUM_ROW){
            memcpy(&currentKeyLedColors[commandBuffer[0] * NUM_COLUMN],&commandBuffer[1], sizeof(uint16_t) * NUM_COLUMN);
        }
    }
}

static uint32_t currentColumn = 0;
static uint32_t columnPWMCount = 0;

void columnCallback(GPTDriver* _driver){
    (void)_driver;
    palClearLine(ledColumns[currentColumn]);
    currentColumn = (currentColumn+1) % NUM_COLUMN;
    palSetLine(ledColumns[currentColumn]);

    if(columnPWMCount < 255){
        for (size_t row = 0; row < NUM_ROW; row++){
            const led_t keyLED = currentKeyLedColors[currentColumn + (NUM_COLUMN * row)];
            const uint8_t red = keyLED.red;
            const uint8_t green = keyLED.green;
            const uint8_t blue = keyLED.blue;

            sPWM(red, columnPWMCount, 0, ledRows[row * 3]);
            sPWM(green, columnPWMCount, red, ledRows[row * 3+1]);
            sPWM(blue, columnPWMCount, red+green, ledRows[row * 3+2]);
        }
        columnPWMCount++;
    }else{
        columnPWMCount = 0;
    }
}

inline void sPWM(uint8_t cycle, uint8_t currentCount, uint8_t start, ioline_t port){
    if (start+cycle>0xFF) start = 0xFF - cycle;
    if (start <= currentCount && currentCount < start+cycle){
        palSetLine(port);
    }else{
        palClearLine(port);
    }
}

void animationCallback(GPTDriver* _driver){
    if(bootStatus){
        gptChangeInterval(_driver, ANIMATION_TIMER_FREQUENCY/4);
        animatedBoot(currentKeyLedColors);
        return;
    }

    profile currentFunction = profiles[currentProfile];
    if(currentFunction == animatedRainbow){
        gptChangeInterval(_driver, ANIMATION_TIMER_FREQUENCY/7);
        currentFunction(currentKeyLedColors);
    }
}

/*
 * Application entry point.
 * HAL initialization
 * Setup timers
 * Create Thread 1
 */
int main(void){
    halInit();
    chSysInit();

    sdStart(&SD1, &usart1Config);
    palSetLine(LINE_LED_PWR);

    gptStart(&GPTD_BFTM0, &bftm0Config);
    gptStartContinuous(&GPTD_BFTM0, 1);

    gptStart(&GPTD_BFTM1, &lightAnimationConfig);
    gptStartContinuous(&GPTD_BFTM1, 1);

    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    while(true){
        // Low priority thread
    }
}