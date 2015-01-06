#include "FieldPainter.hpp"

#include <Eigen/QR>
#include <stdint.h>
#include "types/Point.hpp"
#include "types/BallInfo.hpp"
#include "types/PostInfo.hpp"
#include "types/RobotInfo.hpp"
#include "types/FieldEdgeInfo.hpp"
#include "types/FieldFeatureInfo.hpp"
#include "types/AbsCoord.hpp"
#include "utils/angles.hpp"
#include "utils/SPLDefs.hpp"

FieldPainter::FieldPainter (QPaintDevice *device) : QPainter (device)
{
   // determine the size of the bitmap
   int device_width = device->width();
   int device_height = device->height();
   // translate to center of field
   translate(device_width/2, device_height/2);
   // make positive y point upwards
   scale(1,-1);
   // scale so we can draw using field coordinates
   scale((float)device_width/OFFNAO_FULL_FIELD_LENGTH,
         (float)device_height/OFFNAO_FULL_FIELD_WIDTH);
}

FieldPainter::~FieldPainter () 
{
}

void FieldPainter::drawField() {

   QBrush no_brush(Qt::NoBrush);
   QBrush goal_brush(Qt::yellow);

   QPen no_pen(Qt::NoPen);

   QPen field_line_pen(QColor(255, 255, 255));
   field_line_pen.setWidth(FIELD_LINE_WIDTH);
   field_line_pen.setJoinStyle(Qt::MiterJoin);
   field_line_pen.setMiterLimit(0.5);

   QPen marker_pen(QColor(255, 255, 255));
   marker_pen.setWidth(FIELD_LINE_WIDTH);
   marker_pen.setJoinStyle(Qt::MiterJoin);
   marker_pen.setCapStyle(Qt::FlatCap);
   marker_pen.setMiterLimit(0);

   QPen crossbar_pen(Qt::yellow);
   crossbar_pen.setWidth(GOAL_POST_DIAMETER);
   marker_pen.setCapStyle(Qt::FlatCap);
   crossbar_pen.setMiterLimit(0);

   QPen goal_supports_pen(Qt::gray);
   goal_supports_pen.setWidth(GOAL_SUPPORT_DIAMETER);

   save();

   setRenderHint(QPainter::Antialiasing);

   setBrush(no_brush);
   setPen(field_line_pen);

   drawRect(-FIELD_LENGTH/2, -FIELD_WIDTH/2, FIELD_LENGTH, FIELD_WIDTH);
   drawLine(0, FIELD_WIDTH/2, 0, -FIELD_WIDTH/2);
   drawRect(-FIELD_LENGTH/2, -GOAL_BOX_WIDTH/2, GOAL_BOX_LENGTH, GOAL_BOX_WIDTH);
   drawRect(FIELD_LENGTH/2 - GOAL_BOX_LENGTH, -GOAL_BOX_WIDTH/2, GOAL_BOX_LENGTH, GOAL_BOX_WIDTH);
   drawEllipse(QPoint(0, 0), CENTER_CIRCLE_DIAMETER/2, CENTER_CIRCLE_DIAMETER/2);

   setBrush(no_brush);
   setPen(marker_pen);

   drawLine(-MARKER_DIMENSIONS/2, 0, MARKER_DIMENSIONS/2, 0);

   int penalty_x = FIELD_LENGTH/2 - DIST_GOAL_LINE_TO_MARKER;

   drawLine(penalty_x - MARKER_DIMENSIONS/2, 0, penalty_x + MARKER_DIMENSIONS/2, 0);
   drawLine(penalty_x, -MARKER_DIMENSIONS/2, penalty_x, MARKER_DIMENSIONS/2);

   drawLine(-penalty_x - MARKER_DIMENSIONS/2, 0, -penalty_x + MARKER_DIMENSIONS/2, 0);
   drawLine(-penalty_x, -MARKER_DIMENSIONS/2, -penalty_x, MARKER_DIMENSIONS/2);   

   setBrush(no_brush);
   setPen(goal_supports_pen);

   int goal_x = FIELD_LENGTH/2 + FIELD_LINE_WIDTH/2;

   drawLine(-goal_x, -GOAL_WIDTH/2, -goal_x - GOAL_DEPTH, -GOAL_WIDTH/2);
   drawLine(-goal_x,  GOAL_WIDTH/2, -goal_x - GOAL_DEPTH,  GOAL_WIDTH/2);
   drawLine( goal_x, -GOAL_WIDTH/2,  goal_x + GOAL_DEPTH, -GOAL_WIDTH/2);
   drawLine( goal_x,  GOAL_WIDTH/2,  goal_x + GOAL_DEPTH,  GOAL_WIDTH/2);
   drawLine( goal_x + GOAL_DEPTH, -GOAL_WIDTH/2,  goal_x + GOAL_DEPTH, GOAL_WIDTH/2);
   drawLine(-goal_x - GOAL_DEPTH, -GOAL_WIDTH/2, -goal_x - GOAL_DEPTH, GOAL_WIDTH/2);

   setBrush(no_brush);
   setPen(crossbar_pen);

   drawLine(-goal_x, -GOAL_WIDTH/2, -goal_x, GOAL_WIDTH/2);
   drawLine( goal_x, -GOAL_WIDTH/2,  goal_x, GOAL_WIDTH/2);

   setBrush(goal_brush);
   setPen(no_pen);

   drawEllipse(QPoint(-goal_x, -GOAL_WIDTH/2), GOAL_POST_DIAMETER/2, GOAL_POST_DIAMETER/2);
   drawEllipse(QPoint(-goal_x,  GOAL_WIDTH/2), GOAL_POST_DIAMETER/2, GOAL_POST_DIAMETER/2);
   drawEllipse(QPoint( goal_x, -GOAL_WIDTH/2), GOAL_POST_DIAMETER/2, GOAL_POST_DIAMETER/2);
   drawEllipse(QPoint( goal_x,  GOAL_WIDTH/2), GOAL_POST_DIAMETER/2, GOAL_POST_DIAMETER/2);

   restore();
}

