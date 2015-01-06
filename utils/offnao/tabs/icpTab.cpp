#include <QMenu>
#include <QMenuBar>
#include <QDebug>
#include <QBitmap>
#include <QGroupBox>


#include <utility>
#include <iostream>
#include "icpTab.hpp"
#include "../../robot/perception/vision/yuv.hpp"
#include "../../robot/perception/vision/rle.hpp"
#include "../../robot/perception/kinematics/Pose.hpp"
#include "blackboard/Blackboard.hpp"

#include "utils/CPlaneColours.hpp"

#include "perception/vision/Vision.hpp"

#define DEBUG_IMAGE_ROWS (IMAGE_ROWS / 4)
#define DEBUG_IMAGE_COLS (IMAGE_COLS / 4)

using namespace std;

ICPTab::ICPTab(QTabWidget *parent, QMenuBar *menuBar,
      Vision *vision) : blackboard(0) {
   initMenu(menuBar);
   init();
   this->vision = vision;
   memset(topSaliency, 0, TOP_SALIENCY_ROWS*TOP_SALIENCY_COLS*sizeof(Colour));
   memset(botSaliency, 0, BOT_SALIENCY_ROWS*BOT_SALIENCY_COLS*sizeof(Colour));
   this->parent = parent;
}


void ICPTab::initMenu(QMenuBar *) {
}

