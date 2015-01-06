cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

PROJECT(OFFNAO)

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

SET(OFFNAO_CXX_SRCS
   utils/OverlayPainter.cpp
   utils/FieldPainter.cpp
   utils/CPlaneColours.cpp
   main.cpp
   mediaPanel.cpp
   visualiser.cpp
   readers/dumpReader.cpp
   readers/networkReader.cpp
   readers/reader.cpp
   readers/recordReader.cpp
   readers/bbdReader.cpp
   tabs/PointCloud.cpp
   tabs/classifier.cpp
   tabs/variableView.cpp
   tabs/plots.cpp
   tabs/yuvHistogram.cpp
   tabs/overviewTab.cpp
   tabs/teamTab.cpp
   tabs/LogsTab.cpp
   tabs/LogTab.cpp
   tabs/sensorTab.cpp
   tabs/graphTab.cpp
   tabs/walkTab.cpp
   tabs/visionTab.cpp
   tabs/localisationTab.cpp
   tabs/jointsTab.cpp
   tabs/zmpTab.cpp
   tabs/fieldView.cpp
   tabs/cameraPoseTab.cpp
   tabs/calibrationTab.cpp
   tabs/tab.cpp
   tabs/surfTab.cpp
   tabs/icpTab.cpp
   tabs/icpFieldView.cpp
   tabs/controlTab.cpp
)

SET(OFFNAO_MOC
   readers/reader.hpp 
   readers/dumpReader.hpp
   readers/recordReader.hpp
   readers/bbdReader.hpp
   mediaPanel.hpp
   visualiser.hpp
   tabs/tab.hpp
   tabs/calibrationTab.hpp
   tabs/cameraPoseTab.hpp
   tabs/graphTab.hpp
   tabs/walkTab.hpp
   tabs/jointsTab.hpp
   tabs/overviewTab.hpp
   tabs/teamTab.hpp
   tabs/LogsTab.hpp
   tabs/LogTab.hpp
   tabs/plots.hpp
   tabs/sensorTab.hpp
   tabs/visionTab.hpp
   tabs/localisationTab.hpp
   tabs/zmpTab.hpp
   tabs/controlTab.hpp
   tabs/surfTab.hpp
   tabs/icpTab.hpp
)

if(CMAKE_TOOLCHAIN_FILE)
  list(APPEND OFFNAO_CXX_SRCS tabs/cameraTab.cpp)
  list(APPEND OFFNAO_MOC      tabs/cameraTab.hpp)
endif(CMAKE_TOOLCHAIN_FILE)

SET(OFFNAO_UI
   visualiser.ui
   ConnectionBar.ui
   tabs/LogTab.ui
   tabs/LogsTab.ui
)

SET(OFFNAO_RES
   resources/visualiser_resources.qrc
)

# build cxx files for resources
QT4_ADD_RESOURCES(OFFNAO_RES_SRCS ${OFFNAO_RES})
 
# build ui_XXX files from the XML-style .ui files
QT4_WRAP_UI(OFFNAO_UI_SRCS ${OFFNAO_UI})

# this moc's the above variable and appends to the cxx sources
QT4_WRAP_CPP(OFFNAO_MOC_SRCS ${OFFNAO_MOC})

ADD_EXECUTABLE(offnao.bin ${OFFNAO_CXX_SRCS} ${OFFNAO_RES_SRCS} ${OFFNAO_MOC_SRCS} ${OFFNAO_UI_SRCS})
cotire(offnao.bin)

set_source_files_properties(
   tabs/graphTab.cpp
   tabs/plots.cpp
   tabs/walkTab.cpp
   tabs/zmpTab.cpp
   main.cpp
   visualiser.cpp
   ${OFFNAO_MOC_SRCS} #too lazy to split and list them
   PROPERTIES COMPILE_FLAGS "-I${QWT_INCLUDE_DIR}")

find_library ( QGLVIEWER_LIBRARY NAMES QGLViewer qglviewer-qt4 )
find_package ( OpenGL  REQUIRED )
find_package ( PNG     REQUIRED )

TARGET_LINK_LIBRARIES(offnao.bin ${QT_LIBRARIES} ${QWT_LIBRARY} ${QGLVIEWER_LIBRARY} ${Boost_IOSTREAMS_LIBRARY} soccer ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${PNG_LIBRARIES})

# unadulterated cmake rpath: $CTC_DIR/sysroot/usr/lib/qt4:$CTC_DIR/sysroot/usr/lib:build-release/robot
set_target_properties(offnao.bin PROPERTIES 
   BUILD_WITH_INSTALL_RPATH FALSE
   INSTALL_RPATH ""
   INSTALL_RPATH_USE_LINK_PATH FALSE
   SKIP_BUILD_RPATH FALSE
)

configure_file(offnao-wrapper.sh offnao @ONLY)
configure_file(goffnao-wrapper.sh goffnao @ONLY)