void FieldPainter::translateRR(const RRCoord &rr, const AbsCoord &robot) {
   translate(robot.x(), robot.y());
   rotate((const float)RAD2DEG(robot.theta()));
   rotate((const float)RAD2DEG(rr.heading()));
   translate(rr.distance(), 0);
   /*
   rotate((const float)RAD2DEG(-robot.theta()));
   rotate((const float)RAD2DEG(-rr.heading()));
   */
   if (!isnan(rr.orientation())) {
      rotate(RAD2DEG(-rr.orientation()));
   }
}

void FieldPainter::translatePoint(const RRCoord &rr) {
  
   rotate((const float)RAD2DEG(rr.heading()));
   translate(rr.distance(), 0);
   if (!isnan(rr.orientation())) {
      rotate(RAD2DEG(-rr.orientation()));
   }
}

void FieldPainter::drawBallRR (const BallInfo &ball, const AbsCoord &robot)
{
   save ();
   translateRR(ball.rr, robot);
   setPen (QColor ("black"));
   setBrush(QBrush(QColor(255, 127, 0)));
   drawEllipse (QPoint(0, 0), 40, 40);
   restore ();
}

void FieldPainter::drawGoalArea(const PostInfo::Type &goalArea){
   
   save ();
   if(goalArea & PostInfo::pAway){
      setPen("white");
      setBrush(QColor("red"));
      translate(3000, 705);
      drawEllipse (QPoint(0, 0), 5, 140);
      translate(3000, -705);
      drawEllipse (QPoint(0, 0), 5, 140);

   } else if (goalArea & PostInfo::pHome){
      setPen("white");
      setBrush(QColor("blue"));
      translate(-3000, 705);
      drawEllipse (QPoint(0, 0), 5, 140);
      translate(-3000, -705);
      drawEllipse (QPoint(0, 0), 5, 140);
   }   
   restore ();
}

void FieldPainter::drawPostRR (const PostInfo &post, const AbsCoord &robot)
{
   save();
   translateRR(post.rr, robot);
   setPen("white");
   if (post.type & PostInfo::pAway) {
      setBrush(QColor("red"));
   } else if (post.type & PostInfo::pHome){
      setBrush(QColor("blue"));
   } else {
      setBrush(QColor("yellow"));
   }
   drawEllipse (QPoint(0, 0), 80, 80);
   restore();
}

void FieldPainter::drawPostAbs (const AbsCoord &post, QColor colour)
{
   save();
   translate(post.x(), post.y());
   setPen("white");
   setBrush(colour);
   drawEllipse (QPoint(0, 0), 80, 80);
   restore();
}

