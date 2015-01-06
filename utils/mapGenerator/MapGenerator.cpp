#include "MapGenerator.h"

#define OFFSET 0

// Generate a map based by comparing the given
// resolution to the map's absolute dimensions (in mm)
// Note: resolution must be a multiple of 5
Map* MapGenerator::generateMap(int resolution) {

    int r, c;

    // Adjust dimensions according to resolution
    this->height = ABS_HEIGHT/resolution;
    this->width = ABS_WIDTH/resolution;
    this->border = ABS_BORDER/resolution;
    this->centre_circle_radius = ABS_CENTRE_CIRCLE_RADIUS/resolution;
    this->line_width = ABS_LINE_WIDTH/resolution;
    this->point_width = ABS_POINT_WIDTH/resolution;
    this->point_height = ABS_POINT_HEIGHT/resolution;
    this->point_position = ABS_POINT_POSITION/resolution;
    this->box_width = ABS_BOX_WIDTH/resolution;
    this->box_height = ABS_BOX_HEIGHT/resolution;
    this->stride = width+2*border;
    this->true_height = height+border*2;

    // Allocate and initialise map data array
    this->handle = new int[stride*true_height];
    this->map = handle+border*stride+border;
    for (r = 0; r < true_height ; r++) {
        for (c = 0; c < stride ; c++) {
            handle[r*stride+c] = 0;
        }
    }

    // Now draw lines...
    int rmin = 0, rmax = 0, cmin = 0, cmax = 0; // Ranges

    // Top Field Line
    rmin = 0; rmax = line_width;
    cmin = 0; cmax = width;
    drawLine(rmin,rmax,cmin,cmax);

    // Bottom Field Line
    rmin = height-line_width; rmax = height;
    cmin = 0; cmax = width;
    drawLine(rmin,rmax,cmin,cmax);

    // Left Field Line
    rmin = 0; rmax = height;
    cmin = 0; cmax = line_width;
    drawLine(rmin,rmax,cmin,cmax);

    // Right Field Line
    rmin = 0; rmax = height;
    cmin = width-line_width; cmax = width;
    drawLine(rmin,rmax,cmin,cmax);

    // Draw Centre Field Line
    rmin = 0; rmax = height;
    cmin = width/2-line_width/2 + OFFSET; cmax = width/2+line_width/2 + OFFSET;
    drawLine(rmin,rmax,cmin,cmax);

    int rc = height/2, cc = width/2 + OFFSET, radius = centre_circle_radius;
    // Place each quadrant on a corner of square
    drawQuadrant(rc-1,cc-1,centre_circle_radius,0);
    drawQuadrant(rc-1,cc,centre_circle_radius,1);
    drawQuadrant(rc,cc-1,centre_circle_radius,2);
    drawQuadrant(rc,cc,centre_circle_radius,3);

    // Draw Left Point
    rmin = height/2-point_height/2; rmax = height/2+point_height/2;
    cmin = point_position-point_height/2; cmax = point_position+point_height/2;
    int blank_strip = (point_height-point_width)/2;
    drawPoint(rmin,rmax,cmin,cmax,blank_strip);

    // Draw Right Point
    rmin = height/2-point_height/2; rmax = height/2+point_height/2;
    cmin = width-point_position-point_height/2; cmax = width-point_position+point_height/2;
    blank_strip = (point_height-point_width)/2;
    drawPoint(rmin,rmax,cmin,cmax,blank_strip);

    // Draw Centre Point
    rmin = height/2-point_height/2; rmax = height/2+point_height/2;
    cmin = width/2-point_height/2 + OFFSET; cmax = width/2+point_height/2 + OFFSET;
    drawPoint(rmin,rmax,cmin,cmax,blank_strip);

    // Left Box: Draw Top Line
    rmin = (height-box_height)/2; rmax = (height-box_height)/2+line_width;
    cmin = 0; cmax = box_width;
    drawLine(rmin,rmax,cmin,cmax);

    // Left Box: Draw Bottom Line
    rmin = height-(height-box_height)/2-line_width; rmax = height-(height-box_height)/2;
    cmin = 0; cmax = box_width;
    drawLine(rmin,rmax,cmin,cmax);

    // Left Box: Draw Right Line
    rmin = (height-box_height)/2; rmax = height-(height-box_height)/2;
    cmin = box_width-line_width; cmax = box_width;
    drawLine(rmin,rmax,cmin,cmax);

    // Right Box: Draw Top Line
    rmin = (height-box_height)/2; rmax = (height-box_height)/2+line_width;
    cmin = width-box_width; cmax = width;
    drawLine(rmin,rmax,cmin,cmax);

    // Right Box: Draw Bottom Line
    rmin = height-(height-box_height)/2-line_width; rmax = height-(height-box_height)/2;
    cmin = width-box_width; cmax = width;
    drawLine(rmin,rmax,cmin,cmax);

    // Right Box: Draw Left Line
    rmin = (height-box_height)/2; rmax = height-(height-box_height)/2;
    cmin = width-box_width; cmax = width-box_width+line_width;
    drawLine(rmin,rmax,cmin,cmax);

    // Now create new data array to place line distances...
    int* distance_map = new int[stride*true_height];
    for (r = 0; r < true_height ; r++) {
        for (c = 0; c < stride ; c++) {
            distance_map[r*stride+c] = -1;
        }
    }

    // Update distances...
    for (r = 0; r < true_height ; r++) {
        for (c = 0; c < stride ; c++) {
            if( handle[r*stride+c] == INTERESTING) {
                // FieldEdge point found, update all relevant distances
                updateDistances(distance_map,r,c);
            }
        }
    }


    // Using map data array, create, cleanup and return map
    // Note: Map width is really stride (similarly adjusted height)
    //Map* generated_map = new Map(stride,true_height,resolution,handle);
    Map* generated_map = new Map(stride,true_height,resolution,distance_map);
    delete[] handle;
    return generated_map;
}

