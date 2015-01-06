// Class: MapFileIO
// Description: MapFileIO class implemetation

#include "MapFileIO.h"
#include "MapFileHeader.h"
#include <fstream>

// Read in a map from a file written to by writeMapFile
Map* MapFileIO::readMapFile(string fileName) {
    // Read header
    MapFileHeader header;
    header.readHeader(fileName);

    ifstream data;
    data.open(fileName.c_str(), ios::in | ios::binary); // File contains binary data
    if (data.is_open()) {
        data.seekg(header.getHeaderLength(),ios::beg); // Seek to just after header

        // Create appropriately sized array and read in data
        int arraySize = header.getMapArraySize();
        int* mapElements = new int[arraySize];
        for (int i = 0; i < arraySize; i++) {
            data.read((char*)(&mapElements[i]),sizeof(mapElements[i]));
        }
        data.close();

        // Now create a map object with this data
        Map* map = new Map(header.getMapWidth(),header.getMapHeight(),
            header.getMapResolution(),mapElements);
        return map;
    }
    // File read failed
    return NULL;
}

// Write a map to file as binary data
void MapFileIO::writeMapFile(string fileName, Map* map) {
    // Create and write header
    MapFileHeader header;
    header.setHeaderValues(map->getArraySize(),map->getWidth(),
                           map->getHeight(),map->getResolution());
    header.writeHeader(fileName);

    ofstream data;
    data.open(fileName.c_str(), ios::in | ios::binary | ios::app); // Append as header binary data has been added
    if (data.is_open()) {
        // Write out data contained in the map
        int* mapElements = map->getMapArray();
        int arraySize = map->getArraySize();
        for (int i = 0; i < arraySize; i++) {
            data.write((char*)(&mapElements[i]),sizeof(mapElements[i]));
        }
        data.close();
    }
}
