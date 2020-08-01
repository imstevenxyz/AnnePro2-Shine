#include "board.h"
#include "hal.h"
#include "ch.h"
#include "ap2_qmk_led.h"
#include "light_utils.h"
#include "profiles.h"

/*
 * Function declarations
 */
static void columnCallback(GPTDriver* driver);
// static void animationCallback(GPTDriver* driver);
inline void sPWM(uint8_t cycle, uint8_t currentCount, uint8_t start, ioline_t port);
void executeMsg(msg_t msg);
void disableLeds(void);
void ledSet(void);
void ledRowSet(void);

#define REFRESH_FREQUENCY   200

/*
 * Serial configuration
 */
static const SerialConfig usart1Config = {
    .speed = 115200
};

/*
 * Column multiplex configurations
 */
static const GPTConfig bftm0Config = {
    .frequency = NUM_COLUMN * REFRESH_FREQUENCY * 2 * 16,
    .callback = columnCallback
};

led_t currentKeyLedColors[70];
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
 * Thread 1.
 */
THD_WORKING_AREA(waThread1, 128);
THD_FUNCTION(Thread1, arg) {
    (void)arg;
    while (true){
        msg_t msg;
        msg = sdGet(&SD1);
        if (msg >= MSG_OK) {
            executeMsg(msg);
        }
    }
}

void executeMsg(msg_t msg){
    switch (msg) {
        case CMD_LED_ON:
            chSysLock();
            miamiNights(currentKeyLedColors);
            palSetLine(LINE_LED_PWR);
            chSysUnlock();
            break;
        case CMD_LED_OFF:
            disableLeds();
            break;
        case CMD_LED_SET:
            ledSet();
            break;
        case CMD_LED_SET_ROW:
            ledRowSet();
            break;
        default:
            break;
    }
}

void disableLeds(){
    palClearLine(LINE_LED_PWR);
}

#include "string.h"
static uint8_t commandBuffer[64];

void ledSet(){
    size_t bytesRead;
    bytesRead = sdReadTimeout(&SD1, commandBuffer, 4, 10000);
    if (bytesRead >= 4){
        if (commandBuffer[0] < NUM_ROW || commandBuffer[1] < NUM_COLUMN){
            setKeyColor(&currentKeyLedColors[commandBuffer[0] * NUM_COLUMN + commandBuffer[1]], ((uint16_t)commandBuffer[3] << 8 | commandBuffer[2]));
        }
    }
}

void ledRowSet(){
    size_t bytesRead;
    bytesRead = sdReadTimeout(&SD1, commandBuffer, sizeof(uint16_t) * NUM_COLUMN + 1, 1000);
    if (bytesRead >= sizeof(uint16_t) * NUM_COLUMN + 1){
        if (commandBuffer[0] < NUM_ROW){
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

    if (columnPWMCount < 255){
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

/*
 * Application entry point.
 * HAL initialization en setup
 * Create Thread 1
 */
int main(void) {
    halInit();
    chSysInit();

    sdStart(&SD1, &usart1Config);
    palSetLine(LINE_LED_PWR);

    // Setup Column Multiplex Timer
    gptStart(&GPTD_BFTM0, &bftm0Config);
    gptStartContinuous(&GPTD_BFTM0, 1);

    // Setup Animation Timer
    // gptStart(&GPTD_BFTM1, &lightAnimationConfig);
    // gptStartContinuous(&GPTD_BFTM1, 1);

    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    while (true) {
        // Low priority thread
    }
}