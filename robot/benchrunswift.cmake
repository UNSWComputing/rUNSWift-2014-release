############################ INCLUDE DIRECTORY
# Define include directories here

PROJECT(BENCH_RUNSWIFT)

INCLUDE_DIRECTORIES( ${BOOST_INCLUDE_DIR} ${PTHREAD_INCLUDE_DIR} ${BOOST_INCLUDE_DIR} )

SET(BENCH_RUNSWIFT_SRCS
        bench/BenchFovea.cpp
)

SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-access-control" )

ADD_EXECUTABLE( benchrunswift ${BENCH_RUNSWIFT_SRCS})

############################ SET LIBRARIES TO LINK WITH
# Add any 3rd party libraries to link each target with here
SET ( RUNSWIFT_BOOST  ${Boost_SYSTEM_LIBRARY}
                      ${Boost_REGEX_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
                      ${Boost_SERIALIZATION_LIBRARY} )

TARGET_LINK_LIBRARIES( benchrunswift ${PTHREAD_LIBRARIES} ${RUNSWIFT_BOOST} ${PYTHON_LIBRARY} )

