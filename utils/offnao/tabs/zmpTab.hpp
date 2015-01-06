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
#include "tabs/plots.hpp"
#include "perception/vision/yuv.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "utils/Logger.hpp"
#include "fieldView.hpp"
#include "mediaPanel.hpp"

class ZMPTab : public Tab {
   Q_OBJECT
   public:
      ZMPTab(QTabWidget *parent, QMenuBar *menuBar, Vision *vision);
   private:
      void init();
      void initMenu(QMenuBar *menuBar);
      QGridLayout *layout;
      DataPlot* coronal_plot;
      DataPlot* sagittal_plot;
      XYZPlot* com_x_plot;
      XYZPlot* com_y_plot;
      XYZPlot* zmp_x_plot;
      XYZPlot* zmp_y_plot;
      int last_frame;
   public slots:
      void newNaoData(NaoData *naoData);
      void readerClosed();
};

