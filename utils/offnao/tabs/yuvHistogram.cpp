#include <assert.h>
#include <limits.h>
#include <math.h>
#include <QPainter>

#include "yuvHistogram.hpp"
#include "classifier.hpp"

using namespace std;

// Constructor
YUVHistogram::YUVHistogram(unsigned int minimum, unsigned int maximum) {
    from = minimum;
    to = maximum;
    numberOfItems = 0;

    numberOfBuckets = maximum - minimum + 1;
    frequency.resize(numberOfBuckets);

    minCount = UINT_MAX;
    maxCount = 0U;
    minValue = to;
    maxValue = from;

    valuesValid = true;
    minCountValid = false;
    meanValid = false;
    stdevValid = false;
    entropyValid = false;
    maxEntropyValid = false;
}

// Constructor
YUVHistogram::YUVHistogram(unsigned int minimum, unsigned int maximum, unsigned int buckets) {
    from = minimum;
    to = maximum;
    numberOfItems = 0;
    numberOfBuckets = buckets;

    frequency.resize(numberOfBuckets);

    minCount = UINT_MAX;
    maxCount = 0U;
    minValue = to;
    maxValue = from;

    valuesValid = true;
    minCountValid = false;
    meanValid = false;
    stdevValid = false;
    entropyValid = false;
    maxEntropyValid = false;
}

// Add new data point to frequency count
void YUVHistogram::addDatapoint(unsigned int item) {
    assert(item >= from && item <= to);

    if (item >= from && item <= to) {
        if (item > frequency.size()) {
            frequency.resize(item);
        }
        frequency[item]++;
        numberOfItems++;
    } else {
        cerr << "YUVHistogram::addDatapoint(): " << item << " out of range\n";
    }

    // Update some variables to save on calculations
    //   it's pointless doing this unless the values are currently valid
    if (valuesValid) {
        if (maxCount < frequency[item]) {
            maxCount = frequency[item];
        }
        if (frequency[item] > 0 && item < minValue) {
            minValue = item;
        }
        if (frequency[item] > 0 && item > maxValue) {
            maxValue = item;
        }
    }
}

// Delete data point from frequency count
void YUVHistogram::deleteDatapoint(unsigned int item) {
    assert(item >= from && item <= to);
    assert(frequency[item] > 0);

    frequency[item]--;

    valuesValid = false; // Could be smarter here
    minCountValid = false;
    meanValid = false;
    stdevValid = false;
    entropyValid = false;
    maxEntropyValid = false;
}

// Get minimum possible value
unsigned int YUVHistogram::getMinimum(void) {
    return from;
}

// Get maximum possible value
unsigned int YUVHistogram::getMaximum(void) {
    return to;
}

// Get number of items counted
unsigned int YUVHistogram::getNumberOfItems(void) {
    return numberOfItems;
}

// Get smallest non-zero frequency count
unsigned int YUVHistogram::getMinCount(void) {
    if (!minCountValid) {
        minCount = UINT_MAX;

        for (vector<unsigned int>::const_iterator item = frequency.begin(); item != frequency.end(); ++item) {
            if (minCount > *item && *item != 0) {
                minCount = *item;
            }
        }
        minCountValid = true;
    }

    return minCount;
}

// Get largest frequency count
unsigned int YUVHistogram::getMaxCount(void) {
    if (!valuesValid) {

        maxCount = 0U;

        for (vector<unsigned int>::const_iterator item = frequency.begin(); item != frequency.end(); ++item) {
            if (maxCount < *item) {
                maxCount = *item;
            }
        }
        valuesValid = true;
    }

    return maxCount;
}

// Get smallest value for which frequency count is non-zero
unsigned int YUVHistogram::getMinValue(void) {
    if (!valuesValid) {

        minValue = to;

        for (unsigned int i = 0; i < frequency.size(); ++i) {
            if (frequency[i] > 0 && i < minValue) {
                minValue = i;
            }
        }
        valuesValid = true;
    }

    return minValue;
}

// Get largest value for which frequency count is non-zero
unsigned int YUVHistogram::getMaxValue(void) {
    if (!valuesValid) {

        maxValue = from;

        for (unsigned int i = 0; i < frequency.size(); ++i) {
            if (frequency[i] > 0 && i > maxValue) {
                maxValue = i;
            }
        }
        valuesValid = true;
    }

    return maxValue;
}

// Get average value of all data points
double YUVHistogram::getMeanValue(void) {
    if (!meanValid) {
        meanValue = 0.0;

        for (unsigned int i = 0; i < frequency.size(); ++i) {
            if (frequency[i] > 0) {
                //meanValue += (((double)(i * frequency[i]))/((double)numberOfItems));
                meanValue += (double)frequency[i]/(double)numberOfItems * (double)i;
            }
        }
        meanValid = true;
    }

    return meanValue;
}

// Get standard deviation of values of all data points
double YUVHistogram::getStdevValue(void) {
    if (!stdevValid) {
        stdevValue = 0.0;

        getMeanValue();
        for (unsigned int i = 0; i < frequency.size(); ++i) {
            if (frequency[i] > 0) {
                stdevValue += ((((double)i - meanValue) * ((double)i - meanValue))/(double)numberOfItems) * (double)frequency[i];
            }
        }
        stdevValue = sqrt(stdevValue);
        stdevValid = true;
    }

    return stdevValue;
}

