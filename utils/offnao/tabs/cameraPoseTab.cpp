#include <QMenu>
#include <QMenuBar>
#include <qpushbutton.h>
#include <QFileDialog>
#include <QDebug>
#include <QPainter>
#include <QLine>
#include <QLabel>
#include <QPoint>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QTextEdit>
#include <vector>
#include <iostream>
#include <utility>
#include <sstream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "cameraPoseTab.hpp"
#include "utils/matrix_helpers.hpp"
#include "../../robot/perception/vision/yuv.hpp"
#include "../../robot/utils/basic_maths.hpp"
#include "../../robot/utils/SPLDefs.hpp"
#include "../../robot/perception/behaviour/KinematicsCalibrationSkill.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "blackboard/Blackboard.hpp"

#include "utils/CPlaneColours.hpp"

using namespace std;
using namespace boost::numeric::ublas;

CameraPoseTab::CameraPoseTab(QTabWidget *parent,
      QMenuBar *menuBar, Vision *vision) {
   initMenu(menuBar);
   init();
   this->vision = vision;
   this->parent = parent;

   // set up vector of lines
   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2, FIELD_WIDTH/2, 0));
   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2, -FIELD_WIDTH/2, 0));

   fieldLines.push_back(createPoint(-FIELD_LINE_WIDTH/2, FIELD_WIDTH/2, 0));
   fieldLines.push_back(createPoint(-FIELD_LINE_WIDTH/2, -FIELD_WIDTH/2, 0));

   fieldLines.push_back(createPoint(FIELD_LENGTH/2 - GOAL_BOX_LENGTH +
            FIELD_LINE_WIDTH/2, GOAL_BOX_WIDTH/2, 0));
   fieldLines.push_back(createPoint(FIELD_LENGTH/2 - GOAL_BOX_LENGTH +
            FIELD_LINE_WIDTH/2, -GOAL_BOX_WIDTH/2, 0));

   fieldLines.push_back(createPoint(FIELD_LENGTH/2 - GOAL_BOX_LENGTH -
            FIELD_LINE_WIDTH/2, GOAL_BOX_WIDTH/2, 0));
   fieldLines.push_back(createPoint(FIELD_LENGTH/2 - GOAL_BOX_LENGTH -
            FIELD_LINE_WIDTH/2, -GOAL_BOX_WIDTH/2, 0));
   // lines that connect the goal box to the edge of field
   fieldLines.push_back(createPoint(FIELD_LENGTH/2 - GOAL_BOX_LENGTH,
            GOAL_BOX_WIDTH/2 + FIELD_LINE_WIDTH/2, 0));
   fieldLines.push_back(createPoint(FIELD_LENGTH/2,
            GOAL_BOX_WIDTH/2 + FIELD_LINE_WIDTH/2, 0));

   fieldLines.push_back(createPoint(FIELD_LENGTH/2 - GOAL_BOX_LENGTH,
            GOAL_BOX_WIDTH/2 - FIELD_LINE_WIDTH/2, 0));
   fieldLines.push_back(createPoint(FIELD_LENGTH/2,
            GOAL_BOX_WIDTH/2 - FIELD_LINE_WIDTH/2, 0));


   fieldLines.push_back(createPoint(FIELD_LENGTH/2 - GOAL_BOX_LENGTH,
            -GOAL_BOX_WIDTH/2 + FIELD_LINE_WIDTH/2, 0));
   fieldLines.push_back(createPoint(FIELD_LENGTH/2,
            -GOAL_BOX_WIDTH/2 + FIELD_LINE_WIDTH/2, 0));

   fieldLines.push_back(createPoint(FIELD_LENGTH/2 - GOAL_BOX_LENGTH,
            -GOAL_BOX_WIDTH/2 - FIELD_LINE_WIDTH/2, 0));
   fieldLines.push_back(createPoint(FIELD_LENGTH/2,
            -GOAL_BOX_WIDTH/2 - FIELD_LINE_WIDTH/2, 0));


   // other box
   fieldLines.push_back(createPoint(-FIELD_LENGTH/2 + GOAL_BOX_LENGTH +
            FIELD_LINE_WIDTH/2, GOAL_BOX_WIDTH/2, 0));
   fieldLines.push_back(createPoint(-FIELD_LENGTH/2 + GOAL_BOX_LENGTH +
            FIELD_LINE_WIDTH/2, -GOAL_BOX_WIDTH/2, 0));

   fieldLines.push_back(createPoint(-FIELD_LENGTH/2 + GOAL_BOX_LENGTH -
            FIELD_LINE_WIDTH/2, GOAL_BOX_WIDTH/2, 0));
   fieldLines.push_back(createPoint(-FIELD_LENGTH/2 + GOAL_BOX_LENGTH -
            FIELD_LINE_WIDTH/2, -GOAL_BOX_WIDTH/2, 0));


   // other box: lines that connect the goal box to the edge of field
   fieldLines.push_back(createPoint(-FIELD_LENGTH/2 + GOAL_BOX_LENGTH,
            GOAL_BOX_WIDTH/2 + FIELD_LINE_WIDTH/2, 0));
   fieldLines.push_back(createPoint(-FIELD_LENGTH/2,
            GOAL_BOX_WIDTH/2 + FIELD_LINE_WIDTH/2, 0));

   fieldLines.push_back(createPoint(-FIELD_LENGTH/2 + GOAL_BOX_LENGTH,
            GOAL_BOX_WIDTH/2 - FIELD_LINE_WIDTH/2, 0));
   fieldLines.push_back(createPoint(-FIELD_LENGTH/2,
            GOAL_BOX_WIDTH/2 - FIELD_LINE_WIDTH/2, 0));


   fieldLines.push_back(createPoint(-FIELD_LENGTH/2 + GOAL_BOX_LENGTH,
            -GOAL_BOX_WIDTH/2 + FIELD_LINE_WIDTH/2, 0));
   fieldLines.push_back(createPoint(-FIELD_LENGTH/2,
            -GOAL_BOX_WIDTH/2 + FIELD_LINE_WIDTH/2, 0));

   fieldLines.push_back(createPoint(-FIELD_LENGTH/2 + GOAL_BOX_LENGTH,
            -GOAL_BOX_WIDTH/2 - FIELD_LINE_WIDTH/2, 0));
   fieldLines.push_back(createPoint(-FIELD_LENGTH/2,
            -GOAL_BOX_WIDTH/2 - FIELD_LINE_WIDTH/2, 0));
   // middle cross
   fieldLines.push_back(createPoint(MARKER_CENTER_X - FIELD_LINE_WIDTH,
            FIELD_LINE_WIDTH/2, 0));
   fieldLines.push_back(createPoint(MARKER_CENTER_X + FIELD_LINE_WIDTH,
            FIELD_LINE_WIDTH/2, 0));

   fieldLines.push_back(createPoint(MARKER_CENTER_X - FIELD_LINE_WIDTH,
            -FIELD_LINE_WIDTH/2, 0));
   fieldLines.push_back(createPoint(MARKER_CENTER_X + FIELD_LINE_WIDTH,
            -FIELD_LINE_WIDTH/2, 0));


   fieldLines.push_back(createPoint(MARKER_CENTER_X - FIELD_LINE_WIDTH/2,
            -FIELD_LINE_WIDTH, 0));
   fieldLines.push_back(createPoint(MARKER_CENTER_X - FIELD_LINE_WIDTH/2,
            +FIELD_LINE_WIDTH, 0));

   fieldLines.push_back(createPoint(MARKER_CENTER_X + FIELD_LINE_WIDTH/2,
            -FIELD_LINE_WIDTH, 0));
   fieldLines.push_back(createPoint(MARKER_CENTER_X + FIELD_LINE_WIDTH/2,
            +FIELD_LINE_WIDTH, 0));

   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 + FIELD_LENGTH/2, FIELD_WIDTH/2, 0));
   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 + FIELD_LENGTH/2, -FIELD_WIDTH/2, 0));

   fieldLines.push_back(createPoint(-FIELD_LINE_WIDTH/2 + FIELD_LENGTH/2, FIELD_WIDTH/2, 0));
   fieldLines.push_back(createPoint(-FIELD_LINE_WIDTH/2 + FIELD_LENGTH/2, -FIELD_WIDTH/2, 0));

   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 - FIELD_LENGTH/2, FIELD_WIDTH/2, 0));
   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 - FIELD_LENGTH/2, -FIELD_WIDTH/2, 0));

   fieldLines.push_back(createPoint(-FIELD_LINE_WIDTH/2 - FIELD_LENGTH/2, FIELD_WIDTH/2, 0));
   fieldLines.push_back(createPoint(-FIELD_LINE_WIDTH/2 - FIELD_LENGTH/2, -FIELD_WIDTH/2, 0));


   //side lines
   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 + FIELD_LENGTH/2, FIELD_WIDTH/2 + FIELD_LINE_WIDTH, 0));
   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 - FIELD_LENGTH/2, FIELD_WIDTH/2 + FIELD_LINE_WIDTH, 0));
   
   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 + FIELD_LENGTH/2, FIELD_WIDTH/2 - FIELD_LINE_WIDTH, 0));
   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 - FIELD_LENGTH/2, FIELD_WIDTH/2 - FIELD_LINE_WIDTH, 0));

   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 + FIELD_LENGTH/2, -FIELD_WIDTH/2 + FIELD_LINE_WIDTH, 0));
   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 - FIELD_LENGTH/2, -FIELD_WIDTH/2 + FIELD_LINE_WIDTH, 0));
   
   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 + FIELD_LENGTH/2, -FIELD_WIDTH/2 - FIELD_LINE_WIDTH, 0));
   fieldLines.push_back(createPoint(FIELD_LINE_WIDTH/2 - FIELD_LENGTH/2, -FIELD_WIDTH/2 - FIELD_LINE_WIDTH, 0));

   // generate centre circle
   int PARTS = 20;
   for(int i = 0; i < PARTS; i++) {
      float angle = (i * 360 / PARTS) * M_PI/180.0;
      float n_angle = ((i+1) * 360 / PARTS) * M_PI/180.0;

      int sx = cos(angle) * (CENTER_CIRCLE_DIAMETER/2.0 +
            FIELD_LINE_WIDTH/2);
      int sy = sin(angle) * (CENTER_CIRCLE_DIAMETER/2.0 +
            FIELD_LINE_WIDTH/2);

      int nx = cos(n_angle) * (CENTER_CIRCLE_DIAMETER/2.0 +
            FIELD_LINE_WIDTH/2);
      int ny = sin(n_angle) * (CENTER_CIRCLE_DIAMETER/2.0 +
            FIELD_LINE_WIDTH/2);
      fieldLines.push_back(createPoint(sx, sy, 0));
      fieldLines.push_back(createPoint(nx, ny, 0));

      sx = cos(angle) * (CENTER_CIRCLE_DIAMETER/2.0 -
            FIELD_LINE_WIDTH/2);
      sy = sin(angle) * (CENTER_CIRCLE_DIAMETER/2.0 -
            FIELD_LINE_WIDTH/2);

      nx = cos(n_angle) * (CENTER_CIRCLE_DIAMETER/2.0 -
            FIELD_LINE_WIDTH/2);
      ny = sin(n_angle) * (CENTER_CIRCLE_DIAMETER/2.0 -
            FIELD_LINE_WIDTH/2);
      fieldLines.push_back(createPoint(sx, sy, 0));
      fieldLines.push_back(createPoint(nx, ny, 0));
   }
}


