#include "display_hal.h"

// ===== Enum Definitions =====

enum class TouchState
{
    RELEASED,
    PRESSED,
    LONG_PRESSED
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

ButtonPressed triggeredTouchZone = ButtonPressed::NONE;

TouchState currentTouchState = TouchState::RELEASED;
ButtonPressed whichButtonPressed = ButtonPressed::NONE;
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
                triggeredTouchZone = ButtonPressed::UP_BUTTON;
            }
            else if (samplesInfo.y[0] > SCREEN_HEIGHT / 3 * 2)
            {
                triggeredTouchZone = ButtonPressed::DOWN_BUTTON;
            }
            else
            {
                triggeredTouchZone = ButtonPressed::SELECT_BACK_BUTTON;
            }
        }
    }
    // Not pressed
    else
    {
        triggeredTouchZone = ButtonPressed::NONE;
    }
}

void determineTouchPress()
{
    // If no button press is detected, make sure that the states are set to an unpressed button
    if (triggeredTouchZone == ButtonPressed::NONE)
    {
        currentTouchState = TouchState::RELEASED;
        whichButtonPressed = ButtonPressed::NONE;
        singlePressFunctionCompleted = false;
    }
    else
    {
        static unsigned long last_cycle_edge_time = 0;
        unsigned long cycle_edge_time = millis();

        // To detect a rising edge
        if (currentTouchState == TouchState::RELEASED)
        {
            // If the time passed is longer than the debounce time
            if (cycle_edge_time - last_cycle_edge_time > TOUCH_PRESS_DEBOUNCE_DELAY)
            {
                currentTouchState = TouchState::PRESSED;
                whichButtonPressed = triggeredTouchZone;
            }
        }
        // To detect a button being held
        else if (currentTouchState == TouchState::PRESSED && (triggeredTouchZone == ButtonPressed::UP_BUTTON || triggeredTouchZone == ButtonPressed::DOWN_BUTTON))
        {
            // If the time passed is longer than the time that a long press takes
            if (cycle_edge_time - last_cycle_edge_time > TOUCH_PRESS_LONG_PRESS_DELAY)
            {
                currentTouchState = TouchState::LONG_PRESSED;
                whichButtonPressed = triggeredTouchZone;
            }
        }

        last_cycle_edge_time = cycle_edge_time;
    }
}

void handleTouchStates()
{
    if (currentTouchState == TouchState::PRESSED && singlePressFunctionCompleted == false)
    {
        handleButtonActions();
        singlePressFunctionCompleted = true;
    }
    else if (currentTouchState == TouchState::LONG_PRESSED)
    {
        handleButtonActions();
    }
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