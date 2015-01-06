#include "MapGenerator.h"
#include "Map.h"
#include "MapFileIO.h"

int main (int argc, char **argv) {
   string filename = "map.data";
   MapGenerator *mapGen = new MapGenerator();
   Map *map = mapGen->generateMap(10);
   map->printMap();
   MapFileIO *io = new MapFileIO();
   io->writeMapFile(filename, map);
   return 0;
}


