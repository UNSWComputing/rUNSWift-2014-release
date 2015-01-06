#pragma once

#include <QPainter>
#include <QRectF>

class BallInfo;
class PostInfo;
class AbsCoord;
class RRCoord;

#include "types/FieldFeatureInfo.hpp"
#include "types/PostInfo.hpp"
#include "types/RobotInfo.hpp"
               
class FieldPainter : private QPainter
{
   public:
      FieldPainter (QPaintDevice * device);
      virtual ~FieldPainter ();

      void drawField(void);

      void drawGoalArea(const PostInfo::Type &goalArea);
      void drawBallRR(const BallInfo &ball, const AbsCoord &robot);
      void drawPostRR(const PostInfo &post, const AbsCoord &robot);
      void drawPostAbs (const AbsCoord &post, QColor colour);
      void drawFeatureRR (const FieldFeatureInfo &feat, const AbsCoord &robot);
      void drawFeatureAbs (const AbsCoord &pos, const FieldFeatureInfo::Type &type);
      void drawPlayerNumber(const AbsCoord &pos, int num);
      void drawRobotAbs(const AbsCoord &pos, QColor colour, bool drawEllipse=true, QColor varColour="black");
      void drawRobotRR (const RobotInfo &robot, const AbsCoord &robotPos);

      void drawBallPosRRC (const AbsCoord &ballPos, const AbsCoord &ballVel, bool moving, const AbsCoord &robotPos);
      void drawBallPosAbs (const AbsCoord &ball, QColor colour=QColor(0,0,255));

      void drawRRCovEllipse (const AbsCoord &pos, const AbsCoord &robotPos);
      void drawAbsCovEllipse (const AbsCoord &pos, QPen variancePen);

      void drawFieldLine(LineInfo line);

//      void drawTeamBall (const AbsCoord &ball, const AbsCoord &robotPos);
//      void drawBallPosAbs(const AbsCoord &bPos, float bVelX, float bVelY, const AbsCoord &robotPos);
//      void drawUkfBallRr(float bPosX, float bPosY, float bVelX, float bVelY, const AbsCoord &robot);
//      void drawAbsCovEllipse(const AbsCoord &pos, const AbsCoord &robotPos);
//      void drawRRCovEllipse(const AbsCoord &pos, const AbsCoord &robotPos);

   private:
      void translateRR(const RRCoord &rr, const AbsCoord &robot);
      void translatePoint(const RRCoord &rr);
      /* converts RRCoord to x y coordinates */
      QPoint PToC(const RRCoord &rr);
};