void FieldPainter::drawFeatureRR (const FieldFeatureInfo &feat, const AbsCoord &robot)
{
   save();
   setPen(Qt::NoPen);
   setBrush(QBrush("red"));
   translateRR(feat.rr, robot);
   QPen p;
   switch (feat.type) {
      case FieldFeatureInfo::fFieldLinePoint:
         {
         save();
         setPen("blue");
         p = pen();
         p.setWidth(25);
         setPen(p);
         setBrush(QBrush("blue"));
         for (uint16_t i = 0; i < feat.fieldlinepoints.size(); i++) {
            Point p = feat.fieldlinepoints[i].rrp;
            drawPoint (QPoint(p.x(),p.y()));
         }
         restore();
         }
         break;
      case FieldFeatureInfo::fCorner:
         setPen("black");
         p = pen();
         p.setWidth(50);
         setPen(p);
         save();
         rotate(RAD2DEG(M_PI_4));
         drawRect(25, 25, -300, -50);
         restore();
         save();
         rotate(RAD2DEG(-M_PI_4));
         drawRect(25, 25, -300, -50);
         restore();
         break;
      case FieldFeatureInfo::fTJunction:
         setPen("black");
         p = pen();
         p.setWidth(50);
         setPen(p);
         drawRect(-25, -300, 50, 600);
         drawRect(0, 25, -300, -50);
         break;
      case FieldFeatureInfo::fPenaltySpot:
         setPen("black");
         p = pen();
         p.setWidth(50);
         setPen(p);
         setBrush(Qt::NoBrush);
         drawEllipse (QPoint(-30,-30), 60,60);
         break;
      case FieldFeatureInfo::fCentreCircle:
         setPen("red");
         p = pen();
         p.setWidth(50);
         setPen(p);
         setBrush(Qt::NoBrush);
         drawEllipse (QPoint(0, 0), CENTER_CIRCLE_DIAMETER / 2, CENTER_CIRCLE_DIAMETER / 2);
         break;
      case FieldFeatureInfo::fLine:
         {
         save();
         setPen("red");
         p = pen();
         p.setWidth(50);
         setPen(p);
         setBrush(QBrush("red"));
         drawLine(feat.line.p1.x(), feat.line.p1.y(),
                  feat.line.p2.x(), feat.line.p2.y());
         restore();
         break;
         }
      case FieldFeatureInfo::fParallelLines:
         {
         save();
         setPen("black");
         p = pen();
         p.setWidth(75);
         setPen(p);
         setBrush(QBrush("black"));
         drawLine(feat.parallellines.l1.p1.x(), feat.parallellines.l1.p1.y(),
                  feat.parallellines.l1.p2.x(), feat.parallellines.l1.p2.y());
         drawLine(feat.parallellines.l2.p1.x(), feat.parallellines.l2.p1.y(),
                  feat.parallellines.l2.p2.x(), feat.parallellines.l2.p2.y());
         restore();

         break;
         }
      default:
         break;
   }
   restore();
   
}

void FieldPainter::drawFeatureAbs (const AbsCoord &pos, const FieldFeatureInfo::Type &type) {
   save();
   translate(pos.x(), pos.y());
   rotate(RAD2DEG(pos.theta()));
   setPen(Qt::NoPen);
   setBrush(QBrush("red"));
   QPen p;
   switch (type) {
      case FieldFeatureInfo::fCorner:
         save();
         rotate(RAD2DEG(M_PI_4));
         drawRect(-25, -25, 300, 50);
         restore();
         save();
         rotate(RAD2DEG(-M_PI_4));
         drawRect(-25, -25, 300, 50);
         restore();
         break;
      case FieldFeatureInfo::fTJunction:
         drawRect(-25, -300, 50, 600);
         drawRect(0, -25, 300, 50);
      case FieldFeatureInfo::fPenaltySpot:
         break;
      case FieldFeatureInfo::fCentreCircle:
         save();
         setPen("red");
         p = pen();
         p.setWidth(50);
         setPen(p);
         setBrush(Qt::NoBrush);
         drawEllipse (QPoint(0, 0), 600, 600);
         restore();
         break;
      case FieldFeatureInfo::fLine:
         break;
      default:
         break;
   }
   restore();
}

void FieldPainter::drawPlayerNumber (const AbsCoord &pos, int num) {
   save();
   translate(pos.x() - 50, pos.y() - 400);
   scale(1,-1);
   setPen("white");
   setBrush(QColor("white"));
   QFont font;
   font.setPixelSize(200);
   setFont(font);
   drawText(0, 0, QString::number(num));
   restore();
}



void FieldPainter::drawRobotAbs (const AbsCoord &pos, QColor colour, bool ellipse, QColor varColour)
{
   static const int robot_radius = 120;
   QRect roboRect(-robot_radius,-robot_radius,robot_radius*2,robot_radius*2);
   QPen variancePen(varColour);
   variancePen.setWidth(20);

   // draw pacman
   save();
   translate(pos.x(), pos.y());
   setPen("black");
   int fov;
   if (pos.var(2,2) > 0) {
      fov = MIN(160, RAD2DEG(sqrt(pos.var(2,2))));
   } else {
      fov = 0;
   }
   setBrush(QBrush(colour));
   rotate((RAD2DEG(pos.theta())-(fov/2)));
   drawPie(roboRect, 0, (360-fov)*16);
   restore();

   /* Converting covariance matrix to ellipse representation
    * Eigenvalues of the matrix are the major and minor axis lengths
    * Eigenvectors are the vectors from the center along the axes (should be perpendicular)
    */
   if (ellipse) {
      Eigen::VectorXf eigenvalues = pos.var.block<2, 2>(0,0).marked<Eigen::SelfAdjoint>().eigenvalues();
      int r_major_axis = MAX(sqrt(MAX(eigenvalues[0], eigenvalues[1])), robot_radius);
      int r_minor_axis = MAX(sqrt(MIN(eigenvalues[0], eigenvalues[1])), robot_radius);
      float theta = atan2(2*pos.var(0,1), pos.var(0,0)-pos.var(1,1))/2.0;

      // draw position covariance ellipse
      save();
      translate(pos.x(), pos.y());
      rotate(RAD2DEG(theta));
      QRect varRect(-r_major_axis, -r_minor_axis, r_major_axis*2, r_minor_axis*2);
      setPen(variancePen);
      setBrush(QBrush(Qt::NoBrush));
      drawEllipse(varRect);
      restore();
   }
}