void CameraPoseTab::initMenu(QMenuBar *menuBar) {
}

void CameraPoseTab::init() {
   layout = new QGridLayout();
   this->setLayout(layout);

   // We use the size of the (smaller) bottom camera image and
   // scale the (larger) top camera image down so it fits within the screen
   imagePixmap = QPixmap(BOT_IMAGE_COLS, BOT_IMAGE_ROWS);
   imagePixmap.fill(Qt::darkGray);
   camLabel  = new QLabel();
   camLabel->setPixmap(imagePixmap);

   layout->addWidget(camLabel, 0, 0, 1, 1);

   optionsLayout = new QGridLayout();
   layout->addLayout(optionsLayout, 0, 1, 1, 1, Qt::AlignTop);

   instructionString += "1) Place robot on penalty spot\n";
   instructionString += "2) Face robots feet and head towards far goal\n";
   instructionString += "3) Check image to see if field lines match\n";
   instructionString += "4) Turn the head to check more field lines\n";
   instructionString += "5) Adjust offset if they do not match\n";
   instructionString += "6) Update config file for robot when they match\n";

   QLabel *instructionsBox = new QLabel(instructionString);
   optionsLayout->addWidget(instructionsBox, 0, 0, 1, 1);

   QGridLayout *configureLayout = new QGridLayout();
   offsetYawTopLabel = new QLineEdit("0.0");
   offsetPitchTopLabel = new QLineEdit("0.0");
   offsetRollTopLabel = new QLineEdit("0.0");

   offsetYawBottomLabel = new QLineEdit("0.0");
   offsetPitchBottomLabel = new QLineEdit("0.0");
   offsetRollBottomLabel = new QLineEdit("0.0");
   offsetBodyPitchLabel = new QLineEdit("0.0");
   
   offsetYawTopLabel->setMaximumSize(50, 30);
   offsetPitchTopLabel->setMaximumSize(50, 30);
   offsetRollTopLabel->setMaximumSize(50, 30);

   offsetYawBottomLabel->setMaximumSize(50, 30);
   offsetPitchBottomLabel->setMaximumSize(50, 30);
   offsetRollBottomLabel->setMaximumSize(50, 30);
   offsetBodyPitchLabel->setMaximumSize(50, 30);


   calibrationOutputBox = new QTextEdit("");
   offsetBodyPitchLabel = new QLineEdit("0.0");

   calibrationOutputBox->setMaximumSize(400, 400);
   offsetBodyPitchLabel->setMaximumSize(50, 30);
   
   QPushButton *sendButton = new QPushButton("send");
   whichCamera2 = new QRadioButton(tr("BOTTOM Camera"));
   whichCamera3= new QRadioButton(tr("TOP Camera"));
   isCalibrating = new QCheckBox(tr("Autocalibrate"));
   isCalibrating->setChecked(false);
   whichCamera2->setChecked(true);
   QVBoxLayout *vbox = new QVBoxLayout;
   vbox->addWidget(whichCamera2);
   vbox->addWidget(whichCamera3);
   vbox->addWidget(isCalibrating);
   vbox->addWidget(calibrationOutputBox);
   // QPushButton *decButton = new QPushButton("-");
   // decButton->setMaximumSize(30,30);
   // incButton->setMaximumSize(30,30);

   //configureLayout->addWidget(decButton, 0, 0, 1, 1);
   configureLayout->addWidget(new QLabel("Bottom Camera Yaw"), 0, 0, 1, 1);
   configureLayout->addWidget(offsetYawBottomLabel, 0, 1, 1, 1);
   configureLayout->addWidget(new QLabel("Bottom Camera Pitch"), 1, 0, 1, 1);
   configureLayout->addWidget(offsetPitchBottomLabel, 1, 1, 1, 1);
   
   configureLayout->addWidget(new QLabel("Bottom Roll Pitch"), 2, 0, 1, 1);
   configureLayout->addWidget(offsetRollBottomLabel, 2, 1, 1, 1);


   configureLayout->addWidget(new QLabel("Top Camera Yaw"), 3, 0, 1, 1);
   configureLayout->addWidget(offsetYawTopLabel, 3, 1, 1, 1);

   configureLayout->addWidget(new QLabel("Top Camera Pitch"), 4, 0, 1, 1);
   configureLayout->addWidget(offsetPitchTopLabel, 4, 1, 1, 1);
   
   configureLayout->addWidget(new QLabel("Top Roll Pitch"), 5, 0, 1, 1);
   configureLayout->addWidget(offsetRollTopLabel, 5, 1, 1, 1);


   configureLayout->addWidget(new QLabel("Body Pitch"), 6, 0, 1, 1);
   configureLayout->addWidget(offsetBodyPitchLabel, 6, 1, 1, 1);

   configureLayout->addWidget(sendButton, 7, 0, 1, 2);
   configureLayout->addLayout(vbox, 8, 0, 1, 2, Qt::AlignTop);
   optionsLayout->addLayout(configureLayout, 1, 0, 1, 1,
                            Qt::AlignLeft);

   connect(sendButton, SIGNAL(clicked()), this, SLOT(updateOffset()));
   connect(isCalibrating, SIGNAL(clicked()), this, SLOT(isCalibratingSlot()));
   // connect(decButton, SIGNAL(clicked()), this, SLOT(decOffset()));

   camLabel->setAlignment(Qt::AlignTop);
   camLabel->setMinimumSize(BOT_IMAGE_COLS, BOT_IMAGE_ROWS);
   camLabel->setMaximumSize(BOT_IMAGE_COLS, BOT_IMAGE_ROWS);
   camLabel->installEventFilter(this);

/*
   currentOffsetX = 0.0;
   currentOffsetY = 0.0;
   currentBodyPitchOffset = 0.0;
   stringstream ss;
   ss << currentOffsetX;
   offsetXLabel->setText(QString(ss.str().c_str()));
   
   stringstream ss2;
   ss2 << currentOffsetY;
   offsetYLabel->setText(QString(ss2.str().c_str()));
   
   stringstream ss3;
   ss3 << currentBodyPitchOffset;
   offsetYLabel->setText(QString(ss3.str().c_str()));
   */
}

