#include "OverlayPainter.hpp"

#include "perception/vision/ImageRegion.hpp"
#include "perception/vision/RobotRegion.hpp"
#include "perception/vision/VisionConstants.hpp"
#include "utils/SPLDefs.hpp"

#include "types/FootInfo.hpp"
#include "types/BallInfo.hpp"
#include "types/PostInfo.hpp"
#include "types/RobotInfo.hpp"
#include "types/FieldEdgeInfo.hpp"
#include "types/FieldFeatureInfo.hpp"
#include "types/Ipoint.hpp"


static QPoint p2q (Point p) { return QPoint (p.x (), p.y ()); }

OverlayPainter::OverlayPainter () : QPainter ()
{
}

OverlayPainter::OverlayPainter (QPaintDevice *device) : QPainter (device)
{
}

OverlayPainter::~OverlayPainter () 
{
}

void OverlayPainter::setPen(const QPen &pen)
{
   QPen newPen(pen);

   int width = pen.width();

   double zoom  = sqrt(transform().determinant());

   newPen.setWidth(width / zoom);

   QPainter::setPen(newPen);
}

void OverlayPainter::drawHorizon(std::pair<int, int> horizon)
{
   save();

   QPoint p1(0         , horizon.first );
   QPoint p2(TOP_IMAGE_COLS, horizon.second);

   setPen(QPen(QBrush("pink"), 2));
   drawLine(p1, p2);

   restore();
}

void OverlayPainter::drawFootOverlay (const FootInfo &foot)
{
   save ();

   QPoint centre = p2q (foot.imageCoords);
   int    radius = FOOT_RADIUS;

   setPen (QColor ("blue"));
   drawEllipse (centre, radius, radius);

   restore ();
}


void OverlayPainter::drawBallOverlay (const BallInfo &ball)
{
   save ();

   QPoint centre = p2q (ball.imageCoords);
   int    radius = ball.radius;

   setPen (QColor ("black"));
   drawEllipse (centre, radius, radius);

   restore ();
}

void OverlayPainter::drawPostOverlay (const PostInfo &post)
{
   save ();


   QPoint a = p2q (post.imageCoords.a);
   QPoint b = p2q (post.imageCoords.b);

   if (post.type & PostInfo::pHome) {
      setPen (QPen(Qt::blue, 2, Qt::SolidLine));
   } else if (post.type & PostInfo::pAway){
      setPen (QPen(Qt::red, 2, Qt::SolidLine));
   } else {
      setPen (QPen(Qt::black, 2, Qt::SolidLine));
   }

   drawRect (QRect (a, b));

   restore ();
}

void OverlayPainter::drawLandmarkOverlay (const Ipoint &landmark, std::pair<int, int> horizon)
{

   save ();

   setPen ("black");
   QRect rect (landmark.x*SURF_SUBSAMPLE - landmark.scale/2*SURF_SUBSAMPLE, 
      horizon.first + (horizon.second - horizon.first)*landmark.x/IMAGE_COLS*SURF_SUBSAMPLE - landmark.scale/2*SURF_SUBSAMPLE, 
      landmark.scale*SURF_SUBSAMPLE, landmark.scale*SURF_SUBSAMPLE);

   if (landmark.isRobot == 1){
      setBrush (QBrush ("pink", Qt::SolidPattern));
   } else if (landmark.laplacian == 1) {
      setBrush (QBrush ("red", Qt::SolidPattern));
   } else {
      setBrush (QBrush ("blue", Qt::SolidPattern));
   }  
   drawEllipse (rect);

   restore ();
}

void OverlayPainter::drawRobotBox(const BBox &region, const QBrush &colour) {
   const static int PEN_WIDTH = 2;
   this->save();
   QPoint a = p2q (region.a);
   QPoint b = p2q (region.b);

   this->setPen(QPen(colour, PEN_WIDTH));
   
   this->drawRect(QRect (a,b));

   this->drawLine(a, b);
   int tmp = a.x();
   a.setX(b.x());
   b.setX(tmp);
   this->drawLine(a, b);

   this->restore();
}

void OverlayPainter::drawRobotOverlay (const RobotInfo &robot)
{
    QBrush colour;
    if(robot.type == RobotInfo::rRed) {
          colour = QBrush("pink");
       } else if(robot.type == RobotInfo::rBlue) {
          colour = QBrush("#7f7fff");
       } else {
          colour = QBrush("#00ff00");
       }

   if (robot.cameras == RobotInfo::BOTH_CAMERAS) {
       drawRobotBox(robot.topImageCoords, colour);
       drawRobotBox(robot.botImageCoords, colour);
   } else if (robot.cameras == RobotInfo::TOP_CAMERA) {
       drawRobotBox(robot.topImageCoords, colour);
   } else if (robot.cameras == RobotInfo::BOT_CAMERA) {
       drawRobotBox(robot.botImageCoords, colour);
   } else if (robot.cameras == RobotInfo::OLD_DETECTION) {
       drawRobotBox(robot.imageCoords, colour);
   }


}

void OverlayPainter::drawLinePath(const QPainterPath &path, QColor colour) {
   this->save();
   this->setPen(colour);
   this->drawPath(path);
   this->restore();
}

