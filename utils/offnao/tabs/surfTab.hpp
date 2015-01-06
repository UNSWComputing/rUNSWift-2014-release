#pragma once

#include <QTabWidget>
#include <QMenuBar>
#include <QWidget>
#include <QObject>
#include <QEvent>
#include <QGridLayout>
#include <QPixmap>
#include <QLabel>
#include <QPainter>
#include <QColor>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QPushButton>
#include <QString>
#include <QInputDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDial>
#include <QSlider>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <deque>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>


#include "tabs/tab.hpp"
#include "types/MapEntry.hpp"
#include "types/RobotInfo.hpp"
#include "perception/vision/yuv.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "perception/vision/VisionConstants.hpp"
#include "perception/vision/Vocab.hpp"
#include "utils/Logger.hpp"
#include "fieldView.hpp"
#include "mediaPanel.hpp"



class Vision;

class Blackboard;

/*
 * This is tab for seeing more detail of surf localisation and for 
 * calibrating new map and vocab files
 */


class SurfTab : public Tab {
   Q_OBJECT
   public:
      SurfTab(QTabWidget *parent, QMenuBar *menuBar, Vision *vision);

      void keyPressEvent(QKeyEvent *event);

   private:
      void init();
      void initMenu(QMenuBar *menuBar);
      void redraw();  
      void redrawImage();
      void drawImage(QImage *image); 

      typedef enum {
         mVIEW = 0,
         mVOCAB = 1,
         mMAP = 2,
         mROBOT = 3
      } Mode;    

      Mode mode; // viewing stream, learning vocab or building map

      QGridLayout *layout;

      QPushButton *learnVocabButton;  
      QPushButton *learnRobotButton;   
      QPushButton *buildMapButton;      
		QPushButton *saveButton;
      QPushButton *setLandmarkButton;
      QPushButton *clearLandmarkButton;
      QPushButton *cancelButton;

      QPushButton *resetOdometryButton;

      QLabel *landmarkValueLabel;
      FieldView fieldView;

      QDial *dial;
      QSlider *slider;

      // Variables for camera image
      QPixmap imagePixmap;
      QLabel *camLabel;

      // Data
      Blackboard *blackboard;
      Colour saliency[TOP_SALIENCY_COLS][TOP_SALIENCY_ROWS];
      std::vector<Ipoint> landmarks;

      // for showing odometry output
      Odometry resetVisionOdom;
      Odometry resetMotionOdom;
      Odometry resetDualOdom;
      unsigned int resetMissedFrames;

      // for storing a map
      std::vector<MapEntry> map;
      int map_position;
      int frames_recorded; // if we want to record more than 1 set of interest points at each location

      // for learning robots
      std::vector<MapEntry> robots;
      std::vector<RobotInfo> robotInfo;
      std::vector<Ipoint> robotPoints;
      int range;
      int orientation;

      // for learning vocab
      Vocab vocab;
      std::vector<Ipoint> ipts;
      int images;


   public slots:
      void newNaoData(NaoData *naoData);
    
      void learnVocabSlot();
      void buildMapSlot();
      void learnRobotsSlot();
      void saveSlot();
      void setLandmarkSlot();
      void clearLandmarkSlot();
      void cancelSlot();
      void resetOdometrySlot();

      void rangeSlot(int range);
      void orientationSlot(int orientation);
};

