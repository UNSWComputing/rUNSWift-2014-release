// Class: MapFileHeader
// Description: MapFileHeader class implemetation

#include "MapFileHeader.h"
#include <fstream>

using namespace std;

MapFileHeader::MapFileHeader() {
    this->arraySize = 0;
    this->width = 0;
    this->height = 0;
    this->resolution = 0;
    this->headerLength = sizeof(this->arraySize)+sizeof(this->width)
                         +sizeof(this->height)+sizeof(this->resolution);
}

void MapFileHeader::readHeader(string fileName) {
    ifstream data;
    data.open(fileName.c_str(), ios::in | ios::binary); // File contains binary data
    if (data.is_open()) {
        // Read all header fields in
        data.read((char*)(&arraySize),sizeof(arraySize));
        data.read((char*)(&width),sizeof(width));
        data.read((char*)(&height),sizeof(height));
        data.read((char*)(&resolution),sizeof(resolution));
        data.close();
    }
}

void MapFileHeader::writeHeader(string fileName) {
    ofstream data;
    data.open(fileName.c_str(), ios::out | ios::binary | ios::trunc); // Truncate old file
    if (data.is_open()) {
        // Write all header fields out
        data.write((char*)(&arraySize),sizeof(arraySize));
        data.write((char*)(&width),sizeof(width));
        data.write((char*)(&height),sizeof(height));
        data.write((char*)(&resolution),sizeof(resolution));
        data.close();
    }
}

// Get the header length (in bytes)
// Note: Use this to seekg to map data
int MapFileHeader::getHeaderLength() {
    return this->headerLength;
}

// Get the size of the map in this file
int MapFileHeader::getMapArraySize() {
    return this->arraySize;
}

// Get the width of the map in this file
int MapFileHeader::getMapWidth() {
    return this->width;
}

// Get the height of the map in this file
int MapFileHeader::getMapHeight() {
    return this->height;
}

// Get the resolution of the map in this file
int MapFileHeader::getMapResolution() {
    return this->resolution;
}
    
// Set the size of the map for this file
void MapFileHeader::setHeaderValues(int arraySize, int width, 
                                    int height, int resolution) {
    this->arraySize = arraySize;
    this->width = width;
    this->height = height;
    this->resolution = resolution;
}