void OverlayPainter::drawFieldEdgeOverlay (const FieldEdgeInfo &fieldEdge)
{
   QPoint p1, p2;

   /* a*x + b*y + c = 0 */
   double a = fieldEdge.imageEdge.t1;
   double b = fieldEdge.imageEdge.t2;
   double c = fieldEdge.imageEdge.t3;
   
   /* Avoid divide by 0 */
   if (a == 0 && b == 0) {
      return;
   }

   /* Line is more vertical */
   if (abs (a) > abs (b)) {
      double x, y;

      y = 0;
      x = (-c - b*y) / a;
      p1 = QPoint (x, y);

      y = TOP_IMAGE_ROWS;
      x = (-c - b*y) / a;
      p2 = QPoint (x, y);
   } else {
      double x, y;

      x = 0;
      y = (-c - a*x) / b;
      p1 = QPoint (x, y);

      x = TOP_IMAGE_COLS;
      y = (-c - a*x) / b;
      p2 = QPoint (x, y);
   }

   save ();
   setPen (QColor ("brown"));

   drawLine (p1, p2);

   restore ();
}

void OverlayPainter::drawFieldFeatureOverlay (const FieldFeatureInfo &fieldFeature)
{
}

void OverlayPainter::drawFieldFeatureOverlay (const FieldFeatureInfo &fieldFeature, QColor colour)
{
   if (fieldFeature.type == FieldFeatureInfo::fLine)
   {
      QPoint p1, p2;

      /* a*x + b*y + c = 0 */
      double a = fieldFeature.line.t1;
      double b = fieldFeature.line.t2;
      double c = fieldFeature.line.t3;
   
      /* Avoid divide by 0 */
      if (a == 0 && b == 0) {
         return;
      }

      /* Line is more vertical */
      if (abs (a) > abs (b)) {
         double x, y;

         y = 0;
         x = (-c - b*y) / a;
         p1 = QPoint (x, y);
   
         y = BOT_IMAGE_ROWS;
         x = (-c - b*y) / a;
         p2 = QPoint (x, y);
      } else {
         double x, y;
   
         x = 0;
         y = (-c - a*x) / b;
         p1 = QPoint (x, y);
   
         x = BOT_IMAGE_COLS;
         y = (-c - a*x) / b;
         p2 = QPoint (x, y);
      }

      save ();
      setPen (QColor (colour));

      drawLine (p1, p2);

      restore ();
   }
   
   if (fieldFeature.type == FieldFeatureInfo::fCorner) {
      drawPoint (fieldFeature.corner.p, "black");
   }

   if (fieldFeature.type == FieldFeatureInfo::fTJunction) {
   }

   if (fieldFeature.type == FieldFeatureInfo::fPenaltySpot) {
      int w = fieldFeature.penaltyspot.w;
      int h = fieldFeature.penaltyspot.h;
      Point centre = fieldFeature.penaltyspot.p;

      Point tl(centre.x() - w/2, centre.y() - h/2);

      drawPoint(centre, "green");

      drawRect(tl.x(), tl.y(), w, h);

   }

}

void OverlayPainter::drawImageRegion (ImageRegion *region, QColor colour)
{
   int x = region->topMost.first;
   int y = region->topMost.second;
   int w = region->bottomMost.first - x;
   int h = region->bottomMost.second - y;

   save ();
   //scale (SALIENCY_DENSITY, SALIENCY_DENSITY);

   setPen (colour);
   drawRect (x, y, w, h);

   restore ();
}

void OverlayPainter::drawRobotRegion (RobotRegion *region, QColor colour)
{
   int x = region->topMost;
   int y = region->leftMost;
   int w = region->rightMost - x;
   int h = region->bottomMost - y;

   save ();
   //scale (SALIENCY_DENSITY, SALIENCY_DENSITY);

   setPen (colour);
   drawRect (x, y, w, h);

   restore ();
}

void OverlayPainter::drawPoint (Point p, QColor colour)
{
   QRect rect (p.x () - 1, p.y () - 1, 3, 3);

   save ();

   setPen ("black");
   setBrush (QBrush (colour, Qt::SolidPattern));
   drawEllipse (rect);

   restore ();
}

/**
 * Initialize colour mappings
 */
const OverlayPainter::colour_pair
OverlayPainter::colour_arr[fNUM_FEATURES] =
   { colour_pair(fBALL                    , QColor(225, 114,   0)),
     colour_pair(fGOAL_POST_BLUE          , QColor( 63, 162, 255)),
     colour_pair(fGOAL_BAR_BLUE           , QColor( 63, 162, 255)),
     colour_pair(fGOAL_POST_YELLOW        , QColor(237, 229,   0)),
     colour_pair(fGOAL_BAR_YELLOW         , QColor(237, 229,   0)),
     colour_pair(fROBOT_BLUE              , QColor(  0,   5, 203)),
     colour_pair(fROBOT_RED               , QColor(188,   0,   0)),
     colour_pair(fFIELD_LINE              , QColor(255, 255, 255)),
     colour_pair(fFIELD_LINE_THIN         , QColor(255, 255, 255)),
     colour_pair(fFIELD_CORNER            , QColor(255, 255, 255)),
     colour_pair(fFIELD_T_JUNCTION        , QColor(255, 255, 255)),
     colour_pair(fFIELD_PENALTY_SPOT      , QColor(255, 255, 255)),
     colour_pair(fFIELD_CENTRE_CIRCLE     , QColor(255, 255, 255)),
     colour_pair(fFIELD_EDGE              , QColor(  0, 255,   0))
   };

const std::vector<OverlayPainter::colour_pair>
OverlayPainter::colour_vector(colour_arr, colour_arr + fNUM_FEATURES);

const std::map<feature_type, QColor>
OverlayPainter::colour_map(colour_vector.begin(), colour_vector.end());

