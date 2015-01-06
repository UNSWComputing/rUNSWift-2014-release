#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QMenuBar>
#include <QInputDialog>
#include <vector>
#include <iostream>
#include <utility>
#include <cmath>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <cstdio>

#include "controlTab.hpp"
#include <boost/shared_ptr.hpp>
#include "blackboard/Blackboard.hpp"
#include "../../robot/perception/vision/yuv.hpp"
#include "../../robot/perception/vision/rle.hpp"
#include "types/BehaviourRequest.hpp"
#include "types/ActionCommand.hpp"
#include "utils/CPlaneColours.hpp"

using namespace std;
using boost::asio::ip::udp;
using namespace Qt;

enum keyType
{
   KEY_BODY_CONTROL,
   KEY_HEAD_CONTROL,
   KEY_INDIRECT_CONTROL, // Change camera etc.
   KEY_NO_CONTROL
};

// Power levels
int bodyPowers[5] = {0, 10, 40, 80, 160};
float headPowers[5] = {0.02, 0.05, 0.1, 0.15, 0.2};
float kickPowers[5] = {0.05, 0.15, 0.4, 0.7, 1.0};
float turnPowers[5] = {0.05, 0.15, 0.4, 0.7, 1.0};

// Utility functions
int typeOfKey(int inKey)
{
   switch (inKey)
   {
      case Key_W:
      case Key_S:
      case Key_A:
      case Key_D:
      case Key_Q:
      case Key_E:
      case Key_X:
      case Key_Control:
      case Key_C:
         return KEY_BODY_CONTROL;
      
      case Key_P:
      case Key_Semicolon:
      case Key_L:
      case Key_Apostrophe:
         return KEY_HEAD_CONTROL;
         
      case Key_T:
      case Key_B:
      case Key_Equal:
      case Key_Minus:
         return KEY_INDIRECT_CONTROL;
      
      default:
         return KEY_NO_CONTROL;
   }
}

ControlTab::ControlTab(QTabWidget *parent, QMenuBar *menuBar,
                               Vision *vision) :
   zoomLevel(1), zoomLog(0),  prevMouseX(0), prevMouseY(0),
   prevZoomLevel(1), naoData(0) {
   initMenu(menuBar);
   init();
   this->vision = vision;
   this->parent = parent;
   memset(saliencyr, 0, 160*120*sizeof(Colour));
   underControl = false;
   counter = 0;
   curNao = "";
   br.whichCamera = TOP_CAMERA;
   br.actions.body.actionType = ActionCommand::Body::STAND;
   br.actions.head.isRelative = true;
   curPower = 3;
   commandsIssued = 0;
}

void ControlTab::newNaoData(NaoData *naoData) {
   if (! naoData) {
      return;
   }
   /*
   cout << "1\n";
   if (!naoData || !naoData->getCurrentFrame().blackboard) {
      imagePixmap.fill(Qt::darkGray); cout << "2\n";
      camLabel->setPixmap(imagePixmap); cout << "3\n";
      currentFrame = NULL; cout << "4\n";
      cout << "CURRENTFRAME = NULL";
   } else {
      this->naoData = naoData; cout << "5\n";
   }*/
   
   blackboard = (naoData->getCurrentFrame().blackboard);
   
   /*
   if (readFrom(vision, saliency))
   {
      cout << "Can read from saliency.\n";
      readArray(vision, saliency, saliencyr);
   }
   
   currentFrame = readFrom(vision, currentFrame);
      
   cout << "Read.\n";
   
   
   blackboard = (naoData->getCurrentFrame().blackboard);
   if (readFrom(vision, saliency))
      memcpy(saliency, readFrom(vision, saliency),
             120*160*sizeof(Colour));*/

   redraw();
}

void ControlTab::initMenu(QMenuBar *menuBar) {
   controlMenu = new QMenu("&Control");
   menuBar->addMenu(controlMenu);

   controlOnAct = new QAction(tr("Set Motion Control on"), controlMenu);
   
   controlMenu->addAction(controlOnAct);

   // connect the actions
   connect(controlOnAct, SIGNAL(triggered()), this, SLOT(controlOn()));
}

