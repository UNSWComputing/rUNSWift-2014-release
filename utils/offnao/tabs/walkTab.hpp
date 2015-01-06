#pragma once
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>

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
#include <list>

#include <cstdio>
#include <deque>
#include <QPainter>

#include "tabs/tab.hpp"
#include "tabs/variableView.hpp"
#include "tabs/plots.hpp"
#include "perception/vision/yuv.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "utils/Logger.hpp"
#include "fieldView.hpp"
#include "mediaPanel.hpp"


class WalkTab : public Tab {
   Q_OBJECT
   public:
      WalkTab(QTabWidget *parent, QMenuBar *menuBar, Vision *vision);
   private:
      void init();
      void initMenu(QMenuBar *menuBar);
      QGridLayout *layout;
      QwtPlot *bodyPlot[2];
      QwtPlotCurve *curve[2];
      QwtPlotCurve *axis[2][2];
      std::list<float> theta[2], dTheta[2];
      int lastCurrent;
      static const unsigned int numEntries;
      double atheta[2][5], aDTheta[2][5];
      double zero[100], line[100];

   public slots:
      void newNaoData(NaoData *naoData);
      void readerClosed();
};

