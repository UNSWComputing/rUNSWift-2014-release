SET(ROBOT_SRCS
   # Vision
   perception/vision/NaoCamera.cpp
   perception/vision/NaoCameraV4.cpp

   # Perception
   perception/PerceptionThread.cpp #nothing robot specific, but we depend on visionadapter

   # Motion
   motion/MotionAdapter.cpp
   motion/effector/AgentEffector.cpp
   motion/touch/AgentTouch.cpp
   )

set_source_files_properties(
   perception/PerceptionThread.cpp
   main.cpp
   PROPERTIES COMPILE_FLAGS "-I${PYTHON_INCLUDE_DIR}")

ADD_LIBRARY(robot-static STATIC ${ROBOT_SRCS} )
TARGET_LINK_LIBRARIES( robot-static soccer-static )
SET_TARGET_PROPERTIES(robot-static PROPERTIES OUTPUT_NAME "robot")
SET_TARGET_PROPERTIES(robot-static PROPERTIES PREFIX "lib")
SET_TARGET_PROPERTIES(robot-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

ADD_EXECUTABLE( runswift main.cpp version.cpp )
TARGET_LINK_LIBRARIES( runswift robot-static ${PTHREAD_LIBRARIES} ${RUNSWIFT_BOOST} ${PYTHON_LIBRARY} -lz -lbz2)

ADD_CUSTOM_COMMAND ( TARGET runswift POST_BUILD
   COMMAND rm version.cpp
)
