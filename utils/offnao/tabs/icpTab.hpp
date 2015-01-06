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
#include <QLineEdit>
#include <QRadioButton>

#include <cstdio>
#include <deque>

#include "tabs/tab.hpp"
#include "tabs/variableView.hpp"
#include "perception/vision/yuv.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "perception/localisation/ICP.hpp"
#include "utils/Logger.hpp"
#include "icpFieldView.hpp"
#include "mediaPanel.hpp"

class Vision;

class Blackboard;

class ICPTab : public Tab {
   Q_OBJECT
   public:
      ICPTab(QTabWidget *parent, QMenuBar *menuBar, Vision *vision);
      QPixmap *renderPixmap;
      QLabel *renderWidget;
   private:
      void init();
      void initMenu(QMenuBar *menuBar);
      void redraw();

      /*  Draw the image on top of a pixmap */
      void drawImage(QImage *topImage, QImage *botImage);
      void drawOverlays(QPixmap *topImage, QPixmap *botImage);

      QGridLayout *layout;
      QLineEdit *trueX;
      QLineEdit *trueY;
      QLineEdit *trueTheta;
      QRadioButton *lostButton;
      QLineEdit *ballX;
      QLineEdit *ballY;
      QRadioButton *teamBallButton;
      QPushButton *initButton;
      QLabel *icpInfo;

      /* These variables are used to present the debug variables from the nao*/
      //VariableView variableView;
      ICPFieldView fieldView;

      /* Variables for vision */
      QPixmap topImagePixmap;
      QLabel *topCamLabel;
      QPixmap botImagePixmap;
      QLabel *botCamLabel;

      // ICP stuff
      AbsCoord fixedPos;
      AbsCoord teamBall;
      bool isLost;
      bool useTeamBall;
      void drawICP();

      // Data
      Colour topSaliency[TOP_SALIENCY_COLS][TOP_SALIENCY_ROWS];
      Colour botSaliency[BOT_SALIENCY_COLS][BOT_SALIENCY_ROWS];
      Blackboard *blackboard;

   public slots:
      void newNaoData(NaoData *naoData);
      void setPos();
};