void ICPTab::init() {
   layout = new QGridLayout(this);
   setLayout(layout);
   layout->setAlignment(layout, Qt::AlignTop);

   layout->setHorizontalSpacing(5);
   layout->setHorizontalSpacing(5);


   layout->addWidget(&fieldView, 0, 0, 2, 1);

   /* draw the field with nothing on it */
   fieldView.redraw();

   topImagePixmap = QPixmap(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
   topImagePixmap.fill(Qt::darkGray);
   topCamLabel  = new QLabel();
   topCamLabel->setPixmap(topImagePixmap);
   topCamLabel->setMinimumSize(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
   topCamLabel->setMaximumSize(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
   layout->addWidget(topCamLabel, 0, 1, 1, 2);

   botImagePixmap = QPixmap(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
   botImagePixmap.fill(Qt::darkGray);
   botCamLabel  = new QLabel();
   botCamLabel->setPixmap(botImagePixmap);
   botCamLabel->setMinimumSize(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
   botCamLabel->setMaximumSize(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
   layout->addWidget(botCamLabel, 1, 1, 1, 2);

   //layout->addWidget(&variableView, 0, 3, 2, 2);

   QGroupBox *setup = new QGroupBox(tr("Robot Position Setup"));

   layout->addWidget(setup, 0, 3, 1, 1);

   QGridLayout *setupGroupLayout = new QGridLayout();
   setup->setLayout(setupGroupLayout);

   setupGroupLayout->addWidget(new QLabel("X: "), 1, 1);
   trueX = new QLineEdit("-1500");
   setupGroupLayout->addWidget(trueX, 1, 2);
   setupGroupLayout->addWidget(new QLabel("Y: "), 2, 1);
   trueY = new QLineEdit("0.0");
   setupGroupLayout->addWidget(trueY, 2, 2);
   setupGroupLayout->addWidget(new QLabel("Theta: "), 3, 1);
   trueTheta = new QLineEdit("0.0");
   setupGroupLayout->addWidget(trueTheta, 3, 2);

   lostButton = new QRadioButton(QString("Lost"));
   lostButton->setChecked(false);
   setupGroupLayout->addWidget(lostButton, 4, 1, 1, 2, Qt::AlignHCenter);

   setupGroupLayout->addWidget(new QLabel("TeamBall X: "), 5, 1);
   ballX = new QLineEdit("1500");
   setupGroupLayout->addWidget(ballX, 5, 2);
   setupGroupLayout->addWidget(new QLabel("TeamBall Y: "), 6, 1);
   ballY = new QLineEdit("0.0");
   setupGroupLayout->addWidget(ballY, 6, 2);


   teamBallButton = new QRadioButton(QString("Add Team Ball"));
   teamBallButton->setChecked(false);
   setupGroupLayout->addWidget(teamBallButton, 7, 1, 1, 2, Qt::AlignHCenter);


   initButton = new QPushButton("Set position");
   setupGroupLayout->addWidget(initButton, 8, 1, 1, 2);

   connect(initButton, SIGNAL(released()), this, SLOT(setPos()));

   QGroupBox *info = new QGroupBox(tr("ICP Information"));
   QGridLayout *infoGroupLayout = new QGridLayout();
   info->setLayout(infoGroupLayout);
   icpInfo = new QLabel();
   icpInfo->setAlignment(Qt::AlignTop | Qt::AlignLeft);
   icpInfo->setFont(QFont("Monospace"));
   icpInfo->setText("hello world");
   infoGroupLayout->addWidget(icpInfo, 0, 0);

   layout->addWidget(info, 1, 3);

   setPos();
}

void ICPTab::setPos() {
   fixedPos.x() = trueX->text().toFloat();
   fixedPos.y() = trueY->text().toFloat();
   fixedPos.theta() = DEG2RAD(trueTheta->text().toFloat());
   fixedPos.var(0, 0) = 50000;
   fixedPos.var(1, 1) = 50000;
   fixedPos.var(2, 2) = DEG2RAD(10);
   isLost = lostButton->isChecked();
   teamBall.x() = ballX->text().toFloat();
   teamBall.y() = ballY->text().toFloat();
   teamBall.var(0, 0) = 50000;
   teamBall.var(1, 1) = 50000;
   useTeamBall = teamBallButton->isChecked();
   drawICP();
}

void ICPTab::redraw() {
   if (topFrame || botFrame || topSaliency || botSaliency) {
      QImage *topImage;
      QImage *botImage;
      bool nnmc_loaded = vision->nnmc_top.isLoaded() &&
                         vision->nnmc_bot.isLoaded();

      if (topFrame && botFrame && ! nnmc_loaded) {
         topImage = new QImage(IMAGE_COLS, IMAGE_ROWS, QImage::Format_RGB32);
         botImage = new QImage(IMAGE_COLS, IMAGE_ROWS, QImage::Format_RGB32);
      } else {
         topImage = new QImage(TOP_SALIENCY_COLS,
                            TOP_SALIENCY_ROWS,
                            QImage::Format_RGB32);
         botImage = new QImage(BOT_SALIENCY_COLS,
                            BOT_SALIENCY_ROWS,
                            QImage::Format_RGB32);
      }
      drawImage(topImage, botImage);
      
      // Scale the images up to their real size to draw overlays
      topImagePixmap = QPixmap::fromImage(
               topImage->scaled(2*DEBUG_IMAGE_COLS, 2*DEBUG_IMAGE_ROWS));
      botImagePixmap = QPixmap::fromImage(
               botImage->scaled(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS));

      drawOverlays(&topImagePixmap, &botImagePixmap);

      // Scale top image back down to 640x480 to fit the screen
      topImagePixmap = topImagePixmap.scaled(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
      
      delete topImage;
      delete botImage;
   } else {
      topImagePixmap = QPixmap(IMAGE_COLS, IMAGE_ROWS);
      topImagePixmap.fill(Qt::darkRed);
      botImagePixmap = QPixmap(IMAGE_COLS, IMAGE_ROWS);
      botImagePixmap.fill(Qt::darkRed);
   }
   topCamLabel->setPixmap(topImagePixmap);
   botCamLabel->setPixmap(botImagePixmap);
}

void ICPTab::drawOverlays(QPixmap *topImage, QPixmap *botImage) {
   if (!blackboard) return;

   std::vector<FootInfo>         feet = std::vector<FootInfo>();
      //= readFrom (vision, feet);
   std::vector<BallInfo>         balls         = readFrom(vision, balls);
   std::vector<PostInfo>         posts         = readFrom(vision, posts);
   std::vector<RobotInfo>        robots        = readFrom(vision, robots);
   std::vector<FieldEdgeInfo>    fieldEdges    = readFrom(vision, fieldEdges);
   std::vector<FieldFeatureInfo> fieldFeatures = readFrom(vision, fieldFeatures);
   std::vector<Ipoint>           landmarks     = readFrom(vision, landmarks);

   std::pair<int, int> horizon = readFrom(motion, pose).getHorizon();
   std::pair<int, int> *horizon_p = &horizon;

   drawOverlaysGeneric (topImage,
                        botImage,
                        horizon_p,
                        &feet,
                        &balls,
                        &posts,
                        &robots,
                        &fieldEdges,
                        &fieldFeatures,
                        &landmarks,
                        0.5
                       );

   /*
   QPainter painter(image);
   const Pose &pose = readFrom(kinematics, pose);
   const std::pair<int, int> horizon = pose.getHorizon();
   painter.setBrush(QBrush(QColor(255, 255, 255)));
   painter.drawLine(0,horizon.first/SALIENCY_DENSITY*2,
         640/SALIENCY_DENSITY*2,
         horizon.second/SALIENCY_DENSITY*2);

   //draw body exclusion points
   painter.setBrush(QBrush(QColor(255, 255, 255)));
   float scale = 2.0/SALIENCY_DENSITY;
   const int16_t *points = pose.getExclusionArray();
   for (int i = 0; i < Pose::EXCLUSION_RESOLUTION; i++) {
       painter.drawEllipse(QPoint(scale*640 * i*1.0/Pose::EXCLUSION_RESOLUTION,
                         scale*points[i]), 2, 2);
   }
   */

   return;
}


void ICPTab::drawImage(QImage *topImage, QImage *botImage) {
   if (topFrame && botFrame) {
      bool nnmc_loaded = vision->nnmc_top.isLoaded() &&
                         vision->nnmc_bot.isLoaded();

      if (nnmc_loaded) {
         vision->topFrame = topFrame;
         vision->botFrame = botFrame;
         vision->processFrame();

         fieldView.redraw();

         // Top Image
         for (int row = 0; row < TOP_SALIENCY_ROWS; ++row) {
            for (int col = 0; col < TOP_SALIENCY_COLS; ++col) {
               topImage->setPixel(col, row,
                  CPLANE_COLOURS[vision->topSaliency.asFovea().colour(col, row)].rgb());
            }
         }

         // Bottom Image
         for (int row = 0; row < BOT_SALIENCY_ROWS; ++row) {
            for (int col = 0; col < BOT_SALIENCY_COLS; ++col) {
               botImage->setPixel(col, row,
                  CPLANE_COLOURS[vision->botSaliency.asFovea().colour(col, row)].rgb());
            }
         }
      } else {
         // display normal images
         for (unsigned int row = 0; row < IMAGE_ROWS; ++row) {
            for (unsigned int col = 0; col < IMAGE_COLS; ++col) {
               topImage->setPixel(col, row,
                     getRGB(col, row, topFrame, TOP_IMAGE_COLS));
            }
         }
         for (unsigned int row = 0; row < IMAGE_ROWS; ++row) {
            for (unsigned int col = 0; col < IMAGE_COLS; ++col) {
               botImage->setPixel(col, row,
                     getRGB(col, row, botFrame, BOT_IMAGE_COLS));
            }
         }
      }
   } else {
      for (unsigned int row = 0; row <IMAGE_ROWS/TOP_SALIENCY_DENSITY; ++row) {
         for (unsigned int col =0;col<IMAGE_COLS/TOP_SALIENCY_DENSITY; ++col) {
            if (0 <= topSaliency[col][row] && topSaliency[col][row] < cNUM_COLOURS) {
               topImage->setPixel(col, row,
               CPLANE_COLOURS[topSaliency[col][row]].rgb());
            } else {
               std::cerr << "Bad pixel at " << col << " " << row << std::endl;
            }
         }
      }
      for (unsigned int row = 0; row <IMAGE_ROWS/BOT_SALIENCY_DENSITY; ++row) {
         for (unsigned int col=0; col<IMAGE_COLS/BOT_SALIENCY_DENSITY; ++col) {
            if (0 <= botSaliency[col][row] && botSaliency[col][row] < cNUM_COLOURS) {
                botImage->setPixel(col, row,
                CPLANE_COLOURS[botSaliency[col][row]].rgb());
            } else {
                std::cerr << "Bad pixel at " << col << " " << row << std::endl;
            }
         }
      }
   }
}

// TODO(brockw): see if this can be genericized into tab.cpp, so it's not in
// every tab
void ICPTab::newNaoData(NaoData *naoData) {

   if (!naoData || !naoData->getCurrentFrame().blackboard) {  // clean up display, as read is finished
      topImagePixmap.fill(Qt::darkGray);
      topCamLabel->setPixmap(topImagePixmap);
      botImagePixmap.fill(Qt::darkGray);
      botCamLabel->setPixmap(botImagePixmap);
   } else if (naoData->getFramesTotal() != 0) {
      blackboard = (naoData->getCurrentFrame().blackboard);
      topFrame = readFrom(vision, topFrame);
      botFrame = readFrom(vision, botFrame);
      if (!topFrame) {
         if (readFrom(vision, topSaliency))
            memcpy(topSaliency, readFrom(vision, topSaliency),
                   TOP_SALIENCY_ROWS*TOP_SALIENCY_COLS*sizeof(Colour));
      }
      if (!botFrame) {
         if (readFrom(vision, botSaliency))
            memcpy(botSaliency, readFrom(vision, botSaliency),
                   BOT_SALIENCY_ROWS*BOT_SALIENCY_COLS*sizeof(Colour));
      }
      if (parent->currentIndex() == parent->indexOf(this)) {
         redraw();
         drawICP();
      }
   }
}

void ICPTab::drawICP() {
   if (blackboard && !isnan(fixedPos.theta()) && !isnan(fixedPos.x()) && !isnan(fixedPos.y())) {
      std::vector<PostInfo>         posts         = readFrom(vision, posts);
      std::vector<FieldEdgeInfo>    fieldEdges    = readFrom(vision, fieldEdges);
      std::vector<FieldFeatureInfo> fieldFeatures = readFrom(vision, fieldFeatures);
      float awayGoalProb = readFrom(vision, awayGoalProb);
      
      AbsCoord ballPosAbs = readFrom(localisation, ballPos);
      AbsCoord robotPos = readFrom(localisation, robotPos);
      AbsCoord ballRRC = ballPosAbs.convertToRobotRelativeCartesian(robotPos);

      SensorValues values = readFrom(motion, sensors);
      float headYaw = values.joints.angles[Joints::HeadYaw];

      int icpResult;
      if (useTeamBall) {
         icpResult = ICP::localise(fixedPos, fieldFeatures, posts, awayGoalProb, headYaw, fieldEdges, ballRRC, isLost, teamBall);
         fieldView.redraw(blackboard, fixedPos, icpResult, ICP::getCombinedObs(), ballRRC, teamBall);
      } else {
         icpResult = ICP::localise(fixedPos, fieldFeatures, posts, awayGoalProb, headYaw, fieldEdges, ballRRC, isLost);
         fieldView.redraw(blackboard, fixedPos, icpResult, ICP::getCombinedObs(), ballRRC);
      }

      stringstream icpStream;
/*      icpStream << "Robot Position;" << std::endl;
      icpStream << "       X: " <<  fixedPos.x() << std::endl;
      icpStream << "       Y: " <<  fixedPos.y() << std::endl;
      icpStream << "   Theta: " <<  RAD2DEG(fixedPos.theta()) << std::endl;
      icpStream << "  isLost: " << (isLost ? "True":"False") << std::endl;
      icpStream << std::endl;
*/
      icpStream << "Vision Inputs;" << std::endl;
      icpStream << "Number of posts          : " << posts.size() << std::endl;
      icpStream << "Number of field edges    : " << fieldEdges.size() << std::endl;
      icpStream << "Number of field features : " << fieldFeatures.size() << std::endl;
      icpStream << "Away Goal Prob           : " << awayGoalProb << std::endl;
      icpStream << std::endl;

      icpStream << "ICP Results;" << std::endl;
      icpStream << "Return value: ";
      if (icpResult == ICP_LOST) {
         icpStream << "ICP_LOST      (";
      } else if (icpResult == ICP_NO_OBS) {
         icpStream << "ICP_NO_OBS    (";
      } else if (icpResult >= ICP_LOCALISED) {
         icpStream << "ICP_LOCALISED (";
      } else {
         icpStream << "invalid icp localise return (";
      }
      icpStream << icpResult << ")" << std::endl;
      icpInfo->setText(icpStream.str().c_str());
   }
}

