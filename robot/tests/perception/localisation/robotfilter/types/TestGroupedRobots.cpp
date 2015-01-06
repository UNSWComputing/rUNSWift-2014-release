#include <string.h>

#include <iostream>

#include <math.h>

#include <boost/test/unit_test.hpp>

#include "types/RobotInfo.hpp"
#include "types/RRCoord.hpp"
#include "types/AbsCoord.hpp"
#include "types/Odometry.hpp"
#include "perception/localisation/robotfilter/types/RobotObservation.hpp"
#include "perception/localisation/robotfilter/types/GroupedRobots.hpp"

#include "utils/angles.hpp"

#define NINETY_DEGREES M_PI_2
#define RADIAN_ERROR 0.001
#define CMs(x) (x * 10)

const static float DEFAULT_HEAD_YAW = 0.0f;

const static AbsCoord CENTER_FIELD;

const static Odometry EMPTY_ODOMETRY;

BOOST_AUTO_TEST_SUITE(grouped_robots)

//Test whether the cartesian coordinates of a single observation group
//is always that individual robot
BOOST_AUTO_TEST_CASE(cartesian_single_robot_group)
{
    RRCoord robotRR(CMs(100), 0);
    Point robotCartesian = robotRR.toCartesian();
    RobotInfo robot;
    robot.rr = robotRR;

    GroupedRobots group(robot);

    for (unsigned int i = 0; i < 3; ++i) {
        RRCoord groupCoordinates = group.getRRCoordinates();
        BOOST_CHECK_EQUAL(robotRR.distance(), groupCoordinates.distance());
        BOOST_CHECK_EQUAL(robotRR.heading(), groupCoordinates.heading());

        Point groupCartesian = group.getCartesianCoordinates();
        BOOST_CHECK_EQUAL(robotCartesian[0], groupCartesian[0]);
        BOOST_CHECK_EQUAL(robotCartesian[1], groupCartesian[1]);

        Odometry EMPTY_ODOMETRY;
        group.tick(EMPTY_ODOMETRY, DEFAULT_HEAD_YAW, CENTER_FIELD);
    }
}


BOOST_AUTO_TEST_CASE(test_stale_group)
{
    RRCoord robotRR(CMs(100), 0);
    Point robotCartesian = robotRR.toCartesian();
    RobotInfo robot;
    robot.rr = robotRR;

    GroupedRobots group(robot);

    for (unsigned int i = 0; i < RobotObservation::numberOnScreenFramesToBeStale(); ++i) {
        Odometry EMPTY_ODOMETRY;
        group.tick(EMPTY_ODOMETRY, DEFAULT_HEAD_YAW, CENTER_FIELD);
    }
    BOOST_CHECK(group.isEmpty());
}


BOOST_AUTO_TEST_CASE(odometry_group)
{
    const static unsigned int MOVE_DISTANCE = CMs(50);
    RRCoord robotRR(0, 0);
    Point robotCartesian = robotRR.toCartesian();
    RobotInfo robot;
    robot.rr = robotRR;

    GroupedRobots group(robot);
    Odometry odometry;
    odometry.forward = -CMs(50);
    const static unsigned int NUMBER_TICKS = 5;
    for (unsigned int i = 0; i < NUMBER_TICKS; ++i) {
        group.tick(odometry, DEFAULT_HEAD_YAW, CENTER_FIELD);
    }

    BOOST_CHECK_EQUAL(MOVE_DISTANCE * NUMBER_TICKS, group.getRRCoordinates().distance());
}

BOOST_AUTO_TEST_CASE(merge_robot_horizontally_close)
{
    unsigned int xDistance = CMs(100);
    AbsCoord robotA(xDistance, 0, 0);
    RRCoord robotARR = robotA.convertToRobotRelative();
    RobotInfo initialRobot;
    initialRobot.rr = robotARR;
    GroupedRobots group(initialRobot);

    AbsCoord robotB(xDistance, GroupedRobots::ELLIPSE_HORIZONTAL - CMs(10), 0);
    RRCoord robotBRR = robotB.convertToRobotRelative();
    RobotInfo mergeableRobot;
    mergeableRobot.rr = robotBRR;

    AbsCoord robotC(xDistance, GroupedRobots::ELLIPSE_HORIZONTAL + CMs(10), 0);
    RRCoord robotCRR = robotC.convertToRobotRelative();
    RobotInfo unmergeableRobot;
    unmergeableRobot.rr = robotCRR;

    BOOST_CHECK(group.canMergeRobot(mergeableRobot));
    BOOST_CHECK(!group.canMergeRobot(unmergeableRobot));
}

BOOST_AUTO_TEST_CASE(merge_robot_vertically_close)
{
    unsigned int xDistance = CMs(100);
    AbsCoord robotA(xDistance, 0, 0);
    RRCoord robotARR = robotA.convertToRobotRelative();
    RobotInfo initialRobot;
    initialRobot.rr = robotARR;
    GroupedRobots group(initialRobot);

    AbsCoord robotB(xDistance + GroupedRobots::ELLIPSE_VERTICAL - CMs(10), 0, 0);
    RRCoord robotBRR = robotB.convertToRobotRelative();
    RobotInfo mergeableRobot;
    mergeableRobot.rr = robotBRR;

    AbsCoord robotC(xDistance + GroupedRobots::ELLIPSE_VERTICAL + CMs(10), 0, 0);
    RRCoord robotCRR = robotC.convertToRobotRelative();
    RobotInfo unmergeableRobot;
    unmergeableRobot.rr = robotCRR;

    BOOST_CHECK(group.canMergeRobot(mergeableRobot));
    BOOST_CHECK(!group.canMergeRobot(unmergeableRobot));
}

