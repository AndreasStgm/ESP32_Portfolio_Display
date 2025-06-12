#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <bb_captouch.h>

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

// ===== Touch Configuration Definitions =====

#define TOUCH_INT_PIN 3
#define TOUCH_SCL_PIN 4
#define TOUCH_SDA_PIN 8
#define TOUCH_RESET_PIN 38

#define TOUCH_PRESS_DEBOUNCE_DELAY 50
#define TOUCH_PRESS_LONG_PRESS_DELAY 1000

// ===== Interface Definitions =====

#define INTERFACE_BORDER_WIDTH 3
#define NAVIGATION_WIDTH 90
#define PADDING_SIZE 45

// ===== Enum Definitions =====

enum class TouchState
{
    RELEASED,
    PRESSED
};

enum class ButtonPressed
{
    NONE,
    UP_BUTTON,
    SELECT_BACK_BUTTON,
    DOWN_BUTTON
};

// ===== Function Definitions =====

// Initializes the display and it's backlight at the specified value, sets screen to black and cursor at 0, 0 (top left)
void initializeDisplay(uint8_t initDisplayBrightness);

// Initializes the display touchscreen
bool initializeTouchScreen();

// Reads the touchscreen to see if it is being pressed
ButtonPressed readTouchScreen();

// Determines if a button was short- or long-pressed, and only returns when the press is correct
ButtonPressed determineTouchPress();

// Clears the display by filling it with black
void clearDisplay();

// To set the brightness of the display. Int of 0 - 255 as input
void setBrightness(uint8_t value);

// Set the cursor location in the screen
void setCursorLocation(uint16_t x, uint16_t y);

// Set the text size
void setTextSize(uint8_t size);

// To display a status message mainly for startup diagnostics
void displayStatusMessage(String message, String state, uint16_t stateColor);

// To print a string
void displayPrint(String text, uint16_t color);

// To print a string finishing with a newline
void displayPrintln(String text, uint16_t color);

// Draw the border of the application's interface
void displayDrawInterface(uint16_t interfaceColor, uint16_t centerButtonTextColor, String centerButtonText);
