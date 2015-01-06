############################ INCLUDE DIRECTORY
# Define include directories here

PROJECT(TEST_RUNSWIFT)

INCLUDE_DIRECTORIES( ${BOOST_INCLUDE_DIR} ${PTHREAD_INCLUDE_DIR} ${BOOST_INCLUDE_DIR} )

SET(TEST_RUNSWIFT_SRCS
        tests/TestHistogram.cpp
        tests/TestRANSACTypes.cpp
        tests/TestBresenhamPtr.cpp
        tests/TestRansac.cpp
        tests/TestFovea.cpp

        perception/vision/Ransac.cpp


        #ROBOT FILTER TESTS AND DEPENDENCIES
        tests/perception/localisation/robotfilter/TestRobotFilter.cpp
        tests/perception/localisation/robotfilter/types/TestRobotObservation.cpp
        tests/perception/localisation/robotfilter/types/TestGroupedRobots.cpp

        perception/localisation/robotfilter/RobotFilter.cpp
        perception/localisation/robotfilter/types/GroupedRobots.cpp
        perception/localisation/robotfilter/types/RobotObservation.cpp
)

SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-access-control" )

ADD_EXECUTABLE( testrunswift ${TEST_RUNSWIFT_SRCS})

############################ SET LIBRARIES TO LINK WITH
# Add any 3rd party libraries to link each target with here
SET ( RUNSWIFT_BOOST  ${Boost_SYSTEM_LIBRARY}
                      ${Boost_REGEX_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
                      ${Boost_SERIALIZATION_LIBRARY}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

TARGET_LINK_LIBRARIES( testrunswift ${PTHREAD_LIBRARIES} ${RUNSWIFT_BOOST} ${PYTHON_LIBRARY} )

