#include "icpFieldView.hpp"
#include <QColor>
#include <QBrush>
#include <QImage>
#include <QRectF>

#include "utils/FieldPainter.hpp"
#include "perception/vision/yuv.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "perception/vision/Vision.hpp"
#include "perception/vision/RobotRegion.hpp"
#include "perception/localisation/LocalisationDefs.hpp"
#include "utils/Logger.hpp"
#include "utils/incapacitated.hpp"
#include "blackboard/Blackboard.hpp"
#include "progopts.hpp"
#include "perception/localisation/ICP.hpp"

using namespace std;

ICPFieldView::ICPFieldView() : image(":/images/spl_field.svg") {
   renderPixmap = new QPixmap(640, 480);

   imagePixmap = QPixmap(640, 480);
   QRectF target(0.0, 0.0, 640.0, 480.0);
   QRectF source(0.0, 0.0, 740, 540.0);
   QPainter painter(&imagePixmap);
   painter.drawImage(target, image, source);

   renderPixmap->fill(Qt::darkGray);
   setPixmap(*renderPixmap);
   setMinimumSize(640, 480);
   setMaximumSize(640, 480);
   setAlignment(Qt::AlignTop);
}

ICPFieldView::~ICPFieldView() {
}

void ICPFieldView::redraw() {
   *renderPixmap = imagePixmap;
   setPixmap (*renderPixmap);
   return;
}

void ICPFieldView::redraw(Blackboard *blackboard,
                          const AbsCoord &robotPos,
                          int icpResult,
                          const AbsCoord &icpObs,
                          const AbsCoord &ballRRC,
                          const AbsCoord &teamBall) {

   *renderPixmap = imagePixmap;
   FieldPainter painter(renderPixmap);

   AbsCoord anchor;
   if (icpResult >= ICP_LOCALISED){
      anchor = icpObs;
   } else {
      anchor = robotPos;
   }

   /* Draw features with absolute coordinates */
   if (!isnan(robotPos.theta()) && !isnan(robotPos.x()) && !isnan(robotPos.y())) {

      /* Draw the team ball */
      if (!isnan(teamBall.x()) && !isnan(teamBall.y())){   
         painter.drawBallPosAbs(teamBall, QColor(255, 0, 255));
      }
      /* Draw our position */
      painter.drawRobotAbs(robotPos, "#ffee00", true, "black"); // pacman yellow, black variance

      if (icpResult >= ICP_LOCALISED) {
    
         /* Draw icp's chosen/closest observation on top of the others */
         if (!isnan(icpObs.theta()) && !isnan(icpObs.x()) && !isnan(icpObs.y())) {
            painter.drawRobotAbs(icpObs, "blue", true, "blue");
         }
      }
   }

   /* Draw features with robot-relative coordinates */
   if ((!isnan(anchor.theta()) && !isnan(anchor.x()) && !isnan(anchor.y()))) {
      /* Posts */
      std::vector<PostInfo> posts = readFrom(vision, posts);
      std::vector<PostInfo>::iterator post_i;
      for (post_i = posts.begin (); post_i < posts.end (); ++ post_i) {
         painter.drawPostRR(*post_i, anchor);
      }

      /* Ball */
      AbsCoord bVel;
      if (!isnan(ballRRC.x()) && !isnan(ballRRC.y())){
         painter.drawBallPosRRC(ballRRC, bVel, false, anchor);
      }


      /* Field Lines RR */
      std::vector<FieldFeatureInfo> features = readFrom(vision,fieldFeatures);
      std::vector<FieldFeatureInfo>::iterator feature_i;
      for (feature_i = features.begin (); feature_i < features.end ();
            ++ feature_i) {
         painter.drawFeatureRR(*feature_i, anchor);
      }
 /*     RRCoord fieldPoints[MAX_FIELD_LINE_POINTS];
      readArray(vision, fieldLinePoints, fieldPoints);
      for (uint16_t i = 0; i < MAX_FIELD_LINE_POINTS; i++) {
         std::cout << "point with d = " << fieldPoints[i].distance()
             << " and h = " << fieldPoints[i].heading() << std::endl;
         CentreCircleInfo c = CentreCircleInfo();
         FieldFeatureInfo f = FieldFeatureInfo(fieldPoints[i], c);
         painter.drawFeatureRR(f, anchor);
      }
*/
   }

   /* Draw ICP lines */
   //static std::vector<LineInfo> allFieldLines; // in decreasing order of length
   std::vector<LineInfo>::const_iterator line = ICP::getAllFieldLines().begin();
   for (; line != ICP::getAllFieldLines().end(); ++line) {
      painter.drawFieldLine(*line);
   }


   setPixmap (*renderPixmap);
   return;
}