//Giving that the group has an observation about to expire it should
//allow merging of an object a given scale away from the default radius.
//e.g. if it has a max scale of 2 and a default merge of 40cm, then the
//new scaled merge distance would be 80cm.
BOOST_AUTO_TEST_CASE(merge_robot_vertically_max_time)
{
    unsigned int startDistance = CMs(100);
    AbsCoord robotA(startDistance, 0, 0);
    RRCoord robotARR = robotA.convertToRobotRelative();
    RobotInfo initialRobot;
    initialRobot.rr = robotARR;
    GroupedRobots group(initialRobot);
    Odometry EMPTY_ODOMETRY;

    for (unsigned int i = 0; i < RobotObservation::numberOnScreenFramesToBeStale() - 1; ++i) {
        group.tick(EMPTY_ODOMETRY, DEFAULT_HEAD_YAW, CENTER_FIELD);
    }

    unsigned int maxMerge = GroupedRobots::MAX_MERGE_SCALE * GroupedRobots::ELLIPSE_VERTICAL;

    AbsCoord robotB(startDistance + maxMerge - CMs(1), 0, 0);
    RRCoord robotBRR = robotB.convertToRobotRelative();
    RobotInfo mergeableRobot;
    mergeableRobot.rr = robotBRR;

    AbsCoord robotC(startDistance + maxMerge + CMs(1), 0, 0);
    RRCoord robotCRR = robotC.convertToRobotRelative();
    RobotInfo unmergeableRobot;
    unmergeableRobot.rr = robotCRR;

    BOOST_CHECK(group.canMergeRobot(mergeableRobot));
    BOOST_CHECK(!group.canMergeRobot(unmergeableRobot));
}

BOOST_AUTO_TEST_CASE(merge_robot_horizontally_max_time)
{
    unsigned int startDistance = CMs(100);
    AbsCoord robotA(startDistance, 0, 0);
    RRCoord robotARR = robotA.convertToRobotRelative();
    RobotInfo initialRobot;
    initialRobot.rr = robotARR;
    GroupedRobots group(initialRobot);
    Odometry EMPTY_ODOMETRY;

    for (unsigned int i = 0; i < RobotObservation::numberOnScreenFramesToBeStale() - 1; ++i) {
        group.tick(EMPTY_ODOMETRY, DEFAULT_HEAD_YAW, CENTER_FIELD);
    }

    unsigned int maxMerge = GroupedRobots::MAX_MERGE_SCALE * GroupedRobots::ELLIPSE_HORIZONTAL;

    AbsCoord robotB(startDistance, maxMerge - CMs(1),  0);
    RRCoord robotBRR = robotB.convertToRobotRelative();
    RobotInfo mergeableRobot;
    mergeableRobot.rr = robotBRR;

    AbsCoord robotC(startDistance, maxMerge + CMs(1), 0);
    RRCoord robotCRR = robotC.convertToRobotRelative();
    RobotInfo unmergeableRobot;
    unmergeableRobot.rr = robotCRR;

    BOOST_CHECK(group.canMergeRobot(mergeableRobot));
    BOOST_CHECK(!group.canMergeRobot(unmergeableRobot));
}



BOOST_AUTO_TEST_CASE(robot_too_far_forward) {

   RobotInfo robot;
   robot.rr = RRCoord(CMs(300), 0);

   GroupedRobots group;
   group.mergeRobot(robot);
   group.tick(EMPTY_ODOMETRY, DEFAULT_HEAD_YAW, CENTER_FIELD);

   BOOST_CHECK_EQUAL(group.isOnField(), true);
   AbsCoord robotPos(CMs(100), 0, 0);
   group.tick(EMPTY_ODOMETRY, DEFAULT_HEAD_YAW, robotPos);


   BOOST_CHECK_EQUAL(group.isOnField(), true);

   robotPos = AbsCoord(CMs(200), 0, 0);
   group.tick(EMPTY_ODOMETRY, DEFAULT_HEAD_YAW, robotPos);
   BOOST_CHECK_EQUAL(group.isOnField(), false);
}

BOOST_AUTO_TEST_CASE(robot_too_far_right) {

   RobotInfo robot;
   robot.rr = RRCoord(CMs(100), DEG2RAD(45));

   GroupedRobots group;
   group.mergeRobot(robot);
   group.tick(EMPTY_ODOMETRY, DEFAULT_HEAD_YAW, CENTER_FIELD);

   BOOST_CHECK_EQUAL(group.isOnField(), true);
   AbsCoord robotPos(0, CMs(150), 0);
   group.tick(EMPTY_ODOMETRY, DEFAULT_HEAD_YAW, robotPos);
   BOOST_CHECK_EQUAL(group.isOnField(), true);
   robotPos = AbsCoord(0, CMs(250), 0);
   group.tick(EMPTY_ODOMETRY, DEFAULT_HEAD_YAW, robotPos);
   BOOST_CHECK_EQUAL(group.isOnField(), false);
}


BOOST_AUTO_TEST_CASE(robot_tangents) {
    RobotInfo robot;
    robot.rr = RRCoord(CMs(45), 0);

    GroupedRobots group;
    group.mergeRobot(robot);
    group.mergeRobot(robot);

    RobotObstacle obstacle = group.generateRobotObstacle();

    BOOST_CHECK_CLOSE(obstacle.tangentHeadingLeft, DEG2RAD(45), 0.0001);
    BOOST_CHECK_CLOSE(obstacle.tangentHeadingRight, -DEG2RAD(45), 0.0001);
}

BOOST_AUTO_TEST_SUITE_END()
