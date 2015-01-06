#include <math.h>

#include <vector>

#include <boost/test/unit_test.hpp>

#include "types/Odometry.hpp"
#include "types/AbsCoord.hpp"
#include "types/RRCoord.hpp"
#include "types/RobotInfo.hpp"
#include "types/RobotObstacle.hpp"
#include "perception/localisation/robotfilter/types/RobotFilterUpdate.hpp"
#include "perception/localisation/robotfilter/RobotFilter.hpp"

#include "utils/angles.hpp"

#define CMs(x) (x * 10)

const static Odometry EMPTY_ODOMETRY;
const static AbsCoord CENTER_FIELD;

BOOST_AUTO_TEST_SUITE(robot_filter)

RobotFilterUpdate createUpdate(std::vector<RobotInfo> visualRobots,
        AbsCoord robotPos,
        float headYaw,
        Odometry odometryDiff,
        bool isIncapacitated) {

    RobotFilterUpdate update;
    update.visualRobots = visualRobots;
    update.robotPos = robotPos;
    update.headYaw = headYaw;
    update.odometryDiff = odometryDiff;
    update.isIncapacitated = isIncapacitated;
    return update;
}

//NOTE: test works under the assumption that I know a group is important
//if it has at least two observations.
BOOST_AUTO_TEST_CASE(cartesian_single_robot_group) {
    RobotFilter robotFilter;

    RobotInfo robotA, robotB;
    robotA.rr = RRCoord(CMs(20), DEG2RAD(-45));
    robotB.rr = RRCoord(CMs(20), DEG2RAD(45));

    std::vector<RobotInfo> visualRobots;
    visualRobots.push_back(robotA);
    visualRobots.push_back(robotB);

    robotFilter.update(createUpdate(visualRobots, CENTER_FIELD, 0, EMPTY_ODOMETRY, false));



    RobotInfo robotC, robotD, robotE;
    robotC.rr = RRCoord(CMs(15), DEG2RAD(-25)); //Should merge into A
    robotD.rr = RRCoord(CMs(15), DEG2RAD(25));  //Should merge into B
    robotE.rr = RRCoord(CMs(20), 0);            //Can merge into A and B but won't as already taken

    visualRobots.clear();
    visualRobots.push_back(robotC);
    visualRobots.push_back(robotD);
    visualRobots.push_back(robotE);
    robotFilter.update(createUpdate(visualRobots, CENTER_FIELD, 0, EMPTY_ODOMETRY, false));
    std::vector<RobotObstacle> obstacles = robotFilter.update(createUpdate(visualRobots, CENTER_FIELD, 0, EMPTY_ODOMETRY, false));
    BOOST_CHECK_EQUAL(obstacles.size(), (unsigned int)2);

    obstacles = robotFilter.update(createUpdate(visualRobots, CENTER_FIELD, 0, EMPTY_ODOMETRY, false));
    BOOST_CHECK_EQUAL(obstacles.size(), (unsigned int)3);
}

BOOST_AUTO_TEST_SUITE_END()
