### Added by jayen for profiling
set ( CMAKE_CXX_FLAGS_PROFILE "-pg -O3 -DNDEBUG" CACHE STRING
    "Flags used by the C++ compiler during profile builds."
    FORCE )

set ( CMAKE_C_FLAGS_PROFILE "-pg -O3 -DNDEBUG" CACHE STRING
    "Flags used by the C compiler during profile builds."
    FORCE )

set ( CMAKE_EXE_LINKER_FLAGS_PROFILE
    "-pg" CACHE STRING
    "Flags used for linking binaries during profile builds."
    FORCE )

set ( CMAKE_MODULE_LINKER_FLAGS_PROFILE
    "-pg" CACHE STRING
    "Flags used for linking binaries during profile builds."
    FORCE )

set ( CMAKE_SHARED_LINKER_FLAGS_PROFILE
    "-pg" CACHE STRING
    "Flags used by the shared libraries linker during profile builds."
    FORCE )

MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_PROFILE
    CMAKE_C_FLAGS_PROFILE
    CMAKE_EXE_LINKER_FLAGS_PROFILE
    CMAKE_MODULE_LINKER_FLAGS_PROFILE
    CMAKE_SHARED_LINKER_FLAGS_PROFILE )

set ( CMAKE_CXX_FLAGS_COVERAGE "--coverage -O3 -DNDEBUG" CACHE STRING
    "Flags used by the C++ compiler during coverage builds."
    FORCE )

set ( CMAKE_C_FLAGS_COVERAGE "--coverage -O3 -DNDEBUG" CACHE STRING
    "Flags used by the C compiler during coverage builds."
    FORCE )

set ( CMAKE_EXE_LINKER_FLAGS_COVERAGE
    "--coverage" CACHE STRING
    "Flags used for linking binaries during coverage builds."
    FORCE )

set ( CMAKE_MODULE_LINKER_FLAGS_COVERAGE
    "--coverage" CACHE STRING
    "Flags used for linking binaries during coverage builds."
    FORCE )

set ( CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    "--coverage" CACHE STRING
    "Flags used by the shared libraries linker during coverage builds."
    FORCE )

MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_MODULE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE )

## Added by Sean for mudflap
set ( CMAKE_CXX_FLAGS_MUDFLAP "-fmudflapth -g" CACHE STRING
    "Flags used by the C++ compiler during mudflap builds."
    FORCE )

set ( CMAKE_C_FLAGS_MUDFLAP "-fmudflapth -g" CACHE STRING
    "Flags used by the C compiler during mudflap builds."
    FORCE )

set ( CMAKE_EXE_LINKER_FLAGS_MUDFLAP
    "-lmudflapth" CACHE STRING
    "Flags used for linking binaries during mudflap builds."
    FORCE )

set ( CMAKE_MODULE_LINKER_FLAGS_MUDFLAP
    "-lmudflapth" CACHE STRING
    "Flags used for linking binaries during mudflap builds."
    FORCE )

set ( CMAKE_SHARED_LINKER_FLAGS_MUDFLAP
    "-lmudflapth" CACHE STRING
    "Flags used by the shared libraries linker during mudflap builds."
    FORCE )

MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_MUDFLAP
    CMAKE_C_FLAGS_MUDFLAP
    CMAKE_EXE_LINKER_FLAGS_MUDFLAP
    CMAKE_MODULE_LINKER_FLAGS_MUDFLAP
    CMAKE_SHARED_LINKER_FLAGS_MUDFLAP )


# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Profile Coverage Mudflap."
    FORCE )