QLabel *ControlTab::labelWithText(const char *text, bool isBold)
{
   QLabel *newLabel = new QLabel();
   newLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
   QFont font("Monospace");
   if (isBold)
      font.setBold(true);
   newLabel->setFont(font);
   newLabel->setText(text);
   labels.push_back(newLabel);
   return newLabel;
}

QLabel *ControlTab::labelWithText(const char *text)
{
   return labelWithText(text, false);
}

void ControlTab::init() {
   layout = new QGridLayout(this);
   setLayout(layout);
   layout->setAlignment(layout, Qt::AlignTop);
   layout->setHorizontalSpacing(5);
   
   layout->addWidget(labelWithText("Body Controls", true), 0, 0, 1, 3);
   layout->addWidget(labelWithText("Q"), 1, 0, 1, 1);
   layout->addWidget(labelWithText("W"), 1, 1, 1, 1);
   layout->addWidget(labelWithText("E"), 1, 2, 1, 1);
   layout->addWidget(labelWithText("A"), 2, 0, 1, 1);
   layout->addWidget(labelWithText("S"), 3, 1, 1, 1);
   layout->addWidget(labelWithText("D"), 2, 2, 1, 1);
   
   layout->addWidget(labelWithText("Head Controls", true), 0, 3, 1, 3);
   layout->addWidget(labelWithText("P"), 1, 4, 1, 1);
   layout->addWidget(labelWithText("L"), 2, 3, 1, 1);
   layout->addWidget(labelWithText(";"), 3, 4, 1, 1);
   layout->addWidget(labelWithText("'"), 2, 5, 1, 1);
   
   layout->addWidget(labelWithText("Misc. Controls", true), 0, 6, 1, 2);
   layout->addWidget(labelWithText("Kick (Left Foot): Control"), 1, 6, 1, 2);
   layout->addWidget(labelWithText("Kick (Right Foot): X"), 2, 6, 1, 2);
   layout->addWidget(labelWithText("Crouch: C"), 3, 6, 1, 2);
   layout->addWidget(labelWithText("Increase Power: ="), 4, 6, 1, 2);
   layout->addWidget(labelWithText("Decrease Power: -"), 5, 6, 1, 2);
   
   curPowerLabel = labelWithText("Current Power Level [0-4]: 3");
   layout->addWidget(curPowerLabel, 6, 6, 1, 2);
   
   commandsIssuedLabel = labelWithText("Commands Issued: 0");
   layout->addWidget(commandsIssuedLabel, 10, 0, 1, 1);
   
   /*imagePixmap = QPixmap(640.0/2,480.0/2);
   imagePixmap.fill(Qt::darkGray);
   camLabel = new QLabel();
   camLabel->setPixmap(imagePixmap);
   camLabel->setMinimumSize(640.0/2, 480.0/2);
   camLabel->setMaximumSize(640.0/2, 480.0/2);
   layout->addWidget(camLabel, 0, 11, 3, 3);*/
}

void ControlTab::setNao(const QString &naoName)
{
   curNao = naoName.toStdString();
   cout << "Set curNao to " << curNao << endl;
}

void ControlTab::controlOn() {
   underControl = !underControl;
   
   if (underControl && curNao != "")
   {
      controlOnAct->setText("Set Motion Control off");
      emit showMessage("Motion Control on.");
   }
   else
   {
      controlOnAct->setText("Set Motion Control on");
      stopBehaviour();
      emit showMessage("");
   }
   
   redraw();
}

void ControlTab::drawImage(QImage *image) {
   if (currentFrame) {
         // display normal image
         for (unsigned int row = 0; row < IMAGE_ROWS; ++row) {
            for (unsigned int col = 0; col < IMAGE_COLS; ++col) {
               image->setPixel(col, row,
                     getRGB(col, row, currentFrame, TOP_IMAGE_COLS));
            }
         }
   } else if (saliencyr) {
      for (unsigned int row = 0; row <IMAGE_ROWS/BOT_SALIENCY_DENSITY; ++row) {
         for (unsigned int col=0; col<IMAGE_COLS/BOT_SALIENCY_DENSITY; ++col) {
            if (0 <= saliencyr[col][row] && saliencyr[col][row] < cNUM_COLOURS) {
                image->setPixel(col, row,
                                  CPLANE_COLOURS[saliencyr[col][row]].rgb());
            } else {
                std::cerr << "Bad pixel at " << col << " " << row << std::endl;
            }
         }
      }
   }
}

