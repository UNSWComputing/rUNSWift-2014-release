PROJECT( RUNSWIFT )

############################ PROJECT SOURCES FILES
# Add source files needed to compile this project

SET(SOCCER_SRCS
   soccer.cpp

   # Python
   perception/behaviour/python/PythonSkill.cpp
   perception/behaviour/python/RobotModule.cpp
   perception/behaviour/python/RegisterConverters.cpp
   perception/behaviour/ReadySkillPositionAllocation.cpp

   # Vision
   perception/vision/Vision.cpp
   perception/vision/VisionAdapter.cpp
   perception/vision/rle.cpp
   perception/vision/VisionDefs.cpp
   perception/vision/Camera.cpp
   perception/vision/BallDetection.cpp
   perception/vision/GoalDetection.cpp
   perception/vision/OldRobotDetection.cpp
   perception/vision/robotdetection/RobotDetection.cpp
   perception/vision/robotdetection/types/PossibleRobot.cpp
   perception/vision/robotdetection/detection/obstructiondetector/ObstructionDetector.cpp
   perception/vision/robotdetection/detection/robotfeetdetector/RobotFeetDetector.cpp
   perception/vision/robotdetection/detection/robotheightestimator/RobotHeightEstimator.cpp
   perception/vision/robotdetection/detection/robotsonardetector/RobotSonarDetector.cpp
   perception/vision/robotdetection/detection/bottomdetectionmerger/BottomDetectionMerger.cpp
   perception/vision/robotdetection/analysis/robotmerger/RobotMerger.cpp
   perception/vision/robotdetection/analysis/robotsidedetector/RobotSideDetector.cpp
   perception/vision/robotdetection/analysis/robotwidener/RobotWidener.cpp
   perception/vision/robotdetection/analysis/sanitychecks/RobotSanityChecker.cpp
   perception/vision/robotdetection/analysis/sanitychecks/RobotSanityCheckerBottom.cpp
   perception/vision/robotdetection/analysis/sanitychecks/ShoulderExclusionSanityCheck.cpp
   perception/vision/robotdetection/analysis/bayesian/BayesianRobotValidator.cpp
   perception/vision/robotdetection/analysis/bayesian/TrainingSet.cpp
   #perception/vision/FootDetection.cpp
   perception/vision/FieldEdgeDetection.cpp
   perception/vision/FieldLineDetection.cpp
   perception/vision/SurfDetection.cpp
   perception/vision/Integral.cpp
   perception/vision/Fasthessian.cpp
   perception/vision/Vocab.cpp
   perception/vision/CameraToRR.cpp
   perception/vision/yuv.cpp
   perception/vision/Ransac.cpp
   perception/vision/VisualOdometry.cpp
   perception/vision/GoalMatcher.cpp
   perception/vision/Tfidf.cpp
   perception/vision/VisionFrame.cpp
   perception/vision/NNMC.cpp
   perception/vision/VarianceCalculator.cpp

   perception/dumper/PerceptionDumper.cpp

   # Localisation
   perception/localisation/LocalisationAdapter.cpp
   perception/localisation/LocalisationUtils.cpp
   perception/localisation/Localiser.cpp

   ## RobotFilter
   perception/localisation/robotfilter/RobotFilter.cpp
   perception/localisation/robotfilter/types/GroupedRobots.cpp
   perception/localisation/robotfilter/types/RobotObservation.cpp


   perception/localisation/ICP.cpp
   perception/localisation/SharedDistribution.cpp
   perception/localisation/SimpleGaussian.cpp
   perception/localisation/MultiGaussianDistribution.cpp
   perception/localisation/LocalisationConstantsProvider.cpp
   perception/localisation/VarianceProvider.cpp
   perception/localisation/ObservedPostsHistory.cpp
   perception/localisation/TeamBallTracker.cpp

   # Kinematics
   perception/kinematics/KinematicsAdapter.cpp
   perception/kinematics/Kinematics.cpp
   perception/kinematics/SonarFilter.cpp
   perception/kinematics/Pose.cpp
   perception/kinematics/Parameters.cpp
   perception/kinematics/CKF.cpp

   # Behaviour
   perception/behaviour/BehaviourAdapter.cpp
   perception/behaviour/BehaviourHelpers.cpp
   perception/behaviour/SafetySkill.cpp
   perception/behaviour/KinematicsCalibrationSkill.cpp

   # Types
   types/PostInfo.cpp
   types/RobotInfo.cpp
   types/FieldFeatureInfo.cpp
   types/BallInfo.cpp
   types/XYZ_Coord.hpp
   types/TeamBallInfo.cpp
   types/SPLStandardMessage.hpp

   # Misc
   utils/Connection.cpp
   utils/LeastSquaresLine.cpp
   utils/Cluster.cpp
   #utils/bzip_compress.cpp
   utils/options.cpp
   utils/Logger.cpp
   utils/NaoVersion.cpp
   gamecontroller/GameController.cpp
   gamecontroller/RoboCupGameControlData.cpp
   transmitter/OffNao.cpp
   transmitter/Nao.cpp
   transmitter/Team.cpp
   transmitter/NaturalLandmarks.cpp
   receiver/Nao.cpp
   receiver/RemoteControl.cpp
   receiver/Team.cpp
   blackboard/Blackboard.cpp
   thread/ThreadManager.cpp
   thread/Thread.cpp

   # Motion
   motion/generator/ActionGenerator.cpp
   motion/generator/ClippedGenerator.cpp
   motion/generator/BodyModel.cpp
   motion/generator/RLPlanner.cpp
   motion/generator/DistributedGenerator.cpp
   motion/generator/HeadGenerator.cpp
   motion/generator/NullGenerator.cpp
   motion/generator/RefPickupGenerator.cpp
   motion/generator/StandGenerator.cpp
   motion/generator/DeadGenerator.cpp
   motion/generator/Walk2014Generator.cpp
   motion/generator/WalkCycle.cpp
   motion/generator/PendulumModel.cpp
   motion/generator/WalkEnginePreProcessor.cpp

   motion/touch/NullTouch.cpp
   motion/touch/FilteredTouch.cpp
   motion/touch/TorsoStateFilter.cpp
   motion/touch/FeetState.cpp
   motion/SonarRecorder.cpp
   )

