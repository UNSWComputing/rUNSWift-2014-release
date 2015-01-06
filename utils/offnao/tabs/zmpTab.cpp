#include <QMenu>
#include <QMenuBar>
#include <QDebug>

#include <iostream>
#include <sstream>
#include "zmpTab.hpp"
#include "../../robot/utils/basic_maths.hpp"
#include "../../robot/utils/body.hpp"
#include "../../robot/perception/vision/yuv.hpp"
#include "../../robot/perception/vision/rle.hpp"
#include "blackboard/Blackboard.hpp"
#include "perception/vision/Vision.hpp"

using namespace std;

ZMPTab::ZMPTab(QTabWidget *parent, QMenuBar *menuBar,
      Vision *vision)  {
   initMenu(menuBar);
   init();
   this->vision = vision;
   this->parent = parent;
}

void ZMPTab::initMenu(QMenuBar *) {
}

void ZMPTab::init() {
   layout = new QGridLayout(this);
   setLayout(layout);
   layout->setAlignment(layout, Qt::AlignTop);
   layout->setHorizontalSpacing(5);
   layout->setVerticalSpacing(5);

   last_frame = -1;

   coronal_plot = (DataPlot*) new CoronalZMPPlot(this);
   sagittal_plot = (DataPlot*) new SagittalZMPPlot(this);
   com_x_plot = (XYZPlot*) new COMxPlot(this);
   com_y_plot = (XYZPlot*) new COMyPlot(this);

   layout->addWidget(coronal_plot, 0, 0);
   layout->addWidget(sagittal_plot, 1, 0);
   layout->addWidget(com_y_plot, 0, 1);
   layout->addWidget(com_x_plot, 1, 1);
   //layout->addWidget(diagram, 1, 0, 2, 1);
}

void ZMPTab::newNaoData(NaoData *naoData) {
   // return;
   // if (parent->currentIndex() == parent->indexOf(this)) return;

   if (!naoData || !naoData->getCurrentFrame().blackboard) {
      // clean up display, as read is finished
   } else if (naoData->getFramesTotal() != 0) {
      int new_frame = naoData->getCurrentFrameIndex();
      if (new_frame == last_frame + 1) {
         // special case for one frame at a time
         Blackboard *blackboard = (naoData->getCurrentFrame().blackboard);
         SensorValues s = readFrom(motion, sensors);
         XYZ_Coord com = readFrom(motion, com);
         coronal_plot->push(s);
         sagittal_plot->push(s);
         com_x_plot->push(com);
         com_y_plot->push(com);
      } else if (new_frame == last_frame - 1) {
         // special case for one frame at a time
         /*
         Blackboard *blackboard = (naoData->getFrame(last_frame - PLOT_SIZE).blackboard);
         SensorValues s = readFrom(motion, sensors);
         XYZ_Coord com = readFrom(motion, com);
         coronal_plot->push(s, true);
         sagittal_plot->push(s, true);
         com_x_plot->push(com);
         com_y_plot->push(com);
         */
      } else if (ABS(new_frame - last_frame) > PLOT_SIZE) {
         // scrap all data and pass in new array
         std::vector<SensorValues> s;
         SensorValues null;
         std::vector<XYZ_Coord> coms;
         XYZ_Coord temp;
         if (new_frame < PLOT_SIZE - 1) {
            for (uint8_t i = 0; i < Sensors::NUMBER_OF_SENSORS; ++i)
               null.sensors[i] = 0.0f;
            temp.x = 0;
            temp.y = 0;
            temp.z = 0;
         }
         for (int i = new_frame - PLOT_SIZE + 1; i <= new_frame; ++i) {
            if (i >= 0) {
               s.push_back(naoData->getFrame(i).blackboard->motion.sensors);
               coms.push_back(naoData->getFrame(i).blackboard->motion.com);
            } else {
               s.push_back(null);
               coms.push_back(temp);
            }
         }
         coronal_plot->push(s);
         sagittal_plot->push(s);
         com_x_plot->push(coms);
         com_y_plot->push(coms);
      } else if (new_frame < last_frame) {
         // push some new data to the front of graph
         std::vector<SensorValues> s;
         SensorValues null;
         std::vector<XYZ_Coord> coms;
         XYZ_Coord temp;
         if (new_frame < PLOT_SIZE - 1) {
            for (uint8_t i = 0; i < Sensors::NUMBER_OF_SENSORS; ++i)
               null.sensors[i] = 0.0f;
            temp.x = 0;
            temp.y = 0;
            temp.z = 0;
         }
         for (int i = new_frame - PLOT_SIZE + 1; i <= last_frame - PLOT_SIZE; ++i) {
            if (i < 0) {
               s.push_back(null);
               coms.push_back(temp);
            } else {
               s.push_back(naoData->getFrame(i).blackboard->motion.sensors);
               coms.push_back(naoData->getFrame(i).blackboard->motion.com);
            }
         }
         coronal_plot->push(s, true);
         sagittal_plot->push(s, true);
         com_x_plot->push(coms, true);
         com_y_plot->push(coms, true);
      } else if (new_frame > last_frame) {
         // push some new data to the end of graph
         std::vector<SensorValues> s;
         std::vector<XYZ_Coord> coms;
         for (int i = last_frame + 1; i <= new_frame; ++i) {
            s.push_back(naoData->getFrame(i).blackboard->motion.sensors);
            coms.push_back(naoData->getFrame(i).blackboard->motion.com);
         }
         coronal_plot->push(s);
         sagittal_plot->push(s);
         com_x_plot->push(coms);
         com_y_plot->push(coms);
      }
      last_frame = new_frame;
   }
}

void ZMPTab::readerClosed() {
}
