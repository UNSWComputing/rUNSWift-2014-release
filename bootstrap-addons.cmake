SET(DEFAULT_CTC_DIR "/home/robocup/2011/nao-cross-toolchain-1.10.25")

IF (IS_DIRECTORY "$ENV{CTC_DIR}")
   SET(CTC_DIR "$ENV{CTC_DIR}")
ELSE (IS_DIRECTORY "$ENV{CTC_DIR}")
   SET(CTC_DIR "${DEFAULT_CTC_DIR}")
ENDIF (IS_DIRECTORY "$ENV{CTC_DIR}")

SET(CTC_ADDONS_DIR "${SYSROOT_DIR}/addons")

IF (NOT IS_DIRECTORY "${CTC_ADDONS_DIR}")
   MESSAGE(FATAL_ERROR "Please set the environment variable CTC_DIR and install the addons!")
ENDIF (NOT IS_DIRECTORY "${CTC_ADDONS_DIR}" )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CTC_ADDONS_DIR}/usr/include")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${CTC_ADDONS_DIR}/usr/lib")
#SET(CMAKE_FIND_ROOT_PATH "${CTC_ADDONS_DIR}")