############################ CHECK LIBRARY / EXECUTABLE OPTION

ADD_LIBRARY(soccer-static STATIC ${SOCCER_SRCS})
ADD_LIBRARY(soccer SHARED version.cpp)

find_package(PythonLibs 2 REQUIRED)

############################ INCLUDE DIRECTORY
# Define include directories here
set_source_files_properties(
   perception/behaviour/python/PythonSkill.cpp
   perception/behaviour/BehaviourAdapter.cpp
   perception/behaviour/python/RobotModule.cpp
   perception/behaviour/python/RegisterConverters.cpp
   PROPERTIES COMPILE_FLAGS "-I${PYTHON_INCLUDE_DIR}")
if(CMAKE_TOOLCHAIN_FILE)#this is temporary while the toolchain is broken
   set_source_files_properties(
      perception/vision/Vocab.cpp
      perception/vision/Tfidf.cpp
      PROPERTIES COMPILE_FLAGS "-Wno-sign-compare")
endif(CMAKE_TOOLCHAIN_FILE)#this is temporary while the toolchain is broken

SET_TARGET_PROPERTIES(soccer-static PROPERTIES OUTPUT_NAME "soccer")
SET_TARGET_PROPERTIES(soccer-static PROPERTIES PREFIX "lib")
SET_TARGET_PROPERTIES(soccer PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(soccer-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

TARGET_LINK_LIBRARIES(soccer soccer-static)

ADD_CUSTOM_COMMAND ( OUTPUT version.cpp
   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../bin/genversion.pl > version.cpp
)

SET ( SWIG_DEPENDENCIES
   perception/behaviour/python/robot.i
   perception/behaviour/python/eigen.i
   utils/body.hpp
   utils/boostSerializationVariablesMap.hpp
   utils/SPLDefs.hpp
   utils/speech.hpp
   perception/kinematics/Parameters.hpp
   perception/vision/RobotRegion.hpp
   perception/vision/WhichCamera.hpp
   perception/kinematics/Pose.hpp
   gamecontroller/RoboCupGameControlData.hpp
   types/BehaviourRequest.hpp
   types/Point.hpp
   types/PPoint.hpp
   types/BBox.hpp
   types/ActionCommand.hpp
   types/ButtonPresses.hpp
   types/Odometry.hpp
   types/JointValues.hpp
   types/SensorValues.hpp
   types/RRCoord.hpp
   types/AbsCoord.hpp
   types/BroadcastData.hpp
   types/FootInfo.hpp
   types/BallInfo.hpp
   types/PostInfo.hpp
   types/RobotInfo.hpp
   types/FieldEdgeInfo.hpp
   types/FieldFeatureInfo.hpp
   blackboard/Blackboard.hpp
)

ADD_DEFINITIONS( -DEIGEN_DONT_ALIGN )

#ADD_CUSTOM_COMMAND ( OUTPUT RobotModule.cpp
#   COMMAND swig2.0 -Wextra -w509 -python -c++ -I${CMAKE_CURRENT_SOURCE_DIR} -o RobotModule.cpp ${CMAKE_CURRENT_SOURCE_DIR}/perception/behaviour/python/robot.i
#        && patch robot.py ${CMAKE_CURRENT_SOURCE_DIR}/perception/behaviour/python/robot.py.patch
#        && patch RobotModule.cpp ${CMAKE_CURRENT_SOURCE_DIR}/perception/behaviour/python/RobotModule.cpp.patch
#        && mv robot.py ${CMAKE_CURRENT_SOURCE_DIR}/../image/home/nao/data/behaviours/robot.py
#   DEPENDS ${SWIG_DEPENDENCIES}
#)

#SET_SOURCE_FILES_PROPERTIES( RobotModule.cpp PROPERTIES GENERATED TRUE )
SET_SOURCE_FILES_PROPERTIES( version.cpp PROPERTIES GENERATED TRUE )
SET_SOURCE_FILES_PROPERTIES( log.cpp PROPERTIES GENERATED TRUE )

############################ SET LIBRARIES TO LINK WITH
# Add any 3rd party libraries to link each target with here
find_package(Boost COMPONENTS system python program_options thread serialization regex unit_test_framework REQUIRED)
find_package(ZLIB    REQUIRED)
find_package(BZip2   REQUIRED)
find_package(Threads REQUIRED)
SET ( RUNSWIFT_BOOST  ${Boost_SYSTEM_LIBRARY}
                      ${Boost_REGEX_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
                      ${Boost_SERIALIZATION_LIBRARY}
                      ${Boost_PYTHON_LIBRARY} )

TARGET_LINK_LIBRARIES( soccer  ${CMAKE_THREAD_LIBS_INIT} ${RUNSWIFT_BOOST} ${PYTHON_LIBRARY} ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES})

ADD_CUSTOM_TARGET ( uncrustify
   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../bin/uncrust ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../utils/uncrustify.cfg
   COMMENT "Running uncrustify"
)
