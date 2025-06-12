#include "display_hal.h"

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

// ===== Touch Panel Configuration =====

BBCapTouch touchPanel;

TouchState previousTouchState = TouchState::RELEASED;
TouchState currentTouchState = TouchState::RELEASED;

ButtonPressed whichButtonPressed = ButtonPressed::NONE;

unsigned long last_rise_time = 0;

bool singlePressFunctionCompleted = false;

// ===== Functions Implementations =====

void initializeDisplay(uint8_t initDisplayBrightness)
{
    // Start and clear the display
    if (!gfx->begin())
    {
        Serial.begin(9600);
        Serial.println("gfx->begin() failed!");
    }
    clearDisplay();

    // Setting up the LEDC and configuring the backlight pin
    ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
    ledcAttachPin(LCD_BL_PIN, LEDC_CHANNEL_0);
    setBrightness(initDisplayBrightness);
}

bool initializeTouchScreen()
{
    // Returns a 0 when succesfull, switched around to make a bit more sense
    if (touchPanel.init(TOUCH_SDA_PIN, TOUCH_SCL_PIN, TOUCH_RESET_PIN, TOUCH_INT_PIN))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void readTouchScreen()
{
    TOUCHINFO samplesInfo;
    // Being pressed
    if (touchPanel.getSamples(&samplesInfo))
    {
        // Check if the touch press is within the navigation on the right
        if (samplesInfo.x[0] > SCREEN_WIDTH - NAVIGATION_WIDTH)
        {
            // Check if touch press is within top button
            if (samplesInfo.y[0] < SCREEN_HEIGHT / 3)
            {
                whichButtonPressed = ButtonPressed::UP_BUTTON;
                currentTouchState = TouchState::PRESSED;
            }
            else if (samplesInfo.y[0] > SCREEN_HEIGHT / 3 * 2)
            {
                whichButtonPressed = ButtonPressed::DOWN_BUTTON;
                currentTouchState = TouchState::PRESSED;
            }
            else
            {
                whichButtonPressed = ButtonPressed::SELECT_BACK_BUTTON;
                currentTouchState = TouchState::PRESSED;
            }
        }
    }
    // Not pressed
    else
    {
        whichButtonPressed = ButtonPressed::NONE;
        currentTouchState = TouchState::RELEASED;
    }

    determineTouchPress();
}

void determineTouchPress()
{
    // Detect a long press of the buttons
    if (previousTouchState == TouchState::PRESSED && currentTouchState == TouchState::PRESSED && millis() - last_rise_time > TOUCH_PRESS_LONG_PRESS_DELAY)
    {
        // Only allow a long press for the up and down buttons
        if (whichButtonPressed == ButtonPressed::UP_BUTTON || whichButtonPressed == ButtonPressed::DOWN_BUTTON)
        {
            handleButtonActions();
        }
    }
    // Detect a rising edge of a button
    else if (previousTouchState == TouchState::RELEASED && currentTouchState == TouchState::PRESSED)
    {
        last_rise_time = millis();
        handleButtonActions();
    }

    previousTouchState = currentTouchState;
}

void handleButtonActions()
{
    switch (whichButtonPressed)
    {
    case ButtonPressed::UP_BUTTON:
        displayPrint("UP", WHITE);
        break;
    case ButtonPressed::DOWN_BUTTON:
        displayPrint("DOWN", WHITE);
        break;
    case ButtonPressed::SELECT_BACK_BUTTON:
        displayPrint("SL/BK", WHITE);
        break;

    default:
        break;
    }
}

void clearDisplay()
{
    gfx->fillScreen(BLACK);
    gfx->flush();
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

void displayDrawInterface(uint16_t interfaceColor, uint16_t buttonIconTextColor, String centerButtonText)
{
    clearDisplay();
    // Draw the interface itself, in a for loop to be adaptive with thickness
    for (uint8_t i = 0; i < INTERFACE_BORDER_WIDTH; i++)
    {
        // Draw the outline
        gfx->drawRect(i, i, SCREEN_WIDTH - (2 * i), SCREEN_HEIGHT - (2 * i), interfaceColor);
        // Draw the navigation outline on the right
        gfx->drawLine(SCREEN_WIDTH - NAVIGATION_WIDTH + i, i, SCREEN_WIDTH - NAVIGATION_WIDTH + i, SCREEN_HEIGHT, interfaceColor);
        // Draw the navigation divisions
        gfx->drawLine(SCREEN_WIDTH - NAVIGATION_WIDTH + i, SCREEN_HEIGHT / 3 + i, SCREEN_WIDTH, SCREEN_HEIGHT / 3 + i, interfaceColor);
        gfx->drawLine(SCREEN_WIDTH - NAVIGATION_WIDTH + i, SCREEN_HEIGHT / 3 * 2 + i, SCREEN_WIDTH, SCREEN_HEIGHT / 3 * 2 + i, interfaceColor);
    }
    // Print the text for the center button
    gfx->setTextColor(buttonIconTextColor);
    gfx->setTextSize(2);                                                                                                                // 2 -> 12x16 character size
    gfx->setCursor(SCREEN_WIDTH - NAVIGATION_WIDTH + ((NAVIGATION_WIDTH - centerButtonText.length() * 12) / 2), SCREEN_HEIGHT / 2 - 8); // Compensating and centering text based on size
    gfx->print(centerButtonText);
    // Draw the navigation buttons
    gfx->drawTriangle(SCREEN_WIDTH - NAVIGATION_WIDTH / 2, SCREEN_HEIGHT / 9, SCREEN_WIDTH - NAVIGATION_WIDTH / 3, SCREEN_HEIGHT / 9 * 2, SCREEN_WIDTH - NAVIGATION_WIDTH / 3 * 2, SCREEN_HEIGHT / 9 * 2, buttonIconTextColor);
    gfx->drawTriangle(SCREEN_WIDTH - NAVIGATION_WIDTH / 2, SCREEN_HEIGHT / 9 * 8, SCREEN_WIDTH - NAVIGATION_WIDTH / 3, SCREEN_HEIGHT / 9 * 7, SCREEN_WIDTH - NAVIGATION_WIDTH / 3 * 2, SCREEN_HEIGHT / 9 * 7, buttonIconTextColor);
    // Flush all graphics so they are displayed
    gfx->flush();
}