void CameraPoseTab::redraw() {
   bool top = whichCamera3->isChecked();
   if (top) {
      lastRendering =  QImage(TOP_IMAGE_COLS,
                              TOP_IMAGE_ROWS, QImage::Format_RGB32);
   } else {
      lastRendering =  QImage(BOT_IMAGE_COLS,
                              BOT_IMAGE_ROWS, QImage::Format_RGB32);
   }
   drawImage(&lastRendering);
   if (top) {
      imagePixmap = QPixmap(QPixmap::fromImage(
         lastRendering.scaled(BOT_IMAGE_COLS, BOT_IMAGE_ROWS)));
   } else {
      imagePixmap = QPixmap(QPixmap::fromImage(lastRendering));
   }
   drawOverlays(&imagePixmap);

   camLabel->setPixmap(imagePixmap);
   Parameters<float> p = readFrom(kinematics, parameters);
   std::string s = KinematicsCalibrationSkill::printParams(p);
   calibrationOutputBox->setText(QString(s.c_str()));
}



void CameraPoseTab::drawImage(QImage *image) {

   int s_row, s_col;
   if (whichCamera3->isChecked()) {
      for (unsigned int row = 0; row < TOP_IMAGE_ROWS; ++row) {
         for (unsigned int col = 0; col < TOP_IMAGE_COLS; ++col) {
            s_row = row / TOP_SALIENCY_DENSITY;
            s_col = col / TOP_SALIENCY_DENSITY;
            image->setPixel(col, row,
          CPLANE_COLOURS[topSaliency[s_col][s_row]].rgb());
         }
      }
   } else {
      for (unsigned int row = 0; row < BOT_IMAGE_ROWS; ++row) {
         for (unsigned int col = 0; col < BOT_IMAGE_COLS; ++col) {
            s_row = row / BOT_SALIENCY_DENSITY;
            s_col = col / BOT_SALIENCY_DENSITY;
            image->setPixel(col, row,
          CPLANE_COLOURS[botSaliency[s_col][s_row]].rgb());
         }
      }
   }
}

