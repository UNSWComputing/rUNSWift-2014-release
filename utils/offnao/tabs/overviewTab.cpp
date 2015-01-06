#include <QMenu>
#include <QMenuBar>
#include <QDebug>
#include <QBitmap>

#include <utility>
#include <iostream>
#include "overviewTab.hpp"
#include "../../robot/perception/vision/yuv.hpp"
#include "../../robot/perception/vision/rle.hpp"
#include "../../robot/perception/kinematics/Pose.hpp"
#include "blackboard/Blackboard.hpp"

#include "utils/CPlaneColours.hpp"

#include "perception/vision/Vision.hpp"

#define DEBUG_IMAGE_ROWS (IMAGE_ROWS / 4)
#define DEBUG_IMAGE_COLS (IMAGE_COLS / 4)

using namespace std;

OverviewTab::OverviewTab(QTabWidget *parent, QMenuBar *menuBar,
      Vision *vision) : blackboard(0) {
   initMenu(menuBar);
   init();
   this->vision = vision;
   memset(topSaliency, 0, TOP_SALIENCY_ROWS*TOP_SALIENCY_COLS*sizeof(Colour));
   memset(botSaliency, 0, BOT_SALIENCY_ROWS*BOT_SALIENCY_COLS*sizeof(Colour));
   this->parent = parent;
}


void OverviewTab::initMenu(QMenuBar *) {
}

void OverviewTab::init() {
   layout = new QGridLayout(this);
   setLayout(layout);
   layout->setAlignment(layout, Qt::AlignTop);

   layout->setHorizontalSpacing(5);
   layout->setHorizontalSpacing(5);


   layout->addWidget(&fieldView, 0, 0, 2, 1);

   /* draw the field with nothing on it */
   fieldView.redraw(NULL);

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

   layout->addWidget(&variableView, 0, 3, 2, 2);
}

void OverviewTab::redraw() {
   if (topFrame || botFrame || topSaliency || botSaliency) {
      QImage *topImage;
      QImage *botImage;
      bool nnmc_loaded = vision->nnmc_top.isLoaded() &&
                         vision->nnmc_bot.isLoaded();

      if (topFrame && botFrame && ! nnmc_loaded) {
         topImage = new QImage(TOP_IMAGE_COLS,
                               TOP_IMAGE_ROWS,
                               QImage::Format_RGB32);
         botImage = new QImage(BOT_IMAGE_COLS,
                               BOT_IMAGE_ROWS,
                               QImage::Format_RGB32);
      } else {
         topImage = new QImage(TOP_SALIENCY_COLS,
                               TOP_SALIENCY_ROWS,
                               QImage::Format_RGB32);
         botImage = new QImage(BOT_SALIENCY_COLS,
                               BOT_SALIENCY_ROWS,
                               QImage::Format_RGB32);
      }
      drawImage(topImage, botImage);
      
      // Scale iamges up to real size to draw overlays
      QPixmap t = QPixmap::fromImage(
               topImage->scaled(2*DEBUG_IMAGE_COLS, 2*DEBUG_IMAGE_ROWS));
      botImagePixmap = QPixmap::fromImage(
               botImage->scaled(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS));

      drawOverlays(&t, &botImagePixmap);

      // Rescale the top image back to 640x480 to fit the screen
      topImagePixmap = t.scaled(DEBUG_IMAGE_COLS, DEBUG_IMAGE_ROWS);
      
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

void OverviewTab::drawOverlays(QPixmap *topImage, QPixmap *botImage) {
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


void OverviewTab::drawImage(QImage *topImage, QImage *botImage) {
   if (topFrame && botFrame) {
      bool nnmc_loaded = vision->nnmc_top.isLoaded() &&
                         vision->nnmc_bot.isLoaded();

      if (nnmc_loaded) {
         vision->topFrame = topFrame;
         vision->botFrame = botFrame;
         vision->processFrame();

         fieldView.redraw(NULL);

         // Top Image
         for (int row = 0; row < TOP_SALIENCY_ROWS; ++row) {
            for (int col = 0; col < TOP_SALIENCY_COLS; ++col) {
               topImage->setPixel(col, row,
                  CPLANE_COLOURS[vision->topSaliency.asFovea().
                                 colour(col, row)].rgb());
            }
         }

         // Bottom Image
         for (int row = 0; row < BOT_SALIENCY_ROWS; ++row) {
            for (int col = 0; col < BOT_SALIENCY_COLS; ++col) {
               botImage->setPixel(col, row,
                  CPLANE_COLOURS[vision->botSaliency.asFovea().
                                 colour(col, row)].rgb());
            }
         }
      } else {
         // display normal images
         for (unsigned int row = 0; row < TOP_IMAGE_ROWS; ++row) {
            for (unsigned int col = 0; col < TOP_IMAGE_COLS; ++col) {
               topImage->setPixel(col, row,
                     getRGB(col, row, topFrame, TOP_IMAGE_COLS));
            }
         }
         for (unsigned int row = 0; row < BOT_IMAGE_ROWS; ++row) {
            for (unsigned int col = 0; col < BOT_IMAGE_COLS; ++col) {
               botImage->setPixel(col, row,
                     getRGB(col, row, botFrame, BOT_IMAGE_COLS));
            }
         }
      }
   } else {
      for (unsigned int row = 0;
           row < TOP_IMAGE_ROWS / TOP_SALIENCY_DENSITY; ++row) {
         for (unsigned int col = 0;
              col < TOP_IMAGE_COLS / TOP_SALIENCY_DENSITY; ++col) {
             if (0 <= topSaliency[col][row] && topSaliency[col][row] < cNUM_COLOURS) {
                topImage->setPixel(col, row,
                CPLANE_COLOURS[topSaliency[col][row]].rgb());
             } else {
                std::cerr << "Bad pixel at " << col << " " << row << std::endl;
             }
         }
      }
      for (unsigned int row = 0;
           row < BOT_IMAGE_ROWS / BOT_SALIENCY_DENSITY; ++row) {
         for (unsigned int col= 0;
              col < BOT_IMAGE_COLS / BOT_SALIENCY_DENSITY; ++col) {
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
void OverviewTab::newNaoData(NaoData *naoData) {

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
         fieldView.redraw(naoData);
         variableView.redraw(naoData);
      }
   }
}

