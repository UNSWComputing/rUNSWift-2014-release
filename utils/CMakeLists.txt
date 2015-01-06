cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)

include(bootstrap-qt.cmake)
include(bootstrap-robotdir.cmake)

# offnao and vision tests are required due to use of absolute
# paths in header files
include_directories("." "offnao")

add_subdirectory(offnao)

