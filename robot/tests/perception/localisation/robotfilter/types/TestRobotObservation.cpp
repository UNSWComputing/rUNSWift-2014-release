#include <string.h>

#include <iostream>

#include <math.h>

#include <boost/test/unit_test.hpp>

#include "types/RobotInfo.hpp"
#include "types/RRCoord.hpp"
#include "types/Odometry.hpp"
#include "perception/localisation/robotfilter/types/RobotObservation.hpp"
#include "perception/vision/VisionDefs.hpp"

#include "utils/angles.hpp"

#define NINETY_DEGREES M_PI_2
#define RADIAN_ERROR 0.001
#define CMs(x) (x * 10)

const static float DEFAULT_HEAD_YAW = 0.0f;

const static float MAX_VISION_HEADING = 0.5f * IMAGE_HFOV;
const static float MIN_VISION_HEADING = -MAX_VISION_HEADING;

const static double DEFAULT_DISTANCE_TO_GROUP = 0.0;

const static Odometry EMPTY_ODOMETRY;

BOOST_AUTO_TEST_SUITE(observation_test)

BOOST_AUTO_TEST_CASE(test_odometry_forward)
{
    const static int FORWARD_MOVEMENT = CMs(5);
    unsigned int distance = CMs(10);
    RobotInfo visualRobot;
    visualRobot.rr = RRCoord(distance, 0);

    RobotObservation observation(visualRobot);

    Odometry odometry;

    odometry.forward = FORWARD_MOVEMENT;
    odometry.left = 0;
    odometry.turn = 0;

    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    BOOST_CHECK_EQUAL(observation.getRRCoordinates().distance(), distance - FORWARD_MOVEMENT);
}

BOOST_AUTO_TEST_CASE(test_odometry_back)
{
    const static int BACK_MOVEMENT = CMs(5);
    unsigned int distance = CMs(10);
    RobotInfo visualRobot;
    visualRobot.rr = RRCoord(distance, 0);

    RobotObservation observation(visualRobot);

    Odometry odometry;

    odometry.forward = -BACK_MOVEMENT;
    odometry.left = 0;
    odometry.turn = 0;

    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    BOOST_CHECK_EQUAL(observation.getRRCoordinates().distance(), distance + BACK_MOVEMENT);
}


BOOST_AUTO_TEST_CASE(test_odometry_left)
{
    const static int LEFT_MOVEMENT = CMs(4);
    unsigned int distance = CMs(3);
    RobotInfo visualRobot;
    visualRobot.rr = RRCoord(distance, 0);

    RobotObservation observation(visualRobot);

    Odometry odometry;

    odometry.forward = 0;
    odometry.left = LEFT_MOVEMENT;
    odometry.turn = 0;

    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    RRCoord newCoordinates = observation.getRRCoordinates();
    BOOST_CHECK_EQUAL(newCoordinates.distance(), CMs(5));
    BOOST_CHECK_CLOSE(newCoordinates.heading(), -0.92729, RADIAN_ERROR);
}

BOOST_AUTO_TEST_CASE(test_odometry_right)
{
    const static int LEFT_MOVEMENT = -CMs(4);
    unsigned int distance = CMs(3);
    RobotInfo visualRobot;
    visualRobot.rr = RRCoord(distance, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    RobotObservation observation(visualRobot);

    Odometry odometry;

    odometry.forward = 0;
    odometry.left = LEFT_MOVEMENT;
    odometry.turn = 0;

    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    RRCoord newCoordinates = observation.getRRCoordinates();
    BOOST_CHECK_EQUAL(newCoordinates.distance(), CMs(5));
    BOOST_CHECK_CLOSE(newCoordinates.heading(), 0.92729, RADIAN_ERROR);
}


BOOST_AUTO_TEST_CASE(test_odometry_turn_left)
{
    RobotInfo visualRobot;
    unsigned int DISTANCE = CMs(3);
    visualRobot.rr = RRCoord(DISTANCE, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    RobotObservation observation(visualRobot);

    Odometry odometry;

    odometry.forward = 0;
    odometry.left = 0;
    odometry.turn = NINETY_DEGREES;

    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    RRCoord newCoordinates = observation.getRRCoordinates();
    BOOST_CHECK_EQUAL(newCoordinates.distance(), DISTANCE);
    BOOST_CHECK_CLOSE(newCoordinates.heading(), -NINETY_DEGREES, RADIAN_ERROR);

    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);
    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);
    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    newCoordinates = observation.getRRCoordinates();
    BOOST_CHECK_CLOSE(newCoordinates.heading(), 0, RADIAN_ERROR);
}