boost::numeric::ublas::matrix<float> CameraPoseTab::createPoint(float a,
      float b,
      float c) {
   matrix<float> point(4, 1);
   point(0, 0) = a;
   point(1, 0) = b;
   point(2, 0) = c;
   point(3, 0) = 1;
   return point;                                                 
}

void CameraPoseTab::drawOverlays(QPixmap *pixmap) {
   QPainter painter(pixmap);

   // set up robot abs position and heading
   //QPoint position(0, 0);
   QPoint position(-MARKER_CENTER_X, 0);
   //if (!radio1->isChecked()) {
   //   position = QPoint(-3050, 0);
   //}
   float heading = DEG2RAD(0);

   if (!blackboard) return;


   SensorValues sensorValues;
   for (int i = 0; i < Joints::NUMBER_OF_JOINTS; ++i) {
      sensorValues.joints.angles[i] = 0;
   }
   sensorValues.joints.angles[Joints::HeadPitch] = DEG2RAD(-10);
   if(blackboard) {
      sensorValues = readFrom(motion, sensors);
      kinematics.parameters = readFrom(kinematics, parameters);
   }
   kinematics.setSensorValues(sensorValues);
   Kinematics::Chain foot = kinematics.determineSupportChain();

   kinematics.updateDHChain();
   matrix<float> c2w;
   if (whichCamera3->isChecked()) {
      c2w = kinematics.createCameraToWorldTransform(foot, true);
   } else {
      c2w = kinematics.createCameraToWorldTransform(foot, false);
   }
   
   // c2w = blackboard->kinematics.pose.cameraToWorldTransform;
   // getC2wTransform();
   matrix<float> w2c = c2w;

   invertMatrix(c2w, w2c);
   matrix<float> ctest(4, 1);
   ctest(0, 0) = 0;
   ctest(1, 0) = 0;
   ctest(2, 0) = 0;
   ctest(3, 0) = 1;

   // add on translation to position
   matrix<float> pmatrix(4, 4);
   pmatrix(0, 0) = 1;
   pmatrix(0, 1) = 0;
   pmatrix(0, 2) = 0;
   pmatrix(0, 3) = -position.x();

   pmatrix(1, 0) = 0;
   pmatrix(1, 1) = 1;
   pmatrix(1, 2) = 0;
   pmatrix(1, 3) = -position.y();

   pmatrix(2, 0) = 0;
   pmatrix(2, 1) = 0;
   pmatrix(2, 2) = 1;
   pmatrix(2, 3) = 0;

   pmatrix(3, 0) = 0;
   pmatrix(3, 1) = 0;
   pmatrix(3, 2) = 0;
   pmatrix(3, 3) = 1;

   matrix<float> rmatrix = rotateZMatrix(heading);
   w2c = prod(w2c, rmatrix);
   w2c = prod(w2c, pmatrix);

   matrix<float> cmatrix = w2c;
   

   // finally set up projection matrix
   float ex = 0;
   float ey = 0;
   float ez = 1.0/tan(IMAGE_HFOV/2);
   matrix<float> projection(4, 4);
   projection(0, 0) = 1;
   projection(0, 1) = 0;
   projection(0, 2) = 0;
   projection(0, 3) = -ex;

   projection(1, 0) = 0;
   projection(1, 1) = 1;
   projection(1, 2) = 0;
   projection(1, 3) = -ey;

   projection(2, 0) = 0;
   projection(2, 1) = 0;
   projection(2, 2) = 1;
   projection(2, 3) = 0;

   projection(3, 0) = 0;
   projection(3, 1) = 0;
   projection(3, 2) = 1.0/ez;
   projection(3, 3) = 0;

   matrix<float> transform = prod(projection, w2c);
   /*
      matrix<float> pixel = prod(transform, point);
      std::cout << "Point: " << pixel << std::endl;
      pixel(0, 0) /= pixel(3, 0);
      pixel(1, 0) /= pixel(3, 0);
      pixel(2, 0) = 0;
      pixel(3, 0) = 1;
      std::cout << "Image coord (-1 1) : " << pixel << std::endl;

   // really need to define these somewhere
   float ar = 34.8/46.4;
   float xscale = 640/2;
   float yscale = 480/2;
   pixel(0, 0) = (pixel(0, 0)) * xscale + xscale;
   pixel(1, 0) = (pixel(1, 0)) * yscale + yscale;
   std::cout << "Image coord: " << pixel << std::endl;
    */
   std::vector<matrix<float> > imageLines;
   for(unsigned int i = 0; i < fieldLines.size(); i++) {
      // matrix<float> p = createPoint(1000, 0, 0);
      // std::cout << p << ": " << prod(cmatrix,p) << std::endl;
      matrix<float> pixel = prod(transform, fieldLines[i]);
      // std::cout << fieldLines[i] << ": " << pixel << std::endl;
      // std::cout << "Point: " << pixel << std::endl;
      pixel(0, 0) /= ABS(pixel(3, 0));
      pixel(1, 0) /= ABS(pixel(3, 0));
      pixel(2, 0) = 0;
      // std::cout << "Image coord (-1 1) : " << pixel << std::endl;

      // really need to define these somewhere
      float xscale = BOT_IMAGE_COLS/2;
      float yscale = BOT_IMAGE_ROWS/2;
      pixel(0, 0) = (pixel(0, 0)) * xscale + xscale;
      pixel(1, 0) = (pixel(1, 0)) * xscale + yscale;
      // std::cout << "Image coord: " << pixel << std::endl;

      imageLines.push_back(pixel);
   }
   for(unsigned int i = 0; i < fieldLines.size(); i += 2) {
      if (imageLines[i](3, 0) >= 0 || imageLines[i+1](3, 0) >= 0) {
         painter.drawLine(imageLines[i](0, 0),
               imageLines[i](1, 0),
               imageLines[i+1](0, 0),
               imageLines[i+1](1, 0));
            }
   }

}

