#include <QMenu>
#include <QMenuBar>
#include <QDebug>

#include <iostream>
#include <sstream>
#include "sensorTab.hpp"
#include "../../robot/utils/basic_maths.hpp"
#include "../../robot/perception/vision/yuv.hpp"
#include "../../robot/perception/vision/rle.hpp"
#include "../../robot/types/SensorValues.hpp"
#include "blackboard/Blackboard.hpp"
#include "perception/vision/Vision.hpp"

using namespace std;

SensorTab::SensorTab(QTabWidget *parent, QMenuBar *menuBar,
      Vision *vision)  {
   initMenu(menuBar);
   init();
   this->vision = vision;
   this->parent = parent;
}


void SensorTab::initMenu(QMenuBar *) {
}

void SensorTab::init() {
   layout = new QGridLayout(this);
   setLayout(layout);
   layout->setAlignment(layout, Qt::AlignTop);
   layout->setHorizontalSpacing(5);
   layout->setHorizontalSpacing(5);

   jointValueLabel = new QLabel();
   jointValueLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
   jointValueLabel->setFont(QFont("Monospace"));

   sensorValueLabel = new QLabel();
   sensorValueLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
   sensorValueLabel->setFont(QFont("Monospace"));

   sonarValueLabel = new QLabel();
   sonarValueLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
   sonarValueLabel->setFont(QFont("Monospace"));

   updateDataView(NULL);
   layout->addWidget(jointValueLabel, 0, 0, 1, 1);
   layout->addWidget(sensorValueLabel, 0, 1, 1, 1);
   layout->addWidget(sonarValueLabel, 0, 2, 1, 1);
}

void SensorTab::updateDataView(Blackboard *blackboard) {
   stringstream jointStream;
   SensorValues s;
   // distance in m, confidence (0-1)
   std::vector< std::vector <int> > sonarFiltered; 

   if(blackboard != NULL) {
      s = readFrom(motion, sensors);
      sonarFiltered = readFrom(kinematics, sonarFiltered);
   }
   for(unsigned int i = 0;
       i < sizeof(Joints::jointCodes)/sizeof(Joints::JointCode);
       i++) {
      jointStream.width(20);
      jointStream << Joints::jointNames[i] << ": ";
      if(blackboard == NULL) {
         jointStream << "";
      } else {
         jointStream << s.joints.angles[i] << "  (" << 
                        RAD2DEG(s.joints.angles[i]) << ")";
      }
      jointStream << std::endl;
   }   
   jointValueLabel->setText(QString(jointStream.str().c_str()));
   
   stringstream sensorStream;
   for(unsigned int i = 0;
       i < sizeof(Sensors::sensorCodes)/sizeof(Sensors::SensorCode);
       i++) {
      sensorStream.width(25);
      sensorStream << Sensors::sensorNames[i] << ": ";
      if(blackboard == NULL) {
         sensorStream << "";
      } else {
         sensorStream << s.sensors[i];
      }
      sensorStream << std::endl;
   }
   sensorStream << std::endl;
   sensorStream << "LFoot total: " << s.sensors[Sensors::LFoot_FSR_FrontLeft] +
                                      s.sensors[Sensors::LFoot_FSR_FrontRight] +
                                      s.sensors[Sensors::LFoot_FSR_RearLeft] +
                                      s.sensors[Sensors::LFoot_FSR_RearRight]
                                   << std::endl;
   sensorStream << "RFoot total: " << s.sensors[Sensors::RFoot_FSR_FrontLeft]
                                    + s.sensors[Sensors::RFoot_FSR_FrontRight]
                                    + s.sensors[Sensors::RFoot_FSR_RearLeft]
                                    + s.sensors[Sensors::RFoot_FSR_RearRight]
                                   << std::endl;
   sensorStream << "Total weight: " << s.sensors[Sensors::LFoot_FSR_FrontLeft]
                                     + s.sensors[Sensors::LFoot_FSR_FrontRight]
                                     + s.sensors[Sensors::LFoot_FSR_RearLeft]
                                     + s.sensors[Sensors::LFoot_FSR_RearRight]
                                     + s.sensors[Sensors::RFoot_FSR_FrontLeft]
                                     + s.sensors[Sensors::RFoot_FSR_FrontRight]
                                     + s.sensors[Sensors::RFoot_FSR_RearLeft]
                                     + s.sensors[Sensors::RFoot_FSR_RearRight]
                                    << std::endl;
   sensorValueLabel->setText(QString(sensorStream.str().c_str()));

   stringstream sonarStream;

   sonarStream << "Sonar Filtered Left:\n";
   if (blackboard == NULL){
      sonarStream << std::endl;
   } else {
      for(unsigned int i=0; i< sonarFiltered[Sonar::LEFT].size(); i++){
         sonarStream << sonarFiltered[Sonar::LEFT][i] << ", ";
      }
      sonarStream << "\n";
   }

   sonarStream << "Sonar Filtered Middle:\n";
   if (blackboard == NULL){
      sonarStream << std::endl;
   } else {
      for(unsigned int i=0; i< sonarFiltered[Sonar::MIDDLE].size(); i++){
         sonarStream << sonarFiltered[Sonar::MIDDLE][i] << ", ";
      }
      sonarStream << "\n";
   }   

   sonarStream << "Sonar Filtered Right:\n";
   if (blackboard == NULL){
      sonarStream << std::endl;
   } else {
      for(unsigned int i=0; i< sonarFiltered[Sonar::RIGHT].size(); i++){
         sonarStream << sonarFiltered[Sonar::RIGHT][i] << ", ";
      }
      sonarStream << "\n";
   }

   for(int i = 0; i < Sonar::NUMBER_OF_READINGS; i++) {
      sonarStream.width(20);
      sonarStream << Sonar::readingNames[i] << ": ";
      if(blackboard == NULL) {
         sonarStream << "";
      } else {
         sonarStream << s.sonar[i];
      }
      sonarStream << std::endl;
   }
   sonarValueLabel->setText(QString(sonarStream.str().c_str()));
}

void SensorTab::newNaoData(NaoData *naoData) {
   if (!naoData || !naoData->getCurrentFrame().blackboard) {  // clean up display, as read is finished
      if (parent->currentIndex() == parent->indexOf(this)) {
         updateDataView(NULL);
      }
   } else if (naoData->getFramesTotal() != 0) {
      Blackboard *blackboard = (naoData->getCurrentFrame().blackboard);
      if (parent->currentIndex() == parent->indexOf(this)) {
         updateDataView(blackboard);
      }
   }
}