BOOST_AUTO_TEST_CASE(test_odometry_turn_right)
{
    RobotInfo visualRobot;
    unsigned int DISTANCE = CMs(3);
    visualRobot.rr = RRCoord(DISTANCE, 0);

    RobotObservation observation(visualRobot);

    Odometry odometry;

    odometry.forward = 0;
    odometry.left = 0;
    odometry.turn = -NINETY_DEGREES;

    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    RRCoord newCoordinates = observation.getRRCoordinates();
    BOOST_CHECK_EQUAL(newCoordinates.distance(), DISTANCE);
    BOOST_CHECK_CLOSE(newCoordinates.heading(), NINETY_DEGREES, RADIAN_ERROR);

    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);
    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);
    observation.tick(odometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    newCoordinates = observation.getRRCoordinates();
    BOOST_CHECK_CLOSE(newCoordinates.heading(), 0, RADIAN_ERROR);
}


BOOST_AUTO_TEST_CASE(test_onscreen_stale)
{
    RobotInfo visualRobot;
    unsigned int DISTANCE = CMs(3);
    visualRobot.rr = RRCoord(DISTANCE, 0);

    RobotObservation observation(visualRobot);

    Odometry emptyOdometry;

    for (unsigned int i = 0; i < RobotObservation::numberOnScreenFramesToBeStale(); ++i) {
        BOOST_CHECK(!observation.isStale());
        observation.tick(emptyOdometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);
    }
    BOOST_CHECK(observation.isStale());
}

BOOST_AUTO_TEST_CASE(test_inView)
{

    RobotInfo visualRobot;
    unsigned int DISTANCE = CMs(100);
    visualRobot.rr = RRCoord(DISTANCE, 0);

    RobotObservation observation(visualRobot);

    Odometry emptyOdometry;

    BOOST_CHECK_EQUAL(observation.inView(DEFAULT_HEAD_YAW), true);
    BOOST_CHECK_EQUAL(observation.inView(MIN_VISION_HEADING), true);
    BOOST_CHECK_EQUAL(observation.inView(MAX_VISION_HEADING), true);
    BOOST_CHECK_EQUAL(observation.inView(MIN_VISION_HEADING - DEG2RAD(10)), false);
    BOOST_CHECK_EQUAL(observation.inView(MAX_VISION_HEADING + DEG2RAD(10)), false);
}

BOOST_AUTO_TEST_CASE(test_offscreen_stale)
{
    RobotInfo visualRobot;
    unsigned int DISTANCE = CMs(3);
    visualRobot.rr = RRCoord(DISTANCE, 0);

    RobotObservation observation(visualRobot);
    RobotObservation observationTwo(visualRobot);

    Odometry leftOdometry, rightOdometry, emptyOdometry;
    leftOdometry.turn = MIN_VISION_HEADING - DEG2RAD(10);
    rightOdometry.turn = MAX_VISION_HEADING + DEG2RAD(10);

    observation.tick(leftOdometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);
    observationTwo.tick(rightOdometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    BOOST_CHECK_EQUAL(observation.inView(DEFAULT_HEAD_YAW), false);
    BOOST_CHECK_EQUAL(observationTwo.inView(DEFAULT_HEAD_YAW), false);

    for (unsigned int i = 0; i < RobotObservation::numberOffScreenFramesToBeStale() - 1; ++i) {
        BOOST_CHECK(!observation.isStale());
        observation.tick(emptyOdometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);
    }
    BOOST_CHECK(observation.isStale());
}

BOOST_AUTO_TEST_CASE(test_head_yaw_brings_into_view)
{
    RobotInfo visualRobot;
    unsigned int DISTANCE = CMs(100);
    visualRobot.rr = RRCoord(DISTANCE, 0);
    RobotObservation observation(visualRobot);

    Odometry leftOdometry, emptyOdometry;
    leftOdometry.turn = MIN_VISION_HEADING - DEG2RAD(10);
    observation.tick(leftOdometry, DEFAULT_HEAD_YAW, DEFAULT_DISTANCE_TO_GROUP);

    //Cannot see if looking straight
    BOOST_CHECK_EQUAL(observation.inView(DEFAULT_HEAD_YAW), false);

    BOOST_CHECK_EQUAL(observation.inView(DEG2RAD(9)), false);
    //Can see if i turn my head 20 degrees.
    BOOST_CHECK_EQUAL(observation.inView(DEG2RAD(10)), true);
}

BOOST_AUTO_TEST_CASE(test_on_screen_far_from_group)
{
    RobotInfo visualRobot;
    unsigned int DISTANCE = CMs(100);
    visualRobot.rr = RRCoord(DISTANCE, 0);
    RobotObservation observation(visualRobot);

    unsigned int FRAMES_FOR_FAR_AWAY = ceil((float)RobotObservation::MAX_LIFE_SCORE /
            (float)(RobotObservation::ON_SCREEN_SCORE_REDUCER + RobotObservation::DISTANCE_SCORE_REDUCER));

    for (unsigned int i = 0; i < FRAMES_FOR_FAR_AWAY; ++i) {
        BOOST_CHECK_EQUAL(observation.isStale(), false);
        observation.tick(EMPTY_ODOMETRY, DEFAULT_HEAD_YAW, 1);
    }
    BOOST_CHECK_EQUAL(observation.isStale(), true);
}


BOOST_AUTO_TEST_SUITE_END()
