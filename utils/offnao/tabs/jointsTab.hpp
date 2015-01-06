#pragma once
#include <QMenuBar>
#include <QWidget>
#include <QObject>
#include <QEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QTabWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>

#include <cstdio>
#include "tabs/tab.hpp"
#include "mediaPanel.hpp"


/*
 * This contains the vision debuging tab.
 */
class JointsTab : public Tab {
   Q_OBJECT
   public:
      JointsTab(QTabWidget *parent, QMenuBar *menuBar, Vision *vision);

   private:
      void init();
      void initMenu(QMenuBar *menuBar);

      enum GraphLocations {TOP_LEFT, TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT};

      // Constants
      static const int NUM_GRAPH_LOCATIONS = BOTTOM_LEFT + 1;

      QMenu *jointsMenu;

      /* objects that the debug info is drawn to */

      // Graphs
      QPixmap graphPixmaps[NUM_GRAPH_LOCATIONS];
      QLabel *graphLabels[NUM_GRAPH_LOCATIONS];

      QGridLayout *layout;

      /* Re-draw the image box from current frame. */
      void redraw();

   public slots:
      void newNaoData(NaoData *naoData);
};

