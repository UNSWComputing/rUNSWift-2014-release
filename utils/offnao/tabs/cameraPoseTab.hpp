#pragma once

#include <QMenuBar>
#include <QWidget>
#include <QObject>
#include <QEvent>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QLabel>
#include <QCheckBox>
#include <QTabWidget>
#include <cstdio>
#include <vector>
#include <string>
#include "tabs/tab.hpp"
#include "mediaPanel.hpp"

#include "../../../robot/perception/kinematics/Kinematics.hpp"
#include "../../../robot/perception/kinematics/Pose.hpp"


/*
 * This contains the camera pose/mount debuging tab.
 */
class CameraPoseTab : public Tab {
   Q_OBJECT
   public:
      CameraPoseTab(QTabWidget *parent, QMenuBar *menuBar, Vision *vision);

   private:
      void init();
      void initMenu(QMenuBar *menuBar);

      QGridLayout *layout;
      QGridLayout *optionsLayout;

      QImage lastRendering;

      QPixmap imagePixmap;
      QLabel *camLabel;
      QTextEdit *calibrationOutputBox;
      QLineEdit *offsetYawTopLabel;
      QLineEdit *offsetPitchTopLabel;
      QLineEdit *offsetRollTopLabel;

      QLineEdit *offsetYawBottomLabel;
      QLineEdit *offsetPitchBottomLabel;
      QLineEdit *offsetRollBottomLabel;

      QLineEdit *offsetBodyPitchLabel;

      QRadioButton *whichCamera2;
      QRadioButton *whichCamera3;
      QCheckBox *isCalibrating;


      float currentOffsetX;
      float currentOffsetY;
      float currentBodyPitchOffset;


      std::string createCommandString(std::string argument, std::string message);
      /* Re-draw the image box from current frame. */
      void redraw();

      /*  Draw the image on top of a pixmap */
      void drawImage(QImage *image);

      /* Draw the overlays on top of that pixmap  */
      void drawOverlays(QPixmap *pixmap);

      Kinematics kinematics;

      boost::numeric::ublas::matrix<float> createPoint(float a, float b,
                                                       float c);

      std::vector<boost::numeric::ublas::matrix<float> > fieldLines;
      Blackboard *blackboard;
      Colour topSaliency[TOP_SALIENCY_COLS][TOP_SALIENCY_ROWS];
      Colour botSaliency[BOT_SALIENCY_COLS][BOT_SALIENCY_ROWS];

      QString instructionString;
   public slots:
      void newNaoData(NaoData *naoData);
      void redrawSlot();
      void incOffset();
      void decOffset();
      void updateOffset();
      void isCalibratingSlot();
   signals:
      void sendCommandToRobot(QString item);
};


