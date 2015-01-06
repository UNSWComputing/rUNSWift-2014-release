#include <QMenu>
#include <QMenuBar>
#include <QDebug>

#include <iostream>
#include <sstream>
#include "graphTab.hpp"
#include "../../robot/utils/basic_maths.hpp"
#include "../../robot/utils/body.hpp"
#include "../../robot/perception/vision/yuv.hpp"
#include "../../robot/perception/vision/rle.hpp"
#include "blackboard/Blackboard.hpp"
#include "perception/vision/Vision.hpp"


using namespace std;

GraphTab::GraphTab(QTabWidget *parent, QMenuBar *menuBar,
      Vision *vision)  {
   initMenu(menuBar);
   init();
   this->vision = vision;
   this->parent = parent;
}

void GraphTab::initMenu(QMenuBar *) {
}

void GraphTab::init() {
  
   layout = new QGridLayout(this);
   setLayout(layout);
   layout->setAlignment(layout, Qt::AlignTop);
   layout->setHorizontalSpacing(5);
   layout->setVerticalSpacing(5);

   last_frame = -1;

   acc_plot = (DataPlot*) new AccPlot(this);
   fsr_plot = (DataPlot*) new FsrPlot(this);
   tilt_plot = (DataPlot*) new TiltPlot(this);
   odom_plot = new OdomPlot(this);
   charge_plot = (DataPlot*) new ChargePlot(this);
   current_plot = (DataPlot*) new CurrentPlot(this);

   layout->addWidget(acc_plot, 0, 0);
   layout->addWidget(fsr_plot, 0, 1);
   layout->addWidget(tilt_plot, 0, 2);
   layout->addWidget(odom_plot, 1, 0);
   layout->addWidget(charge_plot, 1, 1);
   layout->addWidget(current_plot, 1, 2);
   
}

void GraphTab::newNaoData(NaoData *naoData) {

   // if (parent->currentIndex() == parent->indexOf(this)) return;

   if (!naoData || !naoData->getCurrentFrame().blackboard) {
      // clean up display, as read is finished
   } else if (naoData->getFramesTotal() != 0) {
      int new_frame = naoData->getCurrentFrameIndex();
      if (new_frame == last_frame + 1) {
         // special case for one frame at a time
         Blackboard *blackboard = (naoData->getCurrentFrame().blackboard);
         SensorValues s = readFrom(motion, sensors);
         Odometry vision = readFrom(vision, vOdometry);
         Odometry motion = readFrom(motion, odometry);
         Odometry combined = readFrom(vision, dualOdometry);

         acc_plot->push(s);
         fsr_plot->push(s);
         tilt_plot->push(s);
         odom_plot->push(vision, motion, combined);
         charge_plot->push(s);
         current_plot->push(s);
      } else if (new_frame == last_frame - 1) {
         // special case for one frame at a time
         /*
         Blackboard *blackboard = (naoData->getFrame(last_frame - PLOT_SIZE).blackboard);
         SensorValues s = readFrom(motion, sensors);
         acc_plot->push(s, true);
         fsr_plot->push(s, true);
         tilt_plot->push(s, true);
         sonar_plot->push(s, true);
         charge_plot->push(s, true);
         current_plot->push(s, true);
         */
      } else if (ABS(new_frame - last_frame) > PLOT_SIZE) {
         // scrap all data and pass in new array
         std::vector<SensorValues> s;
         SensorValues null;
         if (new_frame < PLOT_SIZE - 1)
            for (uint8_t i = 0; i < Sensors::NUMBER_OF_SENSORS; ++i)
               null.sensors[i] = 0.0f;
         for (int i = new_frame - PLOT_SIZE + 1; i <= new_frame; ++i) {
            if (i >= 0)
               s.push_back(naoData->getFrame(i).blackboard->motion.sensors);
            else
               s.push_back(null);
         }
         
         Blackboard *blackboard = (naoData->getCurrentFrame().blackboard);
         Odometry vision = readFrom(vision, vOdometry);
         Odometry motion = readFrom(motion, odometry);
         Odometry combined = readFrom(vision, dualOdometry);
         
         acc_plot->push(s);
         fsr_plot->push(s);
         tilt_plot->push(s);
         odom_plot->push(vision, motion, combined);
         charge_plot->push(s);
         current_plot->push(s);
      } else if (new_frame < last_frame) {
         // push some new data to the front of graph
         std::vector<SensorValues> s;
         SensorValues null;
         if (new_frame < PLOT_SIZE - 1)
            for (uint8_t i = 0; i < Sensors::NUMBER_OF_SENSORS; ++i)
               null.sensors[i] = 0.0f;
         for (int i = new_frame - PLOT_SIZE + 1; i <= last_frame - PLOT_SIZE; ++i) {
            if (i < 0)
               s.push_back(null);
            else
               s.push_back(naoData->getFrame(i).blackboard->motion.sensors);
         }
   
         
         Blackboard *blackboard = (naoData->getCurrentFrame().blackboard);
         Odometry vision = readFrom(vision, vOdometry);
         Odometry motion = readFrom(motion, odometry);
         Odometry combined = readFrom(vision, dualOdometry);
         
         acc_plot->push(s, true);
         fsr_plot->push(s, true);
         tilt_plot->push(s, true);
         odom_plot->push(vision, motion, combined, true);
         charge_plot->push(s, true);
         current_plot->push(s, true);
      } else if (new_frame > last_frame) {
         // push some new data to the end of graph
         Blackboard *blackboard = (naoData->getCurrentFrame().blackboard);
         
         std::vector<SensorValues> s;
         std::vector<Odometry> vision;
         std::vector<Odometry> motion;
         std::vector<Odometry> combined;
         for (int i = last_frame + 1; i <= new_frame; ++i) {
            s.push_back(naoData->getFrame(i).blackboard->motion.sensors);
            vision.push_back(naoData->getFrame(i).blackboard->vision.vOdometry);
            motion.push_back(naoData->getFrame(i).blackboard->motion.odometry);
            combined.push_back(naoData->getFrame(i).blackboard->vision.dualOdometry);
         }
         acc_plot->push(s);
         fsr_plot->push(s);
         tilt_plot->push(s);
         odom_plot->push(vision, motion, combined);
         charge_plot->push(s);
         current_plot->push(s);
      }
      last_frame = new_frame;
   }
}

void GraphTab::readerClosed() {
}
