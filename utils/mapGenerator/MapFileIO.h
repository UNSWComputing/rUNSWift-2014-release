// Class: MapFileIO
// Description: A class that handles
// reading and writing maps to a file

#ifndef MAP_FILE_IO_H
#define MAP_FILE_IO_H

#include "Map.h"
#include <string>

using namespace std;

class MapFileIO {
public:
    Map* readMapFile(string fileName);
    void writeMapFile(string fileName, Map* map);
};

#endif
