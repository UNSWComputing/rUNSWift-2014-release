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
#include <QLabel>

#include <cstdio>
#include <deque>

#include "tabs/tab.hpp"
#include "tabs/variableView.hpp"
#include "perception/vision/yuv.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "utils/Logger.hpp"
#include "fieldView.hpp"
#include "mediaPanel.hpp"

class Vision;

class SensorTab : public Tab {
   Q_OBJECT
   public:
      SensorTab(QTabWidget *parent, QMenuBar *menuBar, Vision *vision);
   private:
      void init();
      void initMenu(QMenuBar *menuBar);
      void updateDataView(Blackboard *blackboard);
      QGridLayout *layout;
      QLabel *jointValueLabel;
      QLabel *sensorValueLabel;
      QLabel *sonarValueLabel;
   public slots:
      void newNaoData(NaoData *naoData);
};

