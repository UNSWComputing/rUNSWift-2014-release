#include <string.h>

#include <iostream>

#include <boost/test/unit_test.hpp>

#include "TestFoveaData.hpp"
#include "perception/vision/Fovea.hpp"

BOOST_AUTO_TEST_SUITE(vision_fovea)

BOOST_AUTO_TEST_CASE(grey)
{
   FoveaT<hGoals, eGrey> fovea(BBox(Point(0,0), Point(20, 30)), 1);
   memcpy(fovea._grey, random_in, sizeof(random_in));

   fovea.blurGrey();
   BOOST_CHECK(memcmp(fovea._grey, grey_out, sizeof(grey_out)) == 0);
}

BOOST_AUTO_TEST_CASE(edge)
{
   FoveaT<hGoals, eGrey> fovea(BBox(Point(0,0), Point(20, 30)), 1);
   memcpy(fovea._grey, random_in, sizeof(random_in));

   fovea.blurGrey();
   fovea.makeEdge();
   BOOST_CHECK(memcmp(fovea._edge, edge_out, sizeof(edge_out)) == 0);
}

BOOST_AUTO_TEST_SUITE_END()

