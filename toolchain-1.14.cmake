cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

set(DEFAULT_CTC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ctc/linux32-nao-atom-cross-toolchain-1.14.5")
if (IS_DIRECTORY "$ENV{CTC_DIR}")
   set(CTC_DIR "$ENV{CTC_DIR}")
else (IS_DIRECTORY "$ENV{CTC_DIR}")
   set(CTC_DIR "${DEFAULT_CTC_DIR}")
endif (IS_DIRECTORY "$ENV{CTC_DIR}")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 4)

set(CMAKE_C_COMPILER   ${CTC_DIR}/cross/bin/i686-aldebaran-linux-gnu-gcc)

set(CMAKE_FIND_ROOT_PATH  ${CTC_DIR}/cross ${CTC_DIR}/sysroot)
# we may actually want native programs, but i prefer we override that on a FIND_XXX basis
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
