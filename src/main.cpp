#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>

// ----- Struct Definitions -----
struct Topic
{
  String name;
  String textFileName;
};

// ----- Function Declarations -----
void readFile(fs::FS &fs, const char *path);
void listDirAndAssembleTopics(fs::FS &fs, const char *dirname, uint8_t levels);

// ----- Global Variable Declarations -----
const uint8_t topicArraySize = 5;

std::array<Topic, topicArraySize> topicArray;

// ----- Setup -----
void setup()
{
  Serial.begin(9600);
  // Start the SD library and attempt to mount the card
  if (!SD.begin(5))
  {
    Serial.println("Card Mount Failed");
    return;
  }

  // Check the SD card type to check if mounted correctly
  // If not, exists and execution stops here
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }
  // If mounted correctly, display card type
  Serial.print("SD Card Type: ");
  switch (cardType)
  {
  case CARD_MMC:
    Serial.println("MMC");
    break;
  case CARD_SDHC:
    Serial.println("SDHC");
    break;
  case CARD_SD:
    Serial.println("SDSC");
    break;

  default:
    Serial.println("UNKNOWN");
    break;
  }

  // Display the SD card's size, total space and used space
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

  topicArray = listDirAndAssembleTopics(SD, "/");
  // readFile(SD, "/international_project.txt");
}

void loop()
{
  delay(1);
}

// ----- Function Definitions -----
void readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

std::array<Topic, 5> listDirAndAssembleTopics(fs::FS &fs, const char *dirname)
{
  std::array<Topic, 5> resultArray;

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  uint8_t arrayPosition = 0;
  File file = root.openNextFile();
  while (file && arrayPosition < topicArraySize)
  {
    if (!file.isDirectory())
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());

      auto newTopic = Topic();
      newTopic.name = file.name(); // TODO: Change to actual name based on filename
      newTopic.textFileName = file.name();
      resultArray[arrayPosition] = newTopic;
      arrayPosition++;
    }
    file = root.openNextFile();
  }
}