void FieldPainter::drawRobotRR (const RobotInfo &robot, const AbsCoord &robotPos) 
{
   save ();
   translateRR(robot.rr, robotPos);
   if(robot.type == RobotInfo::rRed) {
      setBrush(QBrush(Qt::red));
   } else if(robot.type == RobotInfo::rBlue) {
      setBrush(QBrush(Qt::blue));
   } else {
      setBrush(QBrush(Qt::green));
   }
   setPen(QColor(255, 127, 0));
   drawEllipse (QPoint(0, 0), 120, 120);
   restore ();
}

void FieldPainter::drawBallPosRRC(const AbsCoord &ballPos, const AbsCoord &ballVel, bool moving, const AbsCoord &robot)
{
   save();
   translate(robot.x(), robot.y());
   rotate(RAD2DEG(robot.theta()));
   QPoint newBall = QPoint(ballPos.x(), ballPos.y());
   QPoint nextBall = QPoint(ballPos.x() + ballVel.x(), ballPos.y() + ballVel.y());
   setPen(QPen(QColor(255, 0, 0)));
   setBrush(QBrush(QColor(255, 0, 0)));
   drawEllipse(newBall, 40, 40);
   if (moving) drawLine(newBall, nextBall);
   restore();

   drawRRCovEllipse(ballPos, robot);
}

// draw robot relative position covariance ellipse
void FieldPainter::drawRRCovEllipse(const AbsCoord &pos, const AbsCoord &robotPos) {
   QPen variancePen("red");
   variancePen.setWidth(20);

   Eigen::VectorXf eigenvalues = pos.var.block<2, 2>(0,0).marked<Eigen::SelfAdjoint>().eigenvalues();
   int r_major_axis = sqrt(MAX(eigenvalues[0], eigenvalues[1]));
   int r_minor_axis = sqrt(MIN(eigenvalues[0], eigenvalues[1]));
   float theta = atan2(2*pos.var(0,1), pos.var(0,0)-pos.var(1,1))/2.0;

   save();
   translate(robotPos.x(), robotPos.y());
   rotate(RAD2DEG(robotPos.theta()));
   translate(pos.x(), pos.y());
   rotate(RAD2DEG(theta));
   QRect varRect(-r_major_axis, -r_minor_axis, r_major_axis*2, r_minor_axis*2);
   setPen(variancePen);
   setBrush(QBrush(Qt::NoBrush));
   drawEllipse(varRect);
   restore();
}

void FieldPainter::drawBallPosAbs(const AbsCoord &ball, QColor colour)
{
   save();
   QPoint newBall = QPoint(ball.x(), ball.y());
   QPen absPen(colour);
   setPen(absPen);
   setBrush(QBrush(colour));
   drawEllipse(newBall, 40, 40);
   restore();

   drawAbsCovEllipse(ball, absPen);
}

// draw absolute position covariance ellipse
void FieldPainter::drawAbsCovEllipse(const AbsCoord &pos, QPen variancePen) {
   variancePen.setWidth(20);

   Eigen::VectorXf eigenvalues = pos.var.block<2, 2>(0,0).marked<Eigen::SelfAdjoint>().eigenvalues();
   int r_major_axis = sqrt(MAX(eigenvalues[0], eigenvalues[1]));
   int r_minor_axis = sqrt(MIN(eigenvalues[0], eigenvalues[1]));
   float theta = atan2(2*pos.var(0,1), pos.var(0,0)-pos.var(1,1))/2.0;

   save();
   translate(pos.x(), pos.y());
   rotate(RAD2DEG(theta));
   QRect varRect(-r_major_axis, -r_minor_axis, r_major_axis*2, r_minor_axis*2);
   setPen(variancePen);
   setBrush(QBrush(Qt::NoBrush));
   drawEllipse(varRect);
   restore();
}

void FieldPainter::drawFieldLine(LineInfo line) {
   save();
   drawLine(line.p1.x(), line.p1.y(), line.p2.x(), line.p2.y());
   restore();
}

