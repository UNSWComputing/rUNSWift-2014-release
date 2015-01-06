
#pragma once

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QGridLayout>
#include <QSlider>
#include <QToolButton>
#include <QAction>

#include "naoData.hpp"

/*
 * This holds the bar at the bottom with all of the play/stop/pause buttons etc. Just a convienence class to keep the variables together.
 */
class MediaPanel : public QWidget {
   Q_OBJECT
   public slots:
      void newNaoData(NaoData *naoData);
      void togglePausePlay(QAction* action);
      void toggleRecordStop(QAction* action);

   public:
      explicit MediaPanel(QWidget *parent = 0);

      QToolButton *recordButton;
      QToolButton *pauseButton;
      QToolButton *stopButton;
      QToolButton *playButton;
      QToolButton *backButton;
      QToolButton *forwardButton;


      QSlider *frameSlider;



      QAction *recordAct;
      QAction *pauseAct;
      QAction *stopAct;
      QAction *playAct;
      QAction *backAct;
      QAction *forwardAct;

      void setIsPlay(bool b);
      void setIsRecord(bool b);
   private:
      QGridLayout *layout;
      static const int BUTTON_HEIGHT = 30;
      static const int BUTTON_WIDTH = 39;
      bool isPlay;
      bool isRecord;
};
