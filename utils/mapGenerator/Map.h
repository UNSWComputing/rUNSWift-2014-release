// Class: Map
// Description: A class to virtually 
// represent the RoboCup soccer field

#ifndef MAP_H
#define MAP_H

class Map {
private:
    int width; // corresponds to x
    int height; // corresponds to y
    int resolution; // related to map generation added for completeness
    int arraySize;
    int* mapArray;

public:
    Map(int width, int height, int resolution, int* mapArray);
    // int distanceToClosestLine(int x, int y);
    int getArraySize();
    int getHeight();
    int getWidth();
    int getResolution();
    int* getMapArray();
    void printMap();
    
};

#endif
