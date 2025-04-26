#include <Arduino.h>
#include <SD.h>

// ===== Storage Configuration Definitions =====

#define STORAGE_CS 10
#define STORAGE_MOSI 11
#define STORAGE_SCK 12
#define STORAGE_MISO 13

// ===== Function Definitions =====

// Start the SPI communication bus and initialize the SD card
bool initializeStorage();

// Check if the SD card is mounted correctly
bool checkIfSDMounted();

// If the SD card is mounted correctly, determine its type
String determineSDCardType();

// Show the total, used and free space of the SD card
std::array<uint64_t, 3> getSDCardStats();

// Read a file from path
void readFile(fs::FS &fs, const char *path);

void listDirAndAssembleTopics(fs::FS &fs, const char *dirname);