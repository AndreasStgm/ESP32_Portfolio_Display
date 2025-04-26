#include <Arduino.h>

#include "display_hal.h"
#include "storage_hal.h"

// ===== Struct Definitions =====

struct Topic
{
  String name;
  String textFileName;
};

// ===== Global Variable Declarations =====
// const uint8_t topicArraySize = 5;
// std::array<Topic, topicArraySize> topicArray;

// ===== Setup =====

void setup()
{
  // Initialize the display
  initializeDisplay(250);
  displayStatusMessage("Initialize Display: ", "OK", GREEN);

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
  }
  else
  {
    displayPrintln("No SD Card Attached!", WHITE);
  }

  // topicArray = listDirAndAssembleTopics(SD, "/");
  // readFile(SD, "/international_project.txt");
}

// ===== Loop =====

void loop()
{
  delay(1);
}