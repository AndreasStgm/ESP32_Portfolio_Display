#include <Arduino.h>

#include "display_hal.h"
#include "storage_hal.h"

// ===== Enum Definitions =====

enum class ScreenState
{
  WAITING,
  UPDATE
};
enum class DeviceState
{
  MAIN_SCREEN,
  DETAILS_SCREEN
};

// ===== Global Variables =====

std::array<Topic, MAXIMUM_FILE_AMOUNT> topicArray;

ScreenState currentScreenState = ScreenState::UPDATE;
DeviceState currentDeviceState = DeviceState::MAIN_SCREEN;

uint8_t currentScreenIndex = 0;

// ===== Function Declarations =====

// Handles the different states of the device and determines the updating of the screen
void stateHandler();

// Determines the action of the up and down buttons based on the device's state
void determineUpDownActionBasedOnDeviceState(ButtonPressed actionButton);

// Dynamicall moves through the index and cycles it around when the up or down buttons are pressed
void moveThroughIndex(ButtonPressed moveDirection, uint8_t maximum_index);

// Display the different topics with an arrow pointing to the selected topic
void showTopicOptions(uint8_t selectedIndex, std::array<Topic, MAXIMUM_FILE_AMOUNT> topicsArray);

// Display the different topics
void showTopicDetails(Topic selectedTopic);

// ===== Setup =====

void setup()
{
  // Initialize the display
  initializeDisplay(250);
  displayStatusMessage("Initialize Display: ", "OK", GREEN);

  // Initialize the display touch screen
  if (initializeTouchScreen())
  {
    displayStatusMessage("Initialize Touchscreen: ", "OK", GREEN);
  }
  else
  {
    displayStatusMessage("Initialize Touchscreen: ", "FAILED", RED);
  }

  // Initialize the SD card
  if (initializeStorage())
  {
    displayStatusMessage("SD Card Mount: ", "OK", GREEN);
  }
  else
  {
    displayStatusMessage("SD Card Mount: ", "FAILED", RED);
  }

  if (checkIfSDMounted())
  {
    displayStatusMessage("SD Card Type: ", determineSDCardType(), CYAN);

    std::array<uint64_t, 3> stats = getSDCardStats();
    displayStatusMessage("SD Card Size: ", (String)stats[0] + "MB", CYAN);
    displayStatusMessage("SD Card Total Space: ", (String)stats[1] + "MB", CYAN);
    displayStatusMessage("SD Card Used Space: ", (String)stats[2] + "MB", CYAN);

    topicArray = assembleTopicsFromDirectory(SD, "/");
    if (!topicArray[0].textFileName.isEmpty())
    {
      displayStatusMessage("TXT File Read: ", "OK", GREEN);
    }
    else
    {
      displayStatusMessage("TXT File Read: ", "NONE FOUND", RED);
    }
    displayPrintln("TXT Files found: ", WHITE);
    for (uint8_t i = 0; i < countAvailableTopics(topicArray); i++)
    {
      displayPrintln("  " + topicArray[i].textFileName, CYAN);
    }
  }
  else
  {
    displayPrintln("No SD Card Attached!", WHITE);
  }

  // Added delay so all info can be read
  delay(2500);
}

// ===== Loop =====

void loop()
{
  stateHandler();
  delay(50);
}

// ===== Function Definitions =====

void stateHandler()
{
  if (currentScreenState == ScreenState::UPDATE)
  {
    if (currentDeviceState == DeviceState::MAIN_SCREEN)
    {
      displayDrawInterface(MAGENTA, WHITE, "Select");
      showTopicOptions(currentScreenIndex, topicArray);
    }
    else if (currentDeviceState == DeviceState::DETAILS_SCREEN)
    {
      displayDrawInterface(MAGENTA, WHITE, "Back");
      // showTopicDetails(topic)
    }

    currentScreenState = ScreenState::WAITING;
  }
  else if (currentScreenState == ScreenState::WAITING)
  {
    ButtonPressed resultButton = readTouchScreen();
    if (resultButton != ButtonPressed::NONE)
    {
      if (resultButton == ButtonPressed::SELECT_BACK_BUTTON)
      {
        if (currentDeviceState == DeviceState::MAIN_SCREEN)
        {
          currentDeviceState = DeviceState::DETAILS_SCREEN;
        }
        else if (currentDeviceState == DeviceState::DETAILS_SCREEN)
        {
          currentDeviceState = DeviceState::MAIN_SCREEN;
        }
        currentScreenIndex = 0;
      }
      else
      {
        determineUpDownActionBasedOnDeviceState(resultButton);
      }

      currentScreenState = ScreenState::UPDATE;
    }
  }
}

void determineUpDownActionBasedOnDeviceState(ButtonPressed actionButton)
{
  if (currentDeviceState == DeviceState::MAIN_SCREEN)
  {
    moveThroughIndex(actionButton, countAvailableTopics(topicArray) - 1);
  }
  else if (currentDeviceState == DeviceState::DETAILS_SCREEN)
  {
    moveThroughIndex(actionButton, 0);
  }
}

void moveThroughIndex(ButtonPressed moveDirection, uint8_t maximum_index)
{
  if (moveDirection == ButtonPressed::DOWN_BUTTON)
  {
    if (currentScreenIndex >= maximum_index)
    {
      currentScreenIndex = 0;
    }
    else
    {
      currentScreenIndex++;
    }
  }
  else if (moveDirection == ButtonPressed::UP_BUTTON)
  {
    if (currentScreenIndex <= 0)
    {
      currentScreenIndex = maximum_index;
    }
    else
    {
      currentScreenIndex--;
    }
  }
}

void showTopicOptions(uint8_t selectedIndex, std::array<Topic, MAXIMUM_FILE_AMOUNT> topicsArray)
{
  uint16_t divisionSize = (SCREEN_HEIGHT - PADDING_SIZE * 2) / countAvailableTopics(topicsArray);
  setTextSize(2);

  // Display all the different topics available
  for (uint8_t i = 0; i < countAvailableTopics(topicsArray); i++)
  {
    setCursorLocation(PADDING_SIZE, PADDING_SIZE + (divisionSize * i + 8)); // Add the 8 to compensate for the text size
    displayPrint(topicsArray[i].name, WHITE);
  }

  // Display the indicator
  setCursorLocation(PADDING_SIZE / 2, PADDING_SIZE + (divisionSize * selectedIndex + 8));
  displayPrint(">", WHITE);
}

void showTopicDetails(Topic selectedTopic)
{
}