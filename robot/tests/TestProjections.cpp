#include <iostream>
#include <boost/test/unit_test.hpp>

#include "perception/kinematics/Pose.hpp"

using namespace boost::numeric::ublas;


BOOST_AUTO_TEST_SUITE(kinematics_pose)

BOOST_AUTO_TEST_CASE(forward_backwards)
{
   matrix<float> t(4, 4);
   t(0,0) = -0.190343; t(1,0) = -0.980772; t(2,0) =  0.043068; t(3,0) = 0.0,
   t(0,1) = -0.291743; t(1,1) =  0.014622; t(2,1) = -0.956385; t(3,1) = 0.0,
   t(0,2) =  0.937366; t(1,2) = -0.194606; t(2,2) = -0.288916; t(3,2) = 0.0,
   t(0,3) = 49.128441; t(1,3) = -5.226813; t(2,3) =  493.6120; t(3,3) = 1.0;

   Pose pose(t, std::pair<int, int>(0, 0));

   int i;
   for (i = 0; i < 1000; ++ i) {
      Point test  = Point(rand() % IMAGE_COLS, rand() % IMAGE_ROWS);

      int h = rand() % 100;
      Point robot = pose.imageToRobotXY(test , h);
      Point image = pose.robotToImageXY(robot, h);

      if (robot.x() < 0) {
         continue;
      }
      /*
      printf ("%d %d -> %d %d -> %d %d\n",
              test.x(), test.y(), 
              robot.x(), robot.y(),
              image.x(), image.y());
      */

      BOOST_CHECK_LT(abs(test.x() - image.x()), 3);
      BOOST_CHECK_LT(abs(test.y() - image.y()), 3);

   }
}

BOOST_AUTO_TEST_SUITE_END()

