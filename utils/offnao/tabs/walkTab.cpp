#include <QMenu>
#include <QMenuBar>
#include <QDebug>
#include <QString>

#include <iostream>
#include <sstream>
#include "walkTab.hpp"
#include "../../robot/utils/basic_maths.hpp"
#include "../../robot/utils/body.hpp"
#include "../../robot/perception/vision/yuv.hpp"
#include "../../robot/perception/vision/rle.hpp"
#include "../../robot/motion/generator/PendulumModel.hpp"
#include "../../robot/motion/touch/FilteredTouch.hpp"
#include "blackboard/Blackboard.hpp"
#include "perception/vision/Vision.hpp"

using namespace std;

#define X_LEN 30
#define Y_LEN 100
const unsigned int WalkTab::numEntries = 5;

#if QWT_VERSION >= 0x060000
#define setRawData setRawSamples
#endif

WalkTab::WalkTab(QTabWidget *parent, QMenuBar *menuBar,
      Vision *vision)  {
   initMenu(menuBar);
   this->vision = vision;
   this->parent = parent;
   lastCurrent = 0;
   bodyPlot[0] = new QwtPlot(QString("X - Theta && dTheta"), parent);
   bodyPlot[1] = new QwtPlot(QString("Y - Theta && dTheta"), parent);
   const QString title("Graph 1");
   // curve = bodyPlot->insertCurve(title);
   (bodyPlot[0])->setAxisScale(QwtPlot::xBottom, -X_LEN/2, X_LEN/2);
   (bodyPlot[0])->setAxisScale(QwtPlot::yLeft, -Y_LEN/2, Y_LEN/2);
   (bodyPlot[1])->setAxisScale(QwtPlot::xBottom, -X_LEN/2, X_LEN/2);
   (bodyPlot[1])->setAxisScale(QwtPlot::yLeft, -Y_LEN/2, Y_LEN/2);


   axis[0][0] = new QwtPlotCurve("XaxisTheta");
   (axis[0][0])->attach(bodyPlot[0]);
   (axis[0][0])->setPen(QPen(Qt::black));
   axis[0][1] = new QwtPlotCurve("XaxisdTheta");
   (axis[0][1])->attach(bodyPlot[0]);
   (axis[0][1])->setPen(QPen(Qt::black));

   axis[1][0] = new QwtPlotCurve("YaxisTheta");
   (axis[1][0])->attach(bodyPlot[1]);
   (axis[1][0])->setPen(QPen(Qt::black));
   axis[1][1] = new QwtPlotCurve("YaxisdTheta");
   (axis[1][1])->attach(bodyPlot[1]);
   (axis[1][1])->setPen(QPen(Qt::black));

   curve[0] = new QwtPlotCurve("xData");
   (curve[0])->attach(bodyPlot[0]);
   (curve[0])->setPen(QPen(Qt::red));
   curve[1] = new QwtPlotCurve("yData");
   (curve[1])->attach(bodyPlot[1]);
   (curve[1])->setPen(QPen(Qt::red));

   init();
}

void WalkTab::initMenu(QMenuBar *) {
}

void WalkTab::init() {
   layout = new QGridLayout(this);
   setLayout(layout);
   layout->setAlignment(layout, Qt::AlignTop);
   layout->setHorizontalSpacing(5);
   layout->setVerticalSpacing(5);

   layout->addWidget(bodyPlot[0], 0, 0);
   layout->addWidget(bodyPlot[1], 0, 1);
   for(int i = 0; i < 100; i++){
      line[i] = i - 50;
      zero[i] = 0;
   }

   for(int i = 0; i < 2; i++){
      (axis[i][0])->setRawData(line, zero, 100);
      (axis[i][1])->setRawData(zero, line, 100);
   }

}

void WalkTab::newNaoData(NaoData *naoData) {
   Blackboard *blackboard;
   if (naoData && (blackboard = naoData->getCurrentFrame().blackboard)) {
//      PendulumModel pendulumModel = readFrom(motion, pendulumModel);
      SensorValues s = readFrom(motion, sensors);
      theta[0].push_back(RAD2DEG(s.sensors[Sensors::InertialSensor_AngleX]));
      theta[1].push_back(RAD2DEG(s.sensors[Sensors::InertialSensor_AngleY]));
      dTheta[0].push_back(RAD2DEG(s.sensors[Sensors::InertialSensor_GyrX]));
      dTheta[1].push_back(RAD2DEG(s.sensors[Sensors::InertialSensor_GyrY]));
      if (theta[0].size() > numEntries) {
         theta[0].pop_front();
         theta[1].pop_front();
         dTheta[0].pop_front();
         dTheta[1].pop_front();
      }

      for(int i = 0; i < 2; i++){
         std::list<float>::iterator iter = theta[i].begin();
         int j = 0;
         while (iter != theta[i].end()) {
            atheta[i][j] = *iter;
            ++iter;
            j++;
         }

         iter = dTheta[i].begin();
         j = 0;
         while (iter != dTheta[i].end()) {
            aDTheta[i][j] = *iter;
            ++iter;
            j++;
         }
      }

      (curve[0])->setRawData(atheta[0], aDTheta[0], theta[0].size());
      (curve[1])->setRawData(atheta[1], aDTheta[1], theta[1].size());

      bodyPlot[0]->replot();
      bodyPlot[1]->replot();
   }
}

void WalkTab::readerClosed() {
}

