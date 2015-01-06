#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Suites
#include <iostream>
#include <boost/test/unit_test.hpp>

#include "utils/Histogram.hpp"

BOOST_AUTO_TEST_SUITE(vision_histogram)

#define DATA_SIZE 8
int data1    [DATA_SIZE] = { 1,  2,  6,  3,  0,  1,  2,  3};

int exp11win  [DATA_SIZE] = { 1,  2,  6,  3,  0,  1,  2,  3};
int exp11peak [DATA_SIZE] = { 2, -1,  0,  0,  0,  0,  0,  0};
int exp11winc [DATA_SIZE] = { 1,  2,  6,  3,  0,  1,  2,  3};
int exp11peakc[DATA_SIZE] = { 2,  7, -1,  0,  0,  0,  0,  0};

int exp12win  [DATA_SIZE] = { 0,  3,  8,  9,  3,  1,  3,  5};
int exp12peak [DATA_SIZE] = { 3, -1,  0,  0,  0,  0,  0,  0};
int exp12winc [DATA_SIZE] = { 4,  3,  8,  9,  3,  1,  3,  5};
int exp12peakc[DATA_SIZE] = { 3,  7, -1,  0,  0,  0,  0,  0};

int exp13win  [DATA_SIZE] = { 0,  9, 11,  9,  4,  3,  6,  0};
int exp13peak [DATA_SIZE] = { 2, -1,  0,  0,  0,  0,  0,  0};
int exp13winc [DATA_SIZE] = { 6,  9, 11,  9,  4,  3,  6,  6};
int exp13peakc[DATA_SIZE] = { 2, -1,  0,  0,  0,  0,  0,  0};

int exp14win  [DATA_SIZE] = { 0,  0, 12, 11, 10,  6,  6,  0};
int exp14peak [DATA_SIZE] = {-1,  0,  0,  0,  0,  0,  0,  0};
int exp14winc [DATA_SIZE] = { 8, 12, 12, 11, 10,  6,  6,  7};
int exp14peakc[DATA_SIZE] = { 2, -1,  0,  0,  0,  0,  0,  0};

int data2     [DATA_SIZE] = { 2,  1,  0,  0,  0,  0,  0,  1};

int exp23win  [DATA_SIZE] = { 0,  3,  1,  0,  0,  0,  1,  0};
int exp23peak [DATA_SIZE] = {-1,  0,  0,  0,  0,  0,  0,  0};
int exp23winc [DATA_SIZE] = { 4,  3,  1,  0,  0,  0,  1,  3};
int exp23peakc[DATA_SIZE] = { 0, -1,  0,  0,  0,  0,  0,  0};

/* The uniform cases */
int data3     [DATA_SIZE] = { 1,  0,  0,  0,  1,  0,  0,  0};
int data4     [DATA_SIZE] = { 0,  0,  0,  1,  0,  0,  0,  1};

int exp33win  [DATA_SIZE] = { 0,  1,  0,  1,  1,  1,  0,  0};
int exp33peak [DATA_SIZE] = { 4, -1,  0,  0,  0,  0,  0,  0};
int exp33winc [DATA_SIZE] = { 1,  1,  0,  1,  1,  1,  0,  1};
int exp33peakc[DATA_SIZE] = { 0,  4, -1,  0,  0,  0,  0,  0};

int exp43win  [DATA_SIZE] = { 0,  0,  1,  1,  1,  0,  1,  0};
int exp43peak [DATA_SIZE] = { 3, -1,  0,  0,  0,  0,  0,  0};
int exp43winc [DATA_SIZE] = { 1,  0,  1,  1,  1,  0,  1,  1};
int exp43peakc[DATA_SIZE] = { 3,  7, -1,  0,  0,  0,  0,  0};

bool array_cmp(int a[DATA_SIZE][1], int b[DATA_SIZE])
{
   if (memcmp(a, b, DATA_SIZE * sizeof(*a)) == 0) {
      return true;
   } else {
      int i;
      std::cerr << "[";
      for (i = 0; i < DATA_SIZE - 1; ++ i) {
         std::cerr << a[i][0] << ",";
      }
      std::cerr << a[i][0] << "] != [";
      for (i = 0; i < DATA_SIZE - 1; ++ i) {
         std::cerr << b[i] << ",";
      }
      std::cerr << b[i] << "]" << std::endl;
      return false;
   }
}

BOOST_AUTO_TEST_CASE(_window_filter)
{
   Histogram<int, 1> hist(DATA_SIZE);
   memcpy(hist._counts, data1, sizeof(data1));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilter(1, 1);
   BOOST_CHECK(array_cmp(hist._window, exp11win));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp11peak));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilter(2, 1);
   BOOST_CHECK(array_cmp(hist._window, exp12win));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp12peak));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilter(3, 1);
   BOOST_CHECK(array_cmp(hist._window, exp13win));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp13peak));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilter(4, 1);
   BOOST_CHECK(array_cmp(hist._window, exp14win));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp14peak));

   memcpy(hist._counts, data2, sizeof(data2));
   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilter(3, 1);
   BOOST_CHECK(array_cmp(hist._window, exp23win));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp23peak));

}

BOOST_AUTO_TEST_CASE(_window_filter_loop)
{
   Histogram<int, 1> hist(DATA_SIZE);
   memcpy(hist._counts, data1, sizeof(data1));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilterLoop(1, 1);
   BOOST_CHECK(array_cmp(hist._window, exp11winc));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp11peakc));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilterLoop(2, 1);
   BOOST_CHECK(array_cmp(hist._window, exp12winc));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp12peakc));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilterLoop(3, 1);
   BOOST_CHECK(array_cmp(hist._window, exp13winc));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp13peakc));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilterLoop(4, 1);
   BOOST_CHECK(array_cmp(hist._window, exp14winc));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp14peakc));

   memcpy(hist._counts, data2, sizeof(data2));
   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilterLoop(3, 1);
   BOOST_CHECK(array_cmp(hist._window, exp23winc));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp23peakc));

}

BOOST_AUTO_TEST_CASE(_window_filter_uniform)
{
   Histogram<int, 1> hist(DATA_SIZE);
   memcpy(hist._counts, data3, sizeof(data3));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilter(3, 1);
   BOOST_CHECK(array_cmp(hist._window, exp33win));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp33peak));

   memcpy(hist._counts, data4, sizeof(data4));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilter(3, 1);
   BOOST_CHECK(array_cmp(hist._window, exp43win));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp43peak));
}

BOOST_AUTO_TEST_CASE(_window_filter_loop_uniform)
{
   Histogram<int, 1> hist(DATA_SIZE);
   memcpy(hist._counts, data3, sizeof(data3));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilterLoop(3, 1);
   BOOST_CHECK(array_cmp(hist._window, exp33winc));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp33peakc));

   memcpy(hist._counts, data4, sizeof(data4));

   memset(hist._window, 0, sizeof(*hist._window) * hist.size);
   memset(hist._peaks , 0, sizeof(*hist._peaks ) * hist.size);
   hist.applyWindowFilterLoop(3, 1);
   BOOST_CHECK(array_cmp(hist._window, exp43winc));
   BOOST_CHECK(array_cmp((int (*)[1])hist._peaks, exp43peakc));
}

BOOST_AUTO_TEST_SUITE_END()

