// Class: Map
// Description: Map class implemetation

#include "Map.h"
#include <iostream>

// Gaussian Parameters
#define LINEAR 1
#define EXPONENTIAL 0
#define A 1
#define B 0
#define C 5 // bell width
#define SCALING_FACTOR 3

using namespace std;

Map::Map(int width, int height, int resolution, int* mapArray) {
    this->width = width;
    this->height = height;
    this->resolution = resolution;
    this->mapArray = mapArray;
    this->arraySize = width*height;
    // this->mapArrayD = NULL;
}

// Retrieve data array size
int Map::getArraySize() {
    return this->arraySize;
}

// Retrieve map width
int Map::getWidth() {
    return this->width;
}

// Retrieve map height
int Map::getHeight() {
    return this->height;
}

// Retrieve map resolution
int Map::getResolution() {
    return this->resolution;
}

// Retrieve map data array
int* Map::getMapArray() {
    return this->mapArray;
}

// Output map data array to stdout
void Map::printMap() {

    int r,c;
    cout << "Map Details: Width = " << this->width 
         << ", Height = " << this->height
         << ", Size = " << this->arraySize
         << ", Resolution = " << this->resolution << endl;
    for (r = 0; r < this->height ; r++) {
        for (c = 0; c < this->width ; c++) {
            if (this->mapArray[r*this->width+c] == -1) {
                cout << 'N' << " ";
            } else if (this->mapArray[r*this->width+c] > 9) {
                cout << 'O' << " ";
            } else {
                cout << this->mapArray[r*this->width+c] << " ";
            }
        }
        cout << endl;
    }

}


