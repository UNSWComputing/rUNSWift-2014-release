#pragma once
#include <QButtonGroup>
#include <QCheckBox>
#include <QEvent>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMatrix>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>
#include <QWheelEvent>
#include <QWidget>
#include <QtNetwork/QFtp>

#include <cstdio>
#include <string>
#include <vector>

#include "mediaPanel.hpp"
#include "tabs/tab.hpp"
#include "perception/vision/yuv.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "perception/vision/Vision.hpp"

/*
 *  Tab for image classification.
 */
class ControlTab : public Tab {
   Q_OBJECT
   public:
      ControlTab(QTabWidget *parent, QMenuBar *menuBar, Vision *vision);
      QAction *loadDumpAct;
      
      void keyReleaseEvent(QKeyEvent *event);
      void keyPressEvent(QKeyEvent *event);
      
      void setNao(const QString &naoName);

   private:
      void init();
      void initMenu(QMenuBar *menuBar);

      QCheckBox *checkAutoWeight;
      QCheckBox *checkVisionOverlay;

      static const int ZOOM_LIMIT = 6;
      int zoomLevel, zoomLog;
      
      int counter;
      
      void sendBehaviour();
      void stopBehaviour();
      
      QLabel *labelWithText(const char *text, bool isBold);
      QLabel *labelWithText(const char *text);
      std::vector<QLabel*> labels;
      QLabel *curPowerLabel, *commandsIssuedLabel;
      
      Colour saliencyr[160][120];
      
      int curPower, commandsIssued;
      
      std::string curNao;

      BehaviourRequest br;
      
      typedef enum {
         oNONE = 0,
         oSELECTED = 1,
         oSELECTED_UNCLASSIFIED = 2,
         oALL = 3,
         oALL_UNCLASSIFIED = 4,
         oNUM_OVERLAYS = 5
      } Overlay;

      // Access to widgets in higher-levels of the app
      // (is this dodgy, or the 'Qt Way?')
      // Ui::Visualiser *ui;

      // Image
      QLabel *camLabel;
      QPixmap imagePixmap;
   
      // Buttons
      QPushButton *undo;


      // Option Boxes
      QVBoxLayout *checkboxLayout;

      QButtonGroup *colourGroup;
      QVBoxLayout *colourGroupLayout;

      QButtonGroup *overlayGroup;
      QVBoxLayout *overlayGroupLayout;

      QGroupBox *colourBox;
      QRadioButton *colours[cNUM_COLOURS];
      QGroupBox *overlayBox;
      QRadioButton *overlays[oNUM_OVERLAYS];

      // Layout
      QGridLayout *layout;
      
      QLabel *leftLabel, *rightLabel, *middleLabel;

      // Data
      Colour colour;
      Overlay overlay;
      FILE *dumpFile;
      // uint8_t *currentFrame;

      // libsoccer utilities, for debugging vision
      // mouse overlay vars
      QPoint mousePosition;
      // Methods:
      // Re-draw the image box from current frame.
      void redraw();
      // Draw the image on top of a pixmap
      void drawImage(QImage *image);
      QPixmap drawCrosshair();
      QPoint translateToZoomedImageCoords(QPoint);
      
      int mouseX, mouseY;
      int prevMouseX, prevMouseY, prevZoomLevel;
      
      // Menu stuff
      QMenu *controlMenu;

      QAction *controlOnAct;
      
      Blackboard *blackboard;

      QFtp *ftp;
      QFile *nnmcFile;
      QProgressDialog *progressDialog;
      QAction *undoAct;
      NaoData *naoData;
      bool underControl;
      
      public slots:
         // attached to the buttons/checkboxes of the same name
         void controlOn();
         void newNaoData(NaoData *naoData);
};
