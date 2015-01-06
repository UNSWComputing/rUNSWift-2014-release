#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <QObject>

/*
 * YUVHistogram class
 */

class YUVHistogram {
    std::vector<unsigned int> frequency;
    unsigned int from;
    unsigned int to;
    unsigned int numberOfBuckets;
    unsigned int numberOfItems;

    unsigned int minValue;
    unsigned int maxValue;
    unsigned int minCount;
    unsigned int maxCount;
    unsigned int getModeValue(void);
    unsigned int getMedianValue(void);
    double meanValue;
    double stdevValue;
    double entropyValue;
    double maxEntropyValue;

    bool valuesValid;
    bool minCountValid;
    bool meanValid;
    bool stdevValid;
    bool entropyValid;
    bool maxEntropyValid;

public:
    enum ImageType {eBW, eRGB, eYUV, eHSV, eRED, eGREEN, eBLUE};

    YUVHistogram(unsigned int minimum, unsigned int maximum);
    YUVHistogram(unsigned int minimum, unsigned int maximum, unsigned int buckets);

    void addDatapoint(unsigned int item);
    void deleteDatapoint(unsigned int item);

    unsigned int getMinimum(void);
    unsigned int getMaximum(void);
    unsigned int getNumberOfItems(void);
    unsigned int getMinCount(void);
    unsigned int getMaxCount(void);
    unsigned int getMinValue(void);
    unsigned int getMaxValue(void);
    double getMeanValue(void);
    double getStdevValue(void);
    double getEntropyValue(void);
    double getMaxEntropyValue(void);
    void drawHistogram(QPixmap &imagePixmap, unsigned int width, unsigned int height, enum ImageType colour =eBW);
    void printStatistics(std::ostringstream &ost);
};