void ControlTab::redraw() {
   ostringstream oss;
   oss << "Current Power Level [0-4]: " << curPower;
   curPowerLabel->setText(oss.str().c_str());
   
   oss.str("");
   oss << "Commands Issued: " << commandsIssued;
   commandsIssuedLabel->setText(oss.str().c_str());
   
   /*
   if (saliencyr) {
      QImage *image;
      image = new QImage(IMAGE_COLS/SALIENCY_DENSITY,
                            IMAGE_ROWS/SALIENCY_DENSITY,
                            QImage::Format_RGB32);

      drawImage(image);
      
      imagePixmap = QPixmap::fromImage(
         image->scaled(IMAGE_COLS/SALIENCY_DENSITY*2,
                       IMAGE_ROWS/SALIENCY_DENSITY*2));
      delete image;
   } else {
      imagePixmap = QPixmap(IMAGE_COLS, IMAGE_ROWS);
      imagePixmap.fill(Qt::darkRed);
   }
   camLabel->setPixmap(imagePixmap);
   */
}

void ControlTab::keyPressEvent(QKeyEvent* event)
{
   if (underControl && curNao != "")
   {
      int key;
      
      key = event->key();
      
      //cout << "Key press event on ControlTab: " << key << endl;
      
      /* Body */
      if (typeOfKey(key) == KEY_BODY_CONTROL)
      {
         br.actions.body.actionType = ActionCommand::Body::WALK;
         
         if (key == Qt::Key_W)
         {
            br.actions.body.forward = bodyPowers[curPower];
         }
         else if (key == Qt::Key_S)
         {
            br.actions.body.forward = -bodyPowers[curPower];
         }
         else if (key == Qt::Key_A)
         {
            br.actions.body.left = bodyPowers[curPower];
         }
         else if (key == Qt::Key_D)
         {
            br.actions.body.left = -bodyPowers[curPower];
         }
         else if (key == Qt::Key_Q)
         {
            br.actions.body.turn = turnPowers[curPower];
         }
         else if (key == Qt::Key_E)
         {
            br.actions.body.turn = -turnPowers[curPower];
         }
         else if (key == Qt::Key_Control)
         {
            br.actions.body.actionType = ActionCommand::Body::KICK;
            br.actions.body.power = kickPowers[curPower];
            br.actions.body.foot = ActionCommand::Body::LEFT;
         }
         else if (key == Qt::Key_X)
         {
            br.actions.body.actionType = ActionCommand::Body::KICK;
            br.actions.body.power = kickPowers[curPower];
            br.actions.body.foot = ActionCommand::Body::RIGHT;
         }
         else if (key == Qt::Key_C)
         {
            br.actions.body.actionType = ActionCommand::Body::GOALIE_SIT;
         }
      }
      
      if (typeOfKey(key) == KEY_INDIRECT_CONTROL)
      {
         if (key == Qt::Key_T)
         {
            br.whichCamera = TOP_CAMERA;
         }
         else if (key == Qt::Key_B)
         {
            br.whichCamera = BOTTOM_CAMERA;
         }
         else if (key == Qt::Key_Equal)
         {
            curPower++;
            if (curPower > 4) curPower = 4;
         }
         else if (key == Qt::Key_Minus)
         {
            curPower--;
            if (curPower < 0) curPower = 0;
         }
      }
      
      /* Head */
      if (typeOfKey(key) == KEY_HEAD_CONTROL)
      {
         if (key == Qt::Key_P)
         {
            br.actions.head.pitchSpeed = headPowers[curPower];
            br.actions.head.pitch = -0.5;
         }
         else if (key == Qt::Key_Semicolon)
         {
            br.actions.head.pitchSpeed = headPowers[curPower];
            br.actions.head.pitch = 0.5;
         }
         else if (key == Qt::Key_L)
         {
            br.actions.head.yawSpeed = headPowers[curPower];
            br.actions.head.yaw = 0.5;
         }
         else if (key == Qt::Key_Apostrophe)
         {
            br.actions.head.yawSpeed = headPowers[curPower];
            br.actions.head.yaw = -0.5;
         }
      }
      
      if (typeOfKey(key) != KEY_NO_CONTROL)
      {
         sendBehaviour();
         commandsIssued++;
      }
   
      redraw();
   }
}

