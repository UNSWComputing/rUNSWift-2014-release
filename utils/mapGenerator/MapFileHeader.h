// Class: MapFileIO
// Description: A subclass that contains header
// information for the map file (e.g map size)

#ifndef MAP_FILE_HEADER_H
#define MAP_FILE_HEADER_H

#include <string>

class MapFileHeader {
    int arraySize;
    int width;
    int height;
    int resolution;
    int headerLength;

public:
    MapFileHeader(); 
    void readHeader(std::string fileName);
    void writeHeader(std::string fileName);
    int getHeaderLength();
    int getMapArraySize();
    int getMapWidth();
    int getMapHeight();
    int getMapResolution();
    void setHeaderValues(int arraySize, int width, int height, int resolution);
};

#endif
