#include <Arduino.h>

#include "display_hal.h"
#include "storage_hal.h"

// ===== Global Variables =====
std::array<Topic, MAXIMUM_FILE_AMOUNT> topicArray;

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
    for (uint8_t i = 0; i < MAXIMUM_FILE_AMOUNT; i++)
    {
      if (!topicArray[i].textFileName.isEmpty())
      {
        displayPrintln("  " + topicArray[i].textFileName, CYAN);
      }
    }
  }
  else
  {
    displayPrintln("No SD Card Attached!", WHITE);
  }

  delay(2500);
  displayDrawInterface(MAGENTA, WHITE, "Select");
}

// ===== Loop =====

void loop()
{
  readTouchScreen();

  delay(1);
}