// TODO(brockw): see if this can be genericized into tab.cpp, so it's not in
// every tab
void CameraPoseTab::newNaoData(NaoData *naoData) {
   if (!naoData || !naoData->getCurrentFrame().blackboard) {
      imagePixmap.fill(Qt::darkGray);
      camLabel->setPixmap(imagePixmap);
      blackboard = NULL;
   } else {
      if (parent->currentIndex() == parent->indexOf(this)) {
         blackboard = naoData->getCurrentFrame().blackboard;
         if (readFrom(vision, topSaliency) && readFrom(vision, botSaliency)) {
            memcpy(topSaliency, readFrom(vision, topSaliency),
                  TOP_SALIENCY_ROWS*TOP_SALIENCY_COLS*sizeof(Colour));
            memcpy(botSaliency, readFrom(vision, botSaliency),
                  BOT_SALIENCY_ROWS*BOT_SALIENCY_COLS*sizeof(Colour));
            redraw();
         }
      }
   }
}

void CameraPoseTab::redrawSlot() {
   redraw();
}

void CameraPoseTab::isCalibratingSlot() {
   if (blackboard) {
      stringstream ss;
      ss << ("--kinematics.isCalibrating=");
      if (isCalibrating->isChecked()) {
         ss << "1";
      } else {
         ss << "0";
      }
      emit sendCommandToRobot(QString(ss.str().c_str()));
   }
}
void CameraPoseTab::updateOffset() {
   if (blackboard) {
      std::vector<std::string> commands;
      commands.push_back(createCommandString("--kinematics.cameraYawBottom=", offsetYawBottomLabel->text().toStdString()));
      commands.push_back(createCommandString("--kinematics.cameraPitchBottom=", offsetPitchBottomLabel->text().toStdString()));
      commands.push_back(createCommandString("--kinematics.cameraRollBottom=", offsetRollBottomLabel->text().toStdString()));
      commands.push_back(createCommandString("--kinematics.cameraYawTop=", offsetYawTopLabel->text().toStdString()));
      commands.push_back(createCommandString("--kinematics.cameraPitchTop=", offsetPitchTopLabel->text().toStdString()));
      commands.push_back(createCommandString("--kinematics.cameraRollTop=", offsetRollTopLabel->text().toStdString()));
      commands.push_back(createCommandString("--kinematics.bodyPitch=", offsetBodyPitchLabel->text().toStdString()));

/*      if (whichCamera2->isChecked()) {
         commands.push_back(createCommandString("--default.whichCamera=", "BOTTOM_CAMERA"));
      }
      if (whichCamera3->isChecked()) {
         commands.push_back(createCommandString("--default.whichCamera=", "TOP_CAMERA"));
      }
*/
      std::string command;
      for (unsigned int i = 0; i < commands.size(); i++) {
         command += commands[i];
         command += " ";
      }
      emit sendCommandToRobot(QString(command.c_str()));
   }
}

std::string CameraPoseTab::createCommandString(std::string argument, std::string message) {
   stringstream ss;
   ss << argument;
   ss << message;
   return ss.str();
}

void CameraPoseTab::incOffset() {
}

void CameraPoseTab::decOffset() {
}
