// Class: MapGenerator
// Description: A class to the construct
// a virtual RoboCup map with all nece-
// ssary field elements and lines

#ifndef MAP_GEN_H
#define MAP_GEN_H

#include "Map.h"
#include "../../robot/utils/SPLDefs.hpp"

// Map Absolute Dimension Constants (mm)
// Note: height -> rows, width -> columns


#define ABS_HEIGHT FIELD_WIDTH
#define ABS_WIDTH FIELD_LENGTH
#define ABS_BORDER FIELD_LENGTH_OFFSET
#define ABS_CENTRE_CIRCLE_RADIUS (CENTER_CIRCLE_DIAMETER / 2)
#define ABS_LINE_WIDTH FIELD_LINE_WIDTH
#define ABS_POINT_WIDTH (MARKER_DIMENSIONS / 2)
#define ABS_POINT_HEIGHT MARKER_DIMENSIONS
#define ABS_POINT_POSITION BACK_LINE_TO_FURTHEST_MARKER_EDGE // Distance from edge
#define ABS_BOX_WIDTH GOAL_BOX_LENGTH
#define ABS_BOX_HEIGHT GOAL_BOX_WIDTH


/*
#define ABS_HEIGHT 30
#define ABS_WIDTH 60
#define ABS_BORDER 5
#define ABS_CENTRE_CIRCLE_RADIUS 8
#define ABS_LINE_WIDTH 2
#define ABS_POINT_WIDTH 2
#define ABS_POINT_HEIGHT 4
#define ABS_POINT_POSITION 15 // Distance from edge
#define ABS_BOX_WIDTH 6
#define ABS_BOX_HEIGHT 20
*/

// Other Constants
#define INTERESTING 1
#define FIRST_QUADRANT 0
#define SECOND_QUADRANT 1
#define THIRD_QUADRANT 2
#define FOURTH_QUADRANT 3

class MapGenerator {
    int* handle;
    int* map;
    int height;
    int width;
    int border;
    int centre_circle_radius;
    int line_width;
    int point_width;
    int point_height;
    int point_position;
    int box_width;
    int box_height;
    int stride;
    int true_height;
public:
    Map* generateMap(int resolution);
private:
    int abs(int x);
    void drawLine(int r_s, int r_f, int c_s, int c_f);
    void drawCircle(int r_c, int c_c, int radius);
    void drawQuadrant(int r_c, int c_c, int radius, int quadrant);
    void drawPoint(int r_s, int r_f, int c_s, int c_f, int blank_strip);
    void updateDistances(int* distance_map, int r, int c);
};

#endif
