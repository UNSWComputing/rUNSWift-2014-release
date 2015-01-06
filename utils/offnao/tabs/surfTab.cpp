#include <QMenu>
#include <QMenuBar>
#include <QDebug>
#include <QBitmap>

#include <utility>
#include <iostream>
#include <sstream>
#include "surfTab.hpp"
#include "../../robot/perception/vision/yuv.hpp"
#include "../../robot/perception/vision/rle.hpp"
#include "../../robot/perception/kinematics/Pose.hpp"
#include "blackboard/Blackboard.hpp"
#include "utils/CPlaneColours.hpp"
#include "perception/vision/Vision.hpp"


#define DEBUG_IMAGE_ROWS (IMAGE_ROWS / 3)
#define DEBUG_IMAGE_COLS (IMAGE_COLS / 3)
#define SCALE 0.67f

#define VOCAB_SIZE 200
#define TOP_STOP_WORDS 0 // 0% (recommended 5%)
#define BOT_STOP_WORDS 30 // 15% (recommended 10%)

#define FRAMES_PER_LOCATION 2 // Get 2 sets of interest points at each map location


using namespace std;

SurfTab::SurfTab(QTabWidget *parent, QMenuBar *menuBar,
      Vision *vision) : blackboard(0) {
   initMenu(menuBar);

   init();
   cancelSlot();
   this->vision = vision;
   this->parent = parent;
   this->resetMotionOdom = Odometry();
   this->resetVisionOdom = Odometry();
   this->resetDualOdom = Odometry();
   this->resetMissedFrames = 0;
   this->range = 20;
   this->orientation = 180;

}


void SurfTab::initMenu(QMenuBar *) {
}

void SurfTab::keyPressEvent (QKeyEvent *event){

   int increment = 5;

   switch(event->key()){
   
      case Qt::Key_Up: 
         robotInfo[0].imageCoords.a.x() -= increment;
         robotInfo[0].imageCoords.b.x() += increment;  
      break;

      case Qt::Key_Down:
         if (robotInfo[0].imageCoords.a.x() < robotInfo[0].imageCoords.b.x() + 2* increment){
            robotInfo[0].imageCoords.a.x() += increment;
            robotInfo[0].imageCoords.b.x() -= increment;
         }
      break;

      case Qt::Key_Left:
         robotInfo[0].imageCoords.a.x() -= increment;
         robotInfo[0].imageCoords.b.x() -= increment;
      break;

      case Qt::Key_Right:
         robotInfo[0].imageCoords.a.x() += increment;
         robotInfo[0].imageCoords.b.x() += increment;
      break;

      default:
         event->ignore();
      break;

   }
   redrawImage();
   redraw();
}


