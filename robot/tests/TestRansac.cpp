#define BOOST_TEST_DYN_LINK
#include <stdlib.h>
#include <math.h>

#include <algorithm>
#include <vector>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include <boost/test/unit_test.hpp>

#include "perception/vision/Ransac.hpp"
#include "types/RansacTypes.hpp"

#define N_POINTS 16

/* With -fno-access-control unititialized warnings are raised */
#pragma GCC diagnostic ignored "-Wuninitialized"

BOOST_AUTO_TEST_SUITE(ransac)

BOOST_AUTO_TEST_CASE(circle_exact)
{
   std::vector<Point> pts(N_POINTS);

   std::vector<bool> cons_buf[2];
   cons_buf[0].insert(cons_buf[0].end(), N_POINTS, false);
   cons_buf[1].insert(cons_buf[1].end(), N_POINTS, false);

   std::vector<bool> *cons;

   RANSACCircle result;

   unsigned int seed = 0;

   PointF c;

   int i;
   for (i = 0; i < 1000; ++ i) {
      c.x() = random() % 2000 - 1000;
      c.y() = random() % 2000 - 1000;

      float radius = random() % 900 + 100;

      int j;
      for (j = 0; j < N_POINTS; ++ j) {
         pts[j] = (c + PointF(cos(j * 2 * M_PI / N_POINTS),
                              sin(j * 2 * M_PI / N_POINTS))
               * radius).cast<int>();
      }

      RANSAC::findCircleOfRadius(pts, radius, &cons, result, 32, 10,
            N_POINTS / 2, cons_buf, &seed);

      BOOST_CHECK_LT(result.centre.x() - c.x(), 10);
      BOOST_CHECK_LT(result.centre.y() - c.y(), 10);
   }
}

BOOST_AUTO_TEST_CASE(circle_noise)
{
   boost::mt19937 gen;

   std::vector<Point> pts(N_POINTS * 3);

   std::vector<bool> cons_buf[2];
   cons_buf[0].insert(cons_buf[0].end(), N_POINTS, false);
   cons_buf[1].insert(cons_buf[1].end(), N_POINTS, false);

   std::vector<bool> *cons;

   RANSACCircle result;

   unsigned int seed = 0;

   PointF c;

   int i, j, k;
   for (i = 0; i < 1000; ++ i) {
      c.x() = random() % 2000 - 1000;
      c.y() = random() % 2000 - 1000;

      float radius = random() % 900 + 100;

      boost::normal_distribution<> dist(0, 0.01);
      boost::variate_generator<boost::mt19937&, boost::normal_distribution<> >
         noise(gen, dist);

      for (j = 0; j < N_POINTS; ++ j) {
         for (k = 0; k < 3; ++ k) {
            pts[j*3 + k] = (c + PointF(cos(j * 2 * M_PI / N_POINTS),
                                       sin(j * 2 * M_PI / N_POINTS))
                            * radius * (1 + noise())).cast<int>();
         }
      }

      RANSAC::findCircleOfRadius(pts, radius, &cons, result, 100, 10,
            N_POINTS / 2, cons_buf, &seed);

      BOOST_CHECK_LT(result.centre.x() - c.x(), 100);
      BOOST_CHECK_LT(result.centre.y() - c.y(), 100);
   }
}

BOOST_AUTO_TEST_SUITE_END()

