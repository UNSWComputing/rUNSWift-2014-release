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
#include <linux/videodev2.h>
#include <vector>
#include "tabs/tab.hpp"
#include "mediaPanel.hpp"
#include "tabs/PointCloud.hpp"

/*
 * This contains the vision debuging tab.
 */
class CameraTab : public Tab {
   Q_OBJECT
   public:
      CameraTab(QTabWidget *parent, QMenuBar *menuBar, Vision *vision);
      void setControls(const std::vector<struct v4l2_control> &);

   private:
      void init();
      void initMenu(QMenuBar *menuBar);

      enum ImageType {CAMERA, RGB, YUV, HSV};

      // Constants
      static const int NUM_IMG_TYPES = HSV + 1;
      static const unsigned int MAX_RGB_VALUE = 0x00ffffff;
      static const unsigned int MAX_B_VALUE = 0x000000ff;
      static const int HISTOGRAM_COLS = IMAGE_COLS/2;
      static const int RGB_SCALEFACTOR = MAX_RGB_VALUE/HISTOGRAM_COLS;
      static const int B_SCALEFACTOR = 1; // FIX THIS! morri
      static const unsigned int RED_MASK = 0x00ff0000;
      static const unsigned int GREEN_MASK = 0x0000ff00;
      static const unsigned int BLUE_MASK = 0x000000ff;

      QMenu *cameraMenu;

      // Frame counter
      int framesSinceReset;
      int prevControl;
      int prevControlValue;

      /* objects that the debug info is drawn to */

      // Camera image, RGB histogram, YUV histogram and HSV histogram
      QPixmap imagePixmaps[NUM_IMG_TYPES];
      QLabel *imageLabels[NUM_IMG_TYPES];
      PointCloud pointCloud;

      QGridLayout *layout;

      // Track best entropy value
      bool autoTuneOn;
      float bestEntropy;

      /* Re-draw the image box from current frame. */
      void redraw();
   public slots:
      void newNaoData(NaoData *naoData);
      void autoTuner(void); // Turn camera auto tuner on/off
      void tuneValue(float entropy); // Select a random camera attribute and a random value
      void printCameraControlValues(void);
   signals:
      void sendCommandToRobot(QString item);
};

/**
 * handles increment and decrement buttons
 */
class CameraButtonHandler : public QObject {
   Q_OBJECT
   public:
   /**
    * creates a button handler that modifies a textbox and sends a new
    * value to the robot
    *
    * @param camtab the CameraTab i'm called from
    * @param control the index of the control to change
    * @param qpb the button that was pressed, or textbox that was modified
    *            if NULL, uses qle as trigger
    * @param qle the textbox to modify
    */
      CameraButtonHandler(CameraTab *camtab, int control, QPushButton *qpb,
                          QLineEdit *qle);
   public slots:
   /**
    * modifies a textbox and sends a new value to the robot
    */
      void clicked(bool checked = false);
   private:
   /**
    * the CameraTab i'm called from
    */
      CameraTab *camtab;

      /**
       * the index of the control to change
       */
      int control;

      /**
       * the button that was pressed
       */
      QPushButton *qpb;

      /**
       * the textbox to modify
       */
      QLineEdit *qle;
};
