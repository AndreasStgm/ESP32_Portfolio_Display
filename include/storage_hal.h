#include <Arduino.h>
#include <SD.h>

// ===== Storage Configuration Definitions =====

#define STORAGE_CS 10
#define STORAGE_MOSI 11
#define STORAGE_SCK 12
#define STORAGE_MISO 13

// ===== Global Constant =====
const uint8_t MAXIMUM_FILE_AMOUNT = 5;

// ===== Struct Definitions =====

struct Topic
{
    String name;
    String textFileName;
};

// ===== Function Definitions =====

// Start the SPI communication bus and initialize the SD card
bool initializeStorage();

// Check if the SD card is mounted correctly
bool checkIfSDMounted();

// If the SD card is mounted correctly, determine its type
String determineSDCardType();

// Show the total, used and free space of the SD card
std::array<uint64_t, 3> getSDCardStats();

// Assemble the different topics from the specified directory, there is a maximum amount and they must be TXT files
std::array<Topic, MAXIMUM_FILE_AMOUNT> assembleTopicsFromDirectory(fs::FS &fs, const char *dirname);

// Count the amount of topics that are actually filled in in the array
uint8_t countAvailableTopics(std::array<Topic, MAXIMUM_FILE_AMOUNT> topicsArray);

// Read a file from path
String readFile(fs::FS &fs, String path);