void ControlTab::keyReleaseEvent(QKeyEvent *event)
{
   int key = event->key();
   if (underControl && curNao != "")
   {
      if (typeOfKey(key) == KEY_HEAD_CONTROL)
      {
         if (key == Qt::Key_P || key == Qt::Key_Semicolon)
            br.actions.head.pitchSpeed = 0.0;
         else
            br.actions.head.yawSpeed = 0.0;
      }
      else if (typeOfKey(key) == KEY_BODY_CONTROL)
      {
         if (key == Qt::Key_W || key == Qt::Key_S)
            br.actions.body.forward = 0.0;
         else if (key == Qt::Key_A || key == Qt::Key_D)
            br.actions.body.left = 0.0;
         else if (key == Qt::Key_Q || key == Qt::Key_E)
            br.actions.body.turn = 0.0;
         else if (key == Qt::Key_K)
            br.actions.body.power = 0.0;
         
         
      }
      
      if (typeOfKey(key) != KEY_NO_CONTROL)
      {
         if ((br.actions.head.pitchSpeed == br.actions.head.yawSpeed) && (br.actions.head.yawSpeed == 0.0))
         {
            br.actions.head = ActionCommand::Head();
         }
         if (br.actions.body.forward == 0.0 && br.actions.body.left == 0.0 && br.actions.body.turn == 0.0)
            br.actions.body = ActionCommand::Body();
         
         //cout << "Key release event on ControlTab: " << (int)(event->key()) << endl;
         sendBehaviour();
         commandsIssued++;
      }
      redraw();
   }
}

void ControlTab::stopBehaviour()
{
   sendBehaviour();
}

void ControlTab::sendBehaviour()
{
   try
   {
      boost::asio::io_service io_service;
      
      udp::endpoint receiver_endpoint(boost::asio::ip::address(boost::asio::ip::address_v4::from_string(curNao)), 4000);
      
      //cout << "Found endpoint." << std::endl;
      
      udp::socket socket(io_service);
      socket.open(udp::v4());
      
      if (br.actions.body.actionType != ActionCommand::Body::WALK && br.actions.body.actionType != ActionCommand::Body::KICK && br.actions.body.actionType != ActionCommand::Body::GOALIE_SIT)
         br.actions.body.actionType = ActionCommand::Body::STAND;
      
      if (br.actions.body.actionType != ActionCommand::Body::KICK && br.actions.body.actionType != ActionCommand::Body::GOALIE_SIT && br.actions.head.pitchSpeed == 0.0 && br.actions.head.yawSpeed == 0.0)
         br.actions.body.actionType = ActionCommand::Body::WALK;
      
      void *dp = (void *)&br;
      
      socket.send_to(boost::asio::buffer(dp,sizeof(BehaviourRequest)), receiver_endpoint);
      
      //printf("Body {\n\tAction Type:%d\n\tForward: %d\n\tLeft: %d\n\tTurn:%f\n\tPower:%f\n\tBend:%f\n\tSpeed:%f\n\tKick Direction:%f\n\tFoot:%d\n\t}\n", br.actions.body.actionType, br.actions.body.forward, br.actions.body.left, br.actions.body.turn, br.actions.body.power, br.actions.body.bend, br.actions.body.speed, br.actions.body.kickDirection, br.actions.body.foot);
      
      //printf("Camera: %d\n", br.whichCamera);
      
      //cout << "Sent " << sizeof(BehaviourRequest) << " bytes of data on port " << 4000 << std::endl;
   }
   catch (exception e)
   {
      ;
   }
}
