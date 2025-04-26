#include "storage_hal.h"

// ===== Storage Configuration =====

static SPIClass SPIStorage(HSPI);

// ===== Functions Implementations =====

bool initializeStorage()
{
    // Initialize the SPI communication bus
    SPIStorage.begin(STORAGE_SCK, STORAGE_MISO, STORAGE_MOSI, STORAGE_CS);

    if (!SD.begin(STORAGE_CS, SPIStorage))
    {
        return false;
    }
    return true;
}

bool checkIfSDMounted()
{
    // Check the SD card type to check if mounted correctly
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        return false;
    }
    return true;
}

String determineSDCardType()
{
    // Determine the mounted SD card's type
    uint8_t cardType = SD.cardType();

    switch (cardType)
    {
    case CARD_MMC:
        return "MMC";
    case CARD_SDHC:
        return "SDHC";
    case CARD_SD:
        return "SDSC";
    default:
        return "UNKNOWN";
    }
}

std::array<uint64_t, 3> getSDCardStats()
{
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    uint64_t totalSpace = SD.totalBytes() / (1024 * 1024);
    uint64_t usedSpace = SD.usedBytes() / (1024 * 1024);

    return {cardSize, totalSpace, usedSpace};
}

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

void listDirAndAssembleTopics(fs::FS &fs, const char *dirname)
{
    // std::array<Topic, 5> resultArray;

    // File root = fs.open(dirname);
    // if (!root)
    // {
    //     Serial.println("Failed to open directory");
    //     return resultArray;
    // }
    // if (!root.isDirectory())
    // {
    //     Serial.println("Not a directory");
    //     return resultArray;
    // }

    // uint8_t arrayPosition = 0;
    // File file = root.openNextFile();
    // while (file && arrayPosition < topicArraySize)
    // {
    //     if (!file.isDirectory())
    //     {
    //         Serial.print("  FILE: ");
    //         Serial.print(file.name());
    //         Serial.print("  SIZE: ");
    //         Serial.println(file.size());

    //         auto newTopic = Topic();
    //         newTopic.name = file.name(); // TODO: Change to actual name based on filename
    //         newTopic.textFileName = file.name();
    //         resultArray[arrayPosition] = newTopic;
    //         arrayPosition++;
    //     }
    //     file = root.openNextFile();
    // }
}