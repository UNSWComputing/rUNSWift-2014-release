#define BOOST_TEST_DYN_LINK
#include <stdlib.h>
#include <math.h>

#include <algorithm>

#include <boost/test/unit_test.hpp>

#include "types/RansacTypes.hpp"

BOOST_AUTO_TEST_SUITE(ransac_types)

BOOST_AUTO_TEST_CASE(circle_2p)
{
   srandom(2);

   PointF c;
   Point p1, p2;
   int rad;

   int i;
   for (i = 0; i < 1000; ++ i) {
      rad   = random() % 900 + 100;

      c.x() = random() % 2000 - 1000;
      c.y() = random() % 2000 - 1000;

      p1.x() = c.x() + random() %  (rad * 2 + 1) - rad;
      p1.y() = c.y() + sqrt(rad*rad - ((c.x() - p1.x())*(c.x() - p1.x())));
      
      do {
         p2.x() = c.x() + random() % (rad * 2 + 1) - rad;
         p2.y() = c.y() + sqrt(rad*rad - ((c.x() - p2.x())*(c.x() - p2.x())));
      } while (p1 == p2);

      RANSACCircle rc(p1, p2, rad);

      if (! isnan(rc.radius)) {
         BOOST_CHECK_CLOSE((c - p1.cast<float>()).norm(), (float)rad, 1);
         BOOST_CHECK_CLOSE((c - p2.cast<float>()).norm(), (float)rad, 1);
      }
   }
}

BOOST_AUTO_TEST_CASE(circle_3p)
{
   srandom(2);

   PointF c;
   Point p[3];
   int rad;

   int i, k;
   double angle;
   for (i = 0; i < 1000; ++ i) {
      rad   = random() % 900 + 100;

      c.x() = random() % 2000 - 1000;
      c.y() = random() % 2000 - 1000;


      for (k = 0; k < 3; ++ k) {
         angle = ((random() % 32) / 32) * 2 * M_PI;
         p[k].x() = rad * cos(angle);
         p[k].y() = rad * sin(angle);
      }
         
      RANSACCircle rc(p[0], p[1], p[2]);

      if (! isnan(rc.radius)) {
         BOOST_CHECK_CLOSE(rc.centre.x(), c.x(), 1);
         BOOST_CHECK_CLOSE(rc.centre.y(), c.y(), 1);
      }
   }
}

BOOST_AUTO_TEST_SUITE_END()

