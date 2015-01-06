#pragma once

#include "tabs/tab.hpp"
#include "mediaPanel.hpp"
#include "utils/FieldPainter.hpp"
#include "utils/FieldObject.hpp"
#include "types/AbsCoord.hpp"

#include <vector>

#include <QLineEdit>
#include <QPaintDevice>
#include <QPushButton>
#include <QButtonGroup>

#include <boost/random.hpp>
class QLabel;
class Localiser;

/*
 * This contains the localisation debuging tab.
 */
class LocalisationTab : public Tab {
   Q_OBJECT
   public:
      LocalisationTab();
      bool eventFilter (QObject *object, QEvent *event);

   private:
      void initInterface();
      void initFieldObjects();

      AbsCoord trueRobotPos;
      void createObservation(FieldObject *obj);

      RRCoord absToRR(AbsCoord obsAbs);

      QLabel *fieldLabel;
      QImage image;
      QPixmap imagePixmap;
      QLineEdit *trueX;
      QLineEdit *trueY;
      QLineEdit *trueTheta;
      QLineEdit *varianceDistance;
      QLineEdit *varianceHeading;
      QLineEdit *varianceOrientation;
      QLineEdit *biasDistance;
      QLineEdit *biasHeading;
      QLineEdit *biasOrientation;
      QPushButton *initButton;
      QButtonGroup *teamButtonGroup;

      void redraw();
      bool team_red;
      Localiser *localiser;
      QTransform transform;
      void setTransform(int device_width, int device_height);

      std::vector<FieldObject> fieldObjects;

      bool onMouseEnter(QMouseEvent *mevent);
      bool onMouseLeave(QMouseEvent *mevent);
      bool onMouseMove(QMouseEvent *mevent);
      bool onMouseButtonPress(QMouseEvent *mevent);
      bool onMouseButtonRelease(QMouseEvent *mevent);

      bool mouseDown;
      FieldObject *mouseOverObject;

      virtual void paintEvent(QPaintEvent*);

      float zeroMeanGaussianSample(float standardDeviation);

      boost::mt19937 rng;
   public slots:
      void newNaoData(NaoData *naoData);
      void initLocalisation();
      void setTeam(int);
};