void SurfTab::init() {

   // Build map entries for prerecording a field map
   for(int i=0; i<5; i++){
     for(int j=0; j<5; j++){
      
       float x = -2000.f + i*1000.f;
       float y = -2000.f + j*1000.f;

       // Facing first goal       
       AbsCoord pos = AbsCoord(x,y,atan2(-y, -3000.f - x));
       pos.var.setZero(); // since positions are known, set variance to zero
       map.push_back(MapEntry(pos));
 
       // Facing second goal
       AbsCoord pos2 = AbsCoord(x,y,atan2(-y, 3000.f - x));
       pos2.var.setZero(); // since positions are known, set variance to zero
       map.push_back(MapEntry(pos2));
     }
   }
   this->map_position = 0;
   this->frames_recorded = 0;
   this->images = 0;

   // Create an initial start position for robot bounding box
   Point a = Point(200,200);
   Point b = Point(IMAGE_COLS - 200, IMAGE_ROWS - 200);
   BBox robotBox = BBox(a,b);
   RRCoord robotlocation = RRCoord();
   this->robotInfo.push_back(RobotInfo(robotlocation, RobotInfo::rUnknown, robotBox));

   layout = new QGridLayout(this);
   setLayout(layout);
   layout->setAlignment(layout, Qt::AlignTop);
   layout->setHorizontalSpacing(5);
  
   learnVocabButton = new QPushButton("Learn Vocab"); 
   learnRobotButton = new QPushButton("Build Map of Robot Features"); 
   buildMapButton = new QPushButton("Build Map of Goal Background");
	saveButton = new QPushButton("Save to File");
   setLandmarkButton = new QPushButton("Set Image");
   clearLandmarkButton = new QPushButton("Clear");
   cancelButton = new QPushButton("Cancel");
   resetOdometryButton = new QPushButton("Reset Odometry");

   dial = new QDial(this);
   dial->setNotchesVisible(true);
   dial->setWrapping(true);
   dial->setRange(0,360);
   dial->setValue(orientation);
   dial->setPageStep(30);
   dial->setObjectName("Orientation deg");

   slider = new QSlider(this);
   slider->setOrientation(Qt::Horizontal);
   slider->setRange(10, 150);
   slider->setPageStep(10);
   slider->setValue(range);
   slider->setObjectName("Range cm");

   landmarkValueLabel = new QLabel();
   landmarkValueLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
   landmarkValueLabel->setFont(QFont("Monospace"));

   layout->addWidget(&fieldView, 0, 0, 2, 1);
   fieldView.redraw(NULL);

   imagePixmap = QPixmap(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
   imagePixmap.fill(Qt::darkGray);
   camLabel  = new QLabel();
   camLabel->setPixmap(imagePixmap);
   camLabel->setMinimumSize(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
   camLabel->setMaximumSize(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
   layout->addWidget(camLabel, 0, 1, 2, 2);

   layout->addWidget(landmarkValueLabel, 2, 1, 1, 2);

   layout->addWidget(learnRobotButton, 4, 1, 1, 1);
   layout->addWidget(buildMapButton, 4, 2, 1, 1);
   layout->addWidget(learnVocabButton, 4, 3, 1, 1);

   layout->addWidget(setLandmarkButton, 5, 1, 1, 1);
   layout->addWidget(clearLandmarkButton, 5, 2, 1, 1);
	layout->addWidget(saveButton, 5, 3, 1, 1);
   layout->addWidget(cancelButton, 6, 1, 1, 1);
   layout->addWidget(resetOdometryButton, 6, 3, 1, 1);

   layout->addWidget(slider, 3, 1, 1, 2 );
   layout->addWidget(dial,  3, 3, 1, 1);

   connect(learnRobotButton, SIGNAL(clicked(bool)),this, SLOT(learnRobotsSlot()));
   connect(buildMapButton, SIGNAL(clicked(bool)),this, SLOT(buildMapSlot()));
	connect(saveButton, SIGNAL(clicked(bool)),this, SLOT(saveSlot()));
	connect(setLandmarkButton, SIGNAL(clicked(bool)),this, SLOT(setLandmarkSlot()));
   connect(clearLandmarkButton, SIGNAL(clicked(bool)),this, SLOT(clearLandmarkSlot()));
   connect(cancelButton, SIGNAL(clicked(bool)),this, SLOT(cancelSlot()));
   connect(resetOdometryButton, SIGNAL(clicked(bool)),this, SLOT(resetOdometrySlot()));
   connect(learnVocabButton, SIGNAL(clicked(bool)),this, SLOT(learnVocabSlot()));
      
   connect(slider, SIGNAL(valueChanged(int)),this, SLOT(rangeSlot(int)));
   connect(dial, SIGNAL(valueChanged(int)),this, SLOT(orientationSlot(int)));

}

void SurfTab::rangeSlot(int range) {
   this->range = range;
   redraw();
}


void SurfTab::orientationSlot(int orientation) {
   this->orientation = orientation;
   redraw();
}


void SurfTab::resetOdometrySlot() {
  if(blackboard){
   resetVisionOdom = readFrom(vision, vOdometry);
   resetDualOdom = readFrom(vision, dualOdometry);
   resetMotionOdom = readFrom(motion, odometry);
   resetMissedFrames = readFrom(vision, missedFrames);
  }
}


void SurfTab::learnVocabSlot() {
   this->mode = mVOCAB;
   this->learnRobotButton->setEnabled(false);
   this->learnVocabButton->setEnabled(false);
   this->buildMapButton->setEnabled(false);
   this->saveButton->setEnabled(false);
   this->setLandmarkButton->setEnabled(true);
   this->clearLandmarkButton->setEnabled(false);
   this->cancelButton->setEnabled(true);

   redraw();
}

void SurfTab::buildMapSlot() {
   this->mode = mMAP;
   this->learnRobotButton->setEnabled(false);   
   this->learnVocabButton->setEnabled(false);
   this->buildMapButton->setEnabled(false);
   this->saveButton->setEnabled(false);
   this->setLandmarkButton->setEnabled(true);
   this->clearLandmarkButton->setEnabled(false);
   this->cancelButton->setEnabled(true);

   redraw();
}

void SurfTab::learnRobotsSlot() {
   this->mode = mROBOT;
   this->learnRobotButton->setEnabled(false);
   this->learnVocabButton->setEnabled(false);
   this->buildMapButton->setEnabled(false);
   this->saveButton->setEnabled(false);
   this->setLandmarkButton->setEnabled(true);
   this->clearLandmarkButton->setEnabled(false);
   this->cancelButton->setEnabled(true);

   this->setFocusPolicy(Qt::ClickFocus);
   redraw();
   redrawImage();

}

void SurfTab::setLandmarkSlot() {
  if(blackboard){
    this->saveButton->setEnabled(true);
    this->clearLandmarkButton->setEnabled(true);

    switch (mode) {

        case mVOCAB:{
          for(unsigned int i=0; i<landmarks.size(); i++) ipts.push_back(landmarks.at(i));
          images++;          
          if((int)ipts.size() < VOCAB_SIZE) this->saveButton->setEnabled(false);
          break;
          }

        case mROBOT:{
          MapEntry robotView =  MapEntry();
          // Make sure the location of the landmarks given is centred around 0 
          float robotRadius = static_cast<float>((robotInfo[0].imageCoords.b.x() - robotInfo[0].imageCoords.a.x())/SURF_SUBSAMPLE)/2.f;
          float robotCentre = static_cast<float>((robotInfo[0].imageCoords.b.x() + robotInfo[0].imageCoords.a.x())/SURF_SUBSAMPLE)/2.f;
          std::cout << "Robot centre, radius: " << robotCentre << ", " << robotRadius << "\n";
          for(unsigned int i=0; i<robotPoints.size(); i++){ 
            Ipoint ipoint = landmarks.at(i);
            ipoint.x -= robotCentre;
            robotView.ipoints.push_back(ipoint);
          }
          // Store the width/radius of the robot, for convenience just put it as y value of abscoord
          robotView.position.y() = robotRadius;
          robotView.object.distance() = range*10.f; // convert cm to mm
          robotView.object.orientation() = DEG2RAD(orientation);
          robots.push_back(robotView);
          break; 
          }  

        case mMAP:{
          for(unsigned int i=0; i<landmarks.size(); i++) map[map_position].ipoints.push_back(landmarks.at(i));
          frames_recorded++;
          if(frames_recorded >= FRAMES_PER_LOCATION){
            map_position++;
            frames_recorded = 0;
          } 
          if(map_position >= (int)map.size()){
            this->setLandmarkButton->setEnabled(false);     
          } 
          break;
          }  

        default:
           QMessageBox::warning(this, "Error",
                       "This button should have been disabled in this mode");
     }
  }
  redraw();
}

void SurfTab::clearLandmarkSlot() {

  switch (mode) {

      case mVOCAB:
        ipts.clear();
        images = 0; 
        break;

      case mROBOT:
         map.erase(map.end());

      case mMAP:
        for(int i=0; i< (int)map.size(); i++) map[i].ipoints.clear();
        map_position = 0;
        frames_recorded = 0;
        break;

      default:;
   }
   redraw();
}

void SurfTab::cancelSlot() {
   clearLandmarkSlot();
   this->mode = mVIEW;
   this->learnRobotButton->setEnabled(true);   
   this->learnVocabButton->setEnabled(true);
   this->buildMapButton->setEnabled(true);
   this->saveButton->setEnabled(false);
   this->setLandmarkButton->setEnabled(false);
   this->clearLandmarkButton->setEnabled(false);
   this->cancelButton->setEnabled(false);

   this->setFocusPolicy(Qt::NoFocus);
}


void SurfTab::saveSlot()
{
  stringstream landmarkStream;
  Eigen::VectorXf stoplist = Eigen::VectorXf::Zero(VOCAB_SIZE);
  Eigen::VectorXf ni;

  switch (mode) {

      case mVOCAB:
        landmarkStream << "Learning vocab with " << ipts.size() << " points\n";
        landmarkValueLabel->setText(QString(landmarkStream.str().c_str()));
        vocab.learn(ipts, VOCAB_SIZE);

        // Get term frequencies
				ni = vocab.mapToVec(ipts);			
				int row, col;

				for(int i=0; i<TOP_STOP_WORDS; i++){ // most frequent words
					ni.maxCoeff(&row, &col);
					stoplist[row] = 1.f;
					ni[row] = -1.f;
				}
			
				for(int i=0; i<VOCAB_SIZE; i++){
					if(ni[i] < 0.f) ni[i] = std::numeric_limits<float>::max();
				}
				for(int i=0; i<BOT_STOP_WORDS; i++){ // least frequent words
					ni.minCoeff(&row, &col);
					stoplist[row] = 1.f;
					ni[row] = std::numeric_limits<float>::max();
				}
				vocab.setStopWords(stoplist);

				// Save and test load
        vocab.saveVocabFile("../../../image/home/nao/data/words.vocab");
        vocab.loadVocabFile("../../../image/home/nao/data/words.vocab");
     
        cancelSlot(); 
        break;

      case mROBOT: {
         std::ofstream ofs("../../../image/home/nao/data/robots.map");       
        {
        boost::archive::text_oarchive oa(ofs);
        oa << robots;
        }
        cancelSlot();
        break;
        }

      case mMAP: {
 
        std::ofstream ofs("../../../image/home/nao/data/goals.map");       
        {
        boost::archive::text_oarchive oa(ofs);
        oa << map;
        }
        cancelSlot();
        break;
        }
      default:
         QMessageBox::warning(this, "Error",
                     "This button should have been disabled in this mode");
   }

};


void SurfTab::redraw() {

  stringstream landmarkStream;
  if(blackboard){
      landmarks = readFrom(vision, landmarks);
      robotPoints.clear();
      for(int i=0; i<(int)landmarks.size(); i++){
         if(landmarks[i].x*SURF_SUBSAMPLE >= robotInfo[0].imageCoords.a.x() &&
            landmarks[i].x*SURF_SUBSAMPLE <= robotInfo[0].imageCoords.b.x() )
         robotPoints.push_back(landmarks[i]);
      } 
   }

  switch (mode) {

    case mVIEW:
      if(blackboard){

        // First print the visual odometry
        Odometry vOdom = readFrom(vision, vOdometry) - resetVisionOdom; 
        Odometry mOdom = readFrom(motion, odometry) - resetMotionOdom; 
        Odometry dOdom = readFrom(vision, dualOdometry) - resetDualOdom; 
        unsigned int missed = readFrom(vision, missedFrames) - resetMissedFrames;
        landmarkStream << "Visual Odometry turn: " << vOdom.turn*180/3.141592 << " deg\n";
        landmarkStream << "Walk Engine turn: " << mOdom.turn*180/3.141592 << " deg, left: " 
            << mOdom.left << ", forward: " << mOdom.forward << "\n";       
        landmarkStream << "Combined turn: " << dOdom.turn*180/3.141592 << " deg, left: " 
            << dOdom.left << ", forward: " << dOdom.forward << "\n";        
        landmarkStream << "Missed frames: " << missed << "\n";
        landmarkStream << "Current frame landmarks: " << landmarks.size() << "\n\n";
        landmarkValueLabel->setText(QString(landmarkStream.str().c_str()));
      } else { // just clear the text
        landmarkStream << "";
        landmarkValueLabel->setText(QString(landmarkStream.str().c_str()));
      }
    break;

    case mVOCAB:
      if(blackboard){
        landmarkStream << "Current frame landmarks: " << landmarks.size() << "\n";
      }
      landmarkStream << "Collected " << ipts.size() << " features from " << images << " images for vocab\n\n";
      landmarkValueLabel->setText(QString(landmarkStream.str().c_str()));
      fieldView.redraw(NULL);
    break;

    case mROBOT:
      landmarkStream << "Click image and use arrow keys to position box over robot\n";
      if(blackboard){
        landmarkStream << "Current frame landmarks: " << landmarks.size() << "\n";
        landmarkStream << "Robot landmarks: " << robotPoints.size() << "\n";
      } 
      landmarkStream << "Orientation: " << orientation << "deg, Range: " << range << "cm\n";
      landmarkStream << "Mapped " << robots.size() << " robot views\n";
      landmarkValueLabel->setText(QString(landmarkStream.str().c_str()));

    case mMAP:
      if(blackboard){
        landmarkStream << "Current frame landmarks: " << landmarks.size() << "\n";
      }
      landmarkStream << "Mapped " << map_position << " of " << map.size() << " field positions\n\n";
      landmarkValueLabel->setText(QString(landmarkStream.str().c_str()));
      // redraw the map position  
      //robotPos.push_back(map[map_position].position);
      //fieldView.redraw(robotPos, true);
    break;


    default:;
  }
}

void SurfTab::redrawImage() {
   if (topFrame || saliency) {
      QImage *image;
      bool nnmc_loaded = vision->nnmc_top.isLoaded();

      if (topFrame && ! nnmc_loaded) {
         image = new QImage(TOP_IMAGE_COLS, TOP_IMAGE_ROWS, QImage::Format_RGB32);
      } else {
         image = new QImage(TOP_SALIENCY_COLS,
                            TOP_SALIENCY_ROWS,
                            QImage::Format_RGB32);
      }
      drawImage(image);
      
      imagePixmap = QPixmap::fromImage(
               image->scaled(2*DEBUG_IMAGE_COLS, 2*DEBUG_IMAGE_ROWS));


      // Draw surf landmark overlay
      if(blackboard){
        landmarks = readFrom(vision, landmarks);
        robotPoints.clear();
        for(int i=0; i<(int)landmarks.size(); i++){
            if(landmarks[i].x*SURF_SUBSAMPLE >= robotInfo[0].imageCoords.a.x() &&
               landmarks[i].x*SURF_SUBSAMPLE <= robotInfo[0].imageCoords.b.x() )
            robotPoints.push_back(landmarks[i]);
        } 


        std::pair<int, int> horizon = readFrom(motion, pose).getHorizon();

        if (mode == mROBOT){

            robotInfo[0].imageCoords.a.y() = horizon.first - SURF_HORIZON_WIDTH/2;
            robotInfo[0].imageCoords.b.y() = horizon.second + SURF_HORIZON_WIDTH/2;

            drawOverlaysGeneric (&imagePixmap,
                        NULL,
                        &horizon,
                        NULL,
                        NULL,
                        NULL,
                        &robotInfo,
                        NULL,
                        NULL,
                        &robotPoints,
                        SCALE
                       );  
         } else {
            drawOverlaysGeneric (&imagePixmap,
                        NULL,
                        &horizon,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        &landmarks,
                        SCALE
                       );
         }   
      }
      imagePixmap = imagePixmap.scaled(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);

      delete image;
   } else {
      imagePixmap = QPixmap(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
      imagePixmap.fill(Qt::darkRed);
   }
   camLabel->setPixmap(imagePixmap);
}


void SurfTab::drawImage(QImage *image) {
   if (topFrame) {
      bool nnmc_loaded = vision->nnmc_top.isLoaded();

      if (nnmc_loaded) {
         vision->topFrame = topFrame;
         vision->processFrame();

         for (int row = 0; row < TOP_SALIENCY_ROWS; ++row) {
            for (int col = 0; col < TOP_SALIENCY_COLS; ++col) {
               image->setPixel(col, row,
                  CPLANE_COLOURS[vision->topSaliency.asFovea().colour(col, row)].rgb());
            }
         }
      } else {
         // display normal image
         for (unsigned int row = 0; row < IMAGE_ROWS; ++row) {
            for (unsigned int col = 0; col < IMAGE_COLS; ++col) {
               image->setPixel(col, row,
                   getRGB(col, row, topFrame, TOP_IMAGE_COLS));
            }
         }
      }
   } else {
      for (unsigned int row = 0; row <IMAGE_ROWS/TOP_SALIENCY_DENSITY; ++row) {
         for (unsigned int col=0; col<IMAGE_COLS/TOP_SALIENCY_DENSITY; ++col) {
            if (saliency[col][row] < 0 || saliency[col][row] > 9) {
               std::cerr << "Bad pixel at " << col << " " << row << std::endl;
            } else {
               image->setPixel(col, row,
               CPLANE_COLOURS[saliency[col][row]].rgb());
            }
         }
      }
   }
}



// TODO(brockw): see if this can be genericized into tab.cpp, so it's not in
// every tab
void SurfTab::newNaoData(NaoData *naoData) {

  if (!naoData || !naoData->getCurrentFrame().blackboard) {  // clean up display, as read is finished
      imagePixmap.fill(Qt::darkGray);
      camLabel->setPixmap(imagePixmap);
   } else if (naoData->getFramesTotal() != 0) {
      blackboard = (naoData->getCurrentFrame().blackboard);
      topFrame = readFrom(vision, topFrame);
      if (!topFrame) {
         if (readFrom(vision, topSaliency))
            memcpy(saliency, readFrom(vision, topSaliency),
                   TOP_SALIENCY_ROWS*TOP_SALIENCY_COLS*sizeof(Colour));
      }
      if (parent->currentIndex() == parent->indexOf(this)) {
         redraw();
         redrawImage();
      }
   }

}

