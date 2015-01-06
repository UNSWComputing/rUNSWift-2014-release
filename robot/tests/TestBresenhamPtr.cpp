#define BOOST_TEST_DYN_LINK
#include <iostream>
#include <boost/test/unit_test.hpp>

#include "utils/BresenhamPtr.hpp"

BOOST_AUTO_TEST_SUITE(bresenham_ptr)

BOOST_AUTO_TEST_CASE(horizontal_left)
{
   int buf[20][40] = {{0}};

   int x, y, i;
   BresenhamPtr<int> line(&buf[0][0], Point(2, 10), Point(18, 10), 40);
   BresenhamPtr<int>::iterator p = line.begin();

   for (i = 0, x = 2, y = 10; x < 18; ++ x, ++ i) {
      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));

      ++ p;
   }
}

BOOST_AUTO_TEST_CASE(horizontal_right)
{
   int buf[20][40] = {{0}};

   int x, y, i;
   BresenhamPtr<int> line(&buf[0][0], Point(18, 10), Point(2, 10), 40);
   BresenhamPtr<int>::iterator p = line.begin();

   for (i = 0, x = 18, y = 10; x > 2; -- x, ++ i) {
      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));

      ++ p;
   }
}

BOOST_AUTO_TEST_CASE(vertical_down)
{
   int buf[20][40] = {{0}};

   int x, y, i;
   BresenhamPtr<int> line(&buf[0][0], Point(10, 2), Point(10, 18), 40);
   BresenhamPtr<int>::iterator p = line.begin();

   for (i = 0, x = 10, y = 2; y < 18; ++ y, ++ i) {
      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));

      ++ p;
   }
}

BOOST_AUTO_TEST_CASE(vertical_up)
{
   int buf[20][40] = {{0}};

   int x, y, i;
   BresenhamPtr<int> line(&buf[0][0], Point(10, 18), Point(10, 2), 40);
   BresenhamPtr<int>::iterator p = line.begin();

   for (i = 0, x = 10, y = 18; y > 2; -- y, ++ i) {
      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));

      ++ p;
   }
}

BOOST_AUTO_TEST_CASE(diag_pp)
{
   int buf[20][40] = {{0}};

   int x, y, i;
   BresenhamPtr<int> line(&buf[0][0], Point(2, 2), Point(18, 18), 40);
   BresenhamPtr<int>::iterator p = line.begin();

   for (i = 0, x = 2, y = 2; y < 18; ++ x, ++ y, ++ i) {
      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));

      ++ p;
   }
}

BOOST_AUTO_TEST_CASE(diag_pn)
{
   int buf[20][40] = {{0}};

   int x, y, i;
   BresenhamPtr<int> line(&buf[0][0], Point(2, 18), Point(18, 2), 40);
   BresenhamPtr<int>::iterator p = line.begin();

   for (i = 0, x = 2, y = 18; x < 18; ++ x, -- y, ++ i) {
      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));

      ++ p;
   }
}

BOOST_AUTO_TEST_CASE(diag_np)
{
   int buf[20][40] = {{0}};

   int x, y, i;
   BresenhamPtr<int> line(&buf[0][0], Point(18, 2), Point(2, 18), 40);
   BresenhamPtr<int>::iterator p = line.begin();

   for (i = 0, x = 18, y = 2; y < 18; -- x, ++ y, ++ i) {
      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));

      ++ p;
   }
}

BOOST_AUTO_TEST_CASE(diag_nn)
{
   int buf[20][40] = {{0}};

   int x, y, i;
   BresenhamPtr<int> line(&buf[0][0], Point(18, 18), Point(2, 2), 40);
   BresenhamPtr<int>::iterator p = line.begin();

   for (i = 0, x = 18, y = 18; y > 2; -- x, -- y, ++ i) {
      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));

      ++ p;
   }
}

BOOST_AUTO_TEST_CASE(diag_12)
{
   int buf[20][40] = {{0}};

   int x, y, i;
   BresenhamPtr<int> line(&buf[0][0], Point(2, 2), Point(18, 36), 40);
   BresenhamPtr<int>::iterator p = line.begin();

   for (i = 0, x = 2, y = 2; x < 10; ++ x, ++ y, ++ i) {
      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));
      ++ p;

      ++ y; ++ i;

      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));
      ++ p;
   }
}

BOOST_AUTO_TEST_CASE(diag_21)
{
   int buf[20][40] = {{0}};

   int x, y, i;
   BresenhamPtr<int> line(&buf[0][0], Point(2, 2), Point(36, 18), 40);
   BresenhamPtr<int>::iterator p = line.begin();

   for (i = 0, x = 2, y = 2; y < 10; ++ x, ++ y, ++ i) {
      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));
      ++ p;

      ++ x; ++ i;

      buf[x][y] = i;
      BOOST_CHECK_EQUAL(*p, i);
      BOOST_CHECK_EQUAL(p.point(), Point(x, y));
      ++ p;
   }
}


BOOST_AUTO_TEST_SUITE_END()

