#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ===== Display Configuration Definitions =====

#define DISPLAY_CS_PIN 45
#define DISPLAY_SCK_PIN 47
#define DISPLAY_D0_PIN 21
#define DISPLAY_D1_PIN 48
#define DISPLAY_D2_PIN 40
#define DISPLAY_D3_PIN 39

#define DISPLAY_ROTATION 0
#define DISPLAY_IS_IPS true

// ===== Display Size Definitions =====

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272
#define SCR_BUF_LEN 32

// ===== Display Backlight Definitions =====

#define LCD_BL_PIN 1
#define LEDC_CHANNEL_0 0     // use first channel of 16 channels (started from zero)
#define LEDC_TIMER_12_BIT 12 // use 12 bit precission for LEDC timer
#define LEDC_BASE_FREQ 5000  // use 5000 Hz as a LEDC base frequency

// ===== Function Definitions =====

// Initializes the display and it's backlight at the specified value, sets screen to black and cursor at 0, 0 (top left)
void initializeDisplay(uint8_t initDisplayBrightness);

// To set the brightness of the display. Int of 0 - 255 as input
void setBrightness(uint8_t value);

// To display a status message mainly for startup diagnostics
void displayStatusMessage(String message, String state, uint16_t stateColor);

// To print a string
void displayPrint(String text, uint16_t color);

// To print a string finishing with a newline
void displayPrintln(String text, uint16_t color);