// (r,c) is point that lies on a line, now update distances
// to all other points in the up, down, left, right direction
// (r,c) is point that lies on a line, now update distances
// to all other points in the up, down, left, right direction
void MapGenerator::updateDistances(int* distance_map, int r, int c) {
    int current_distance, rn, cn;

    // Update Leftwards
    current_distance = 0;
    for (cn = c ; cn >= 0 ; cn--, current_distance++) {
        if ( handle[r*stride+cn] == 1 && cn != c) { // cn != c to avoid breaking on original point
            // We have reached another white line
            // so we need not look further
            break;
        } 
        if (   distance_map[r*stride+cn] == -1
            || distance_map[r*stride+cn] > current_distance) {
            // Either there is no distance available or we have
            // found a distance longer than from (r,c) so update
            distance_map[r*stride+cn] = current_distance;
        }

    }

    // Update Rightwards
    current_distance = 0;
    for (cn = c ; cn < stride ; cn++, current_distance++) {
        if ( handle[r*stride+cn] == 1 && cn != c ) {
            // We have reached another white line
            // so we need not look further
            break;
        } 
        if (   distance_map[r*stride+cn] == -1
            || distance_map[r*stride+cn] > current_distance) {
            // Either there is no distance available or we have
            // found a distance longer than from (r,c) so update
            distance_map[r*stride+cn] = current_distance;
        }

    }

    // Update Upwards
    current_distance = 0;
    for (rn = r ; rn >= 0 ; rn--, current_distance++) {
        if ( handle[rn*stride+c] == 1 && rn != r ) {
            // We have reached another white line
            // so we need not look further
            break;
        } 
        if (   distance_map[rn*stride+c] == -1
            || distance_map[rn*stride+c] > current_distance) {
            // Either there is no distance available or we have
            // found a distance longer than from (r,c) so update
            distance_map[rn*stride+c] = current_distance;
        }

    }

    // Update Downwards
    current_distance = 0;
    for (rn = r ; rn < true_height ; rn++, current_distance++) {
        if ( handle[rn*stride+c] == 1 && rn != r ) {
            // We have reached another white line
            // so we need not look further
            break;
        } 
        if (   distance_map[rn*stride+c] == -1
            || distance_map[rn*stride+c] > current_distance) {
            // Either there is no distance available or we have
            // found a distance longer than from (r,c) so update
            distance_map[rn*stride+c] = current_distance;
        }

    }
}

// Return absolute value of x
int MapGenerator::abs(int x) {
    if ( x < 0 ) {
        return -x;
    }
    return x;
}

// Draw a "line" in the rectangle defined 
// between r_s->r_f and c_s -> c_f
void MapGenerator::drawLine(int r_s, int r_f, int c_s, int c_f) {
    int r = 0, c = 0;
    for (r = r_s; r < r_f ; r++) {
        for (c = c_s; c < c_f ; c++) {
            map[r*stride+c] = INTERESTING;
        }
    }
}

// Draw one quadrant of a circle that would
// be roughly centred at (r_c,c_c)
void MapGenerator::drawQuadrant(int r_c, int c_c, int radius, int quadrant) {

    int rmin = 0, rmax = 0;
    int cmin = 0, cmax = 0;
    if (quadrant == FIRST_QUADRANT) {
        rmin = r_c-radius, rmax = r_c;
        cmin = c_c-radius, cmax = c_c;
    } else if (quadrant == SECOND_QUADRANT) {
        rmin = r_c-radius, rmax = r_c;
        cmin = c_c, cmax = c_c+radius;
    } else if (quadrant == THIRD_QUADRANT) {
        rmin = r_c, rmax = r_c+radius;
        cmin = c_c-radius, cmax = c_c;
    } else if (quadrant == FOURTH_QUADRANT) {
        rmin = r_c, rmax = r_c+radius;;
        cmin = c_c, cmax = c_c+radius;;
    } 

    int r, c, r_e, c_e;
    for (r = rmin, r_e=abs(r-r_c); r <= rmax ; r++, r_e=abs(r-r_c)) {
        for (c = cmin, c_e=abs(c-c_c); c <= cmax ; c++, c_e=abs(c-c_c)) {
            if (   r_e*r_e + c_e*c_e <= radius*radius
                && r_e*r_e + c_e*c_e > (radius-line_width)*(radius-line_width)) {
                map[r*stride+c] = INTERESTING;
            }
        }
    }
}

// Draw a point...
void MapGenerator::drawPoint(int r_s, int r_f, int c_s, int c_f, int blank_strip) {
    int r, c;
    for (r = r_s; r < r_f ; r++) {
        for (c = c_s; c < c_f ; c++) {
            if (   r >= r_s+blank_strip && r < r_f-blank_strip 
                || c >= c_s+blank_strip && c < c_f-blank_strip ) {
                map[r*stride+c] = INTERESTING;
            }
        }
    }
}
