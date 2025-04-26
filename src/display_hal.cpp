#include "display_hal.h"

// ===== Display Driver and Panel Configuration =====

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    DISPLAY_CS_PIN,
    DISPLAY_SCK_PIN,
    DISPLAY_D0_PIN,
    DISPLAY_D1_PIN,
    DISPLAY_D2_PIN,
    DISPLAY_D3_PIN);
Arduino_NV3041A *panel = new Arduino_NV3041A(
    bus,
    GFX_NOT_DEFINED,
    DISPLAY_ROTATION,
    DISPLAY_IS_IPS);
Arduino_GFX *gfx = new Arduino_Canvas(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    panel);

// ===== Functions Implementations =====

void initializeDisplay(uint8_t initDisplayBrightness)
{
    // Start and clear the display
    if (!gfx->begin())
    {
        Serial.begin(9600);
        Serial.println("gfx->begin() failed!");
    }
    gfx->fillScreen(BLACK);
    gfx->flush();

    // Setting up the LEDC and configuring the backlight pin
    ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
    ledcAttachPin(LCD_BL_PIN, LEDC_CHANNEL_0);
    setBrightness(initDisplayBrightness);
}

void setBrightness(uint8_t value)
{
    uint32_t duty = 4095 * value / 255;
    ledcWrite(LEDC_CHANNEL_0, duty);
}

void displayStatusMessage(String message, String state, uint16_t stateColor)
{
    displayPrint(message, WHITE);
    displayPrintln(state, stateColor);
}

void displayPrint(String text, uint16_t color)
{
    gfx->setTextColor(color);
    gfx->print(text);
    gfx->flush();
}

void displayPrintln(String text, uint16_t color)
{
    gfx->setTextColor(color);
    gfx->println(text);
    gfx->flush();
}