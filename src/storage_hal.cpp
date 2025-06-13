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

std::array<Topic, MAXIMUM_FILE_AMOUNT> assembleTopicsFromDirectory(fs::FS &fs, const char *dirname)
{
    File root = fs.open(dirname);
    // if (!root) // TODO: fix by passing an error to the main and filling the array through a pointer
    // {
    //     displayStatusMessage("Open Directory: ", "FAILED", RED);
    // }
    // if (!root.isDirectory())
    // {
    //     displayPrintln("Specified path is not a directory!", WHITE);
    // }

    std::array<Topic, MAXIMUM_FILE_AMOUNT> resultArray;
    uint8_t arrayPosition = 0;

    File file = root.openNextFile();
    while (file)
    {
        if (!file.isDirectory())
        {
            String stringName = (String)file.name();
            if (stringName.endsWith(".txt"))
            {
                auto newTopic = Topic();
                newTopic.name = stringName.substring(0, stringName.indexOf(".txt"));
                newTopic.textFileName = file.name();
                resultArray[arrayPosition] = newTopic;
                arrayPosition++;
            }
        }
        file = root.openNextFile();
    }
    return resultArray;
}

uint8_t countAvailableTopics(std::array<Topic, MAXIMUM_FILE_AMOUNT> topicsArray)
{
    // If the array is not completely filled, return the found index where it is empty, this is then the count of filled objects
    for (uint8_t i = 0; i < MAXIMUM_FILE_AMOUNT; i++)
    {
        if (topicsArray[i].textFileName.isEmpty())
        {
            return i;
        }
    }

    return MAXIMUM_FILE_AMOUNT;
}

String readFile(fs::FS &fs, String path)
{
    // Open up the file, and if the file does not want to open, return -1 to indicate error
    File file = fs.open(path);
    if (!file)
    {
        return "-1";
    }
    // Read the actual file contents
    String completeFileContents = file.readString();
    file.close();

    return completeFileContents;
}