// Get entropy of all data points
double YUVHistogram::getEntropyValue(void) {
    if (!entropyValid) {
        entropyValue = 0.0;

        for (unsigned int i = 0; i < frequency.size(); ++i) {
            if (frequency[i] > 0) {
                entropyValue -= ((double)frequency[i]/numberOfItems) * log((double)frequency[i]/numberOfItems);
            }
        }
        entropyValid = true;
    }

    return entropyValue;
}

// Get maximum possible entropy
double YUVHistogram::getMaxEntropyValue(void) {
    if (!maxEntropyValid) {
        maxEntropyValue = 0.0;

        for (unsigned int i = 0; i < to + 1; i++) {
            maxEntropyValue -= (1.0/(to + 1)) * log(1.0/(to + 1));
        }
        maxEntropyValid = true;
    }

    return maxEntropyValue;
}

// Draw histogram on pixmap
void YUVHistogram::drawHistogram(QPixmap &imagePixmap, unsigned int width, unsigned int height, enum ImageType colour) {
   unsigned int bucketSize = ceil((to - from + 1)/(double)width);
   unsigned int count = 0U;
   unsigned int freq[width];

   // Set painter
   QPainter imagePainter(&imagePixmap);
   imagePainter.setPen(Qt::black);
   imagePainter.setBrush(Qt::SolidPattern);
   //imagePainter.setCompositionMode(QPainter::CompositionMode_Xor);
   imagePainter.setRenderHint(QPainter::Antialiasing, true);

   // Create new frequency array scaled to width of pixmap
   for (unsigned int col = 0U; col < width && (col * bucketSize) <= to; ++col) {
      freq[col] = 0U;

      for (unsigned int bucket = 0U; bucket < bucketSize && ((col * bucketSize) + bucket) <= to; bucket++) {
         freq[col] += frequency[(col * bucketSize) + bucket];
      }
      if (freq[col] > count) {
         count = freq[col];
      }
   }
 
   // Draw histogram on pixmap
   unsigned int col;
   for (col = 0U; col < width && (col * bucketSize) <= to; ++col) {
      if (freq[col] > 0) {
         // Use middle colour of bucket to display histogram bar
         unsigned int colourValue =  (col * bucketSize) + bucketSize/2;
         // Use first colour of bucket to display histogram bar
         //unsigned int colourValue =  col * bucketSize;
         // Use last colour of bucket to display histogram bar
         //unsigned int colourValue =  (col * bucketSize) + (bucketSize - 1);

         switch(colour) {
         case eRGB:
            imagePainter.setPen(QColor((colourValue & 0x00ff0000) >> 16,
                                       (colourValue & 0x0000ff00) >> 8,
                                       (colourValue & 0x000000ff)));
            break;
         case eYUV:
            colourValue = Classifier::yuv2rgb((colourValue & 0x00ff0000) >> 16,
                                              (colourValue & 0x0000ff00) >> 8,
                                              (colourValue & 0x000000ff));
            imagePainter.setPen(QColor((colourValue & 0x00ff0000) >> 16,
                                       (colourValue & 0x0000ff00) >> 8,
                                       (colourValue & 0x000000ff)));
            break;
         case eRED:
            imagePainter.setPen(QColor(colourValue, 0, 0));
            break;
         case eGREEN:
            imagePainter.setPen(QColor(0, colourValue, 0));
            break;
         case eBLUE:
            imagePainter.setPen(QColor(0, 0, colourValue));
            break;
         case eHSV:
         case eBW:
            break;
         }

         // Draw bar on histogram
         imagePainter.drawLine(QPoint(col, height - 1),
           QPoint(col, (height - 1) - ((double)freq[col]/(double)count*height)));
      }
   }

   // Fill in remainder of pixmap (if any) with lightGray (what's the correct colour here?)
   imagePainter.setPen(Qt::lightGray);
   for (; col < width; ++col) {
      imagePainter.drawLine(QPoint(col, height - 1), QPoint(col, 0));
   }
}

// Print statistics
void YUVHistogram::printStatistics(ostringstream &ost) {

    ost << "STATISTICS\n";
    ost << dec << " Number of Items =\t" << (int)getNumberOfItems() << "\n";
    ost << hex << " Min Value =\t0x" << getMinValue() << "\t/\t0x" << getMaximum() << "\t (" << getMinValue()/getMaximum() * 100.0 << "%)\n";
    ost << hex << " Max Value =\t0x" << getMaxValue() << "\t/\t0x" << getMaximum() << "\t (" << getMaxValue()/getMaximum() * 100.0 << "%)\n";
    ost << dec << " Min Count =\t" << getMinCount() << "\t/\t" << getNumberOfItems() << "\t (" << (float)getMinCount()/getNumberOfItems() * 100.0 << "%)\n";
    ost << dec << " Max Count =\t" << getMaxCount() << "\t/\t" << getNumberOfItems() << "\t (" << (float)getMaxCount()/getNumberOfItems() * 100.0 << "%)\n";
    ost << hex << " Avg Value =\t0x" << (int)getMeanValue() << "\t/\t0x" << getMaximum() <<  "\t (" << getMeanValue()/getMaximum() * 100.0 << "%)\n";
    ost << dec << " Stdev =\t" << getStdevValue() << "\t/\t" << getMaximum() <<  "\t (" << getStdevValue()/getMaximum()*100.0 << "%)\n";
    ost << dec << " Entropy =\t" << getEntropyValue() << "\t/\t" << getMaxEntropyValue() << "\t (" << getEntropyValue()/getMaxEntropyValue() * 100.0 << "%)\n";
    //cout << ost.str();
}
