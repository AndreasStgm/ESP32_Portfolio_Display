#include <Arduino.h>

#include "display_hal.h"
#include "storage_hal.h"

// ===== Definitions =====

#define READ_DIRECTORY "/"

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
Topic selectedtopic;
uint16_t topicLineCount = 0;

ScreenState currentScreenState = ScreenState::UPDATE;
DeviceState currentDeviceState = DeviceState::MAIN_SCREEN;

uint8_t currentScreenIndex = 0;

// ===== Function Declarations =====

// Handles the different states of the device and determines the updating of the screen
void stateHandler();

// Determines the action of the up and down buttons based on the device's state
void determineUpDownActionBasedOnDeviceState(ButtonPressed actionButton);

// Dynamically moves through the index and cycles it around when the up or down buttons are pressed
void moveThroughIndexAndCycle(ButtonPressed moveDirection, uint8_t maximum_index);

// Also moves through teh index, but limits it to the maximum value
void moveThroughIndexAndLimit(ButtonPressed moveDirection, uint8_t maximum_index);

// Display the different topics with an arrow pointing to the selected topic
void showTopicOptions(uint8_t selectedIndex, std::array<Topic, MAXIMUM_FILE_AMOUNT> topicsArray);

// Display the different topics
void showTopicDetails(uint8_t lineIndex, Topic selectedTopic);

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

    topicArray = assembleTopicsFromDirectory(SD, READ_DIRECTORY);
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
      showTopicDetails(currentScreenIndex, selectedtopic);
    }

    flushToDisplay();
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
          selectedtopic = topicArray[currentScreenIndex];
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
    moveThroughIndexAndCycle(actionButton, countAvailableTopics(topicArray) - 1);
  }
  else if (currentDeviceState == DeviceState::DETAILS_SCREEN)
  {
    moveThroughIndexAndLimit(actionButton, topicLineCount - DETAILS_LINE_AMOUNT);
  }
}

void moveThroughIndexAndCycle(ButtonPressed moveDirection, uint8_t maximum_index)
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

void moveThroughIndexAndLimit(ButtonPressed moveDirection, uint8_t maximum_index)
{
  if (moveDirection == ButtonPressed::DOWN_BUTTON)
  {
    if (currentScreenIndex < maximum_index)
    {
      currentScreenIndex++;
    }
  }
  else if (moveDirection == ButtonPressed::UP_BUTTON)
  {
    if (currentScreenIndex > 0)
    {
      currentScreenIndex--;
    }
  }
}

void showTopicOptions(uint8_t selectedIndex, std::array<Topic, MAXIMUM_FILE_AMOUNT> topicsArray)
{
  uint16_t divisionSize = (SCREEN_HEIGHT - MAIN_SCREEN_PADDING_SIZE * 2) / countAvailableTopics(topicsArray);

  setTextSize(2);

  // Display all the different topics available
  for (uint8_t i = 0; i < countAvailableTopics(topicsArray); i++)
  {
    setCursorLocation(MAIN_SCREEN_PADDING_SIZE, MAIN_SCREEN_PADDING_SIZE + (divisionSize * i + 8)); // Add the 8 to compensate for the text size
    displayPrintWithoutFlush(topicsArray[i].name, WHITE);
  }

  // Display the indicator
  setCursorLocation(MAIN_SCREEN_PADDING_SIZE / 2, MAIN_SCREEN_PADDING_SIZE + (divisionSize * selectedIndex + 8));
  displayPrintWithoutFlush(">", WHITE);
}

void showTopicDetails(uint8_t lineIndex, Topic selectedTopic)
{
  // Display the title of the topic
  setTextSize(2);
  setCursorLocation(DETAILS_SCREEN_PADDING_SIZE, DETAILS_SCREEN_PADDING_SIZE);
  displayPrintWithoutFlush(selectedTopic.name, WHITE);

  String fileContents = readFile(SD, READ_DIRECTORY + selectedtopic.textFileName);
  if (fileContents == "-1")
  {
    displayStatusMessage("Open File: ", "FAILED", RED);
  }
  else
  {
    // First count the amount of lines based on the
    topicLineCount = 0;
    for (unsigned int i = 0; i < fileContents.length(); i++)
    {
      if (fileContents[i] == '\n')
      {
        topicLineCount++;
      }
    }

    String splitLines[topicLineCount];
    for (uint16_t i = 0; i < topicLineCount; i++)
    {
      // Add the current line to the array
      splitLines[i] = fileContents.substring(0, fileContents.indexOf('\n'));
      // Cut the added line off from the remaining string
      fileContents = fileContents.substring(fileContents.indexOf('\n') + 1);
    }

    // Display the text of the topic, for a set amount of lines or for the max amount of lines available
    setTextSize(1);
    for (uint8_t i = 0; i < DETAILS_LINE_AMOUNT && i < topicLineCount; i++)
    {
      setCursorLocation(DETAILS_SCREEN_PADDING_SIZE, DETAILS_SCREEN_PADDING_SIZE + 29 + i * 8);
      displayPrintWithoutFlush(splitLines[currentScreenIndex + i], WHITE);
    }
  }
}