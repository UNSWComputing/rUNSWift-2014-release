############################ INCLUDE DIRECTORY
# Define include directories here
INCLUDE_DIRECTORIES( "." ".." ${BOOST_INCLUDE_DIR} ${PTHREAD_INCLUDE_DIR} ${BOOST_INCLUDE_DIR} )

ADD_EXECUTABLE( testreceiver testreceiver.cpp )

############################ SET LIBRARIES TO LINK WITH
# Add any 3rd party libraries to link each target with here
SET ( RUNSWIFT_BOOST  ${Boost_SYSTEM_LIBRARY}
                      ${Boost_REGEX_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
                      ${Boost_SERIALIZATION_LIBRARY}
                      ${PYTHON_LIBRARY} )

TARGET_LINK_LIBRARIES( testreceiver soccer-static ${PTHREAD_LIBRARIES} ${RUNSWIFT_BOOST} -lz )

