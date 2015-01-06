#include "fieldView.hpp"
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
#include "types/RobotObstacle.hpp"


using namespace std;

FieldView::FieldView() : image(":/images/spl_field.svg") {
   renderPixmap = new QPixmap(640, 480);

   imagePixmap = QPixmap(640, 480);
   imagePixmap.fill(QColor(0, 128, 0));
   FieldPainter painter(&imagePixmap);
   painter.drawField();

   renderPixmap->fill(Qt::darkGray);
   setPixmap(*renderPixmap);
   setMinimumSize(640, 480);
   setMaximumSize(640, 480);
   setAlignment(Qt::AlignTop);
}

FieldView::~FieldView() {
}

// Used by the team tab
void FieldView::redraw(std::vector<AbsCoord> &robotPos,
                       std::vector<AbsCoord> &ballPos){

   *renderPixmap = imagePixmap;
   FieldPainter painter(renderPixmap);  
   if(robotPos.size() > 0){
     for (unsigned int i = 0; i < robotPos.size(); ++i) {
        if (!isnan(robotPos[i].theta()) &&
            !isnan(robotPos[i].x()) &&
            !isnan(robotPos[i].y())) {
           painter.drawRobotAbs(robotPos[i], "#ffee00", true, "black");
           painter.drawPlayerNumber(robotPos[i], i+1);
        }
     }
   }

   if (ballPos.size() > 0){
     for (unsigned int i = 0; i < ballPos.size(); ++i) {
        if (!isnan(ballPos[i].x()) &&
            !isnan(ballPos[i].y())) {
           painter.drawBallPosAbs(ballPos[i]);
        }
     }
   }
   setPixmap (*renderPixmap);

}

void FieldView::redraw(std::vector<AbsCoord> &robotPos){

  *renderPixmap = imagePixmap;
   if(robotPos.size() > 0){
     FieldPainter painter(renderPixmap);  
     for (unsigned int i = 1; i < robotPos.size(); ++i) {
        if (!isnan(robotPos[i].theta()) && !isnan(robotPos[i].x()) && !isnan(robotPos[i].y())) {
//           painter.drawRobotAbs(robotPos[i], "blue", true);
           painter.drawRobotAbs(robotPos[i], "black", true);
        }
     }

     if (!isnan(robotPos[0].theta()) && !isnan(robotPos[0].x()) && !isnan(robotPos[0].y())) {
        painter.drawRobotAbs(robotPos[0], "#ffee00", true); // pacman yellow

     }
   }
   setPixmap (*renderPixmap);

}


void FieldView::redraw(NaoData *naoData) {
   *renderPixmap = imagePixmap;
   Blackboard *blackboard;

   if (! (naoData && (blackboard = naoData->getCurrentFrame ().blackboard))) {
      setPixmap (*renderPixmap);
      return;
   }

   FieldPainter painter(renderPixmap);

   bool teamBallExperiment = false;
   if (teamBallExperiment) {
      const AbsCoord robotPos = readFrom(localisation, robotPos);
      if (!isnan(robotPos.theta()) && !isnan(robotPos.x()) && !isnan(robotPos.y())) {
         // Draw our position
         painter.drawRobotAbs(robotPos, "blue", true, "blue");

         // Draw our ball observation
         AbsCoord bPos = readFrom(localisation, ballPos);
         if (!isnan(bPos.x()) && !isnan(bPos.y())){
            painter.drawBallPosAbs(bPos, "yellow");
         }
      }
      // Draw team's ball observations
      int playerNumber = readFrom(gameController, player_number);
      BroadcastData teamData[ROBOTS_PER_TEAM];
      readArray(receiver, data, teamData);
      bool incapacitated[ROBOTS_PER_TEAM];
      readArray(receiver, incapacitated, incapacitated);
      for (int playerIndex = 0; playerIndex < ROBOTS_PER_TEAM; playerIndex++) {
         if (!(incapacitated[playerIndex] || (playerIndex == playerNumber - 1))) {
            painter.drawRobotAbs(teamData[playerIndex].robotPos, "blue", true, "blue"); 
            if (!isnan(teamData[playerIndex].ballPosAbs.x())) {
               painter.drawBallPosAbs(teamData[playerIndex].ballPosAbs, "yellow");
            }
         }
      }
      // Draw team ball
      TeamBallInfo teamBall = readFrom(localisation, teamBall);
      if (teamBall.contributors > 0){
         painter.drawBallPosAbs(teamBall.pos, QColor(255, 0, 255));
      }
      setPixmap (*renderPixmap);
      return;
   }

   /* Draw goal area indication */
   painter.drawGoalArea(readFrom(vision, goalArea));


   /* Draw features with absolute coordinates */
   const AbsCoord robotPos = readFrom(localisation, robotPos);
   if (!isnan(robotPos.theta()) && !isnan(robotPos.x()) && !isnan(robotPos.y())) {
      painter.drawRobotAbs(robotPos, "#ffee00", true, "black"); // pacman yellow, black variance

      /* Draw filtered absolute ball position */
      AbsCoord bPos = readFrom(localisation, ballPos);
      if (!isnan(bPos.x()) && !isnan(bPos.y())){
         painter.drawBallPosAbs(bPos);
      }
   }

   /* Draw received ball observations and team ball */
   int playerNumber = readFrom(gameController, player_number);
   BroadcastData teamData[ROBOTS_PER_TEAM];
   readArray(receiver, data, teamData);
   bool incapacitated[ROBOTS_PER_TEAM];
   readArray(receiver, incapacitated, incapacitated);
   for (int playerIndex = 0; playerIndex < ROBOTS_PER_TEAM; playerIndex++) {
      if (!(incapacitated[playerIndex] || 
            playerIndex == playerNumber - 1 || 
            isnan(teamData[playerIndex].ballPosAbs.x()))){
         painter.drawBallPosAbs(teamData[playerIndex].ballPosAbs, "yellow");
      }
   }

   AbsCoord pos = robotPos;
   bool drawRR = true;

   /* Draw features with robot-relative coordinates */
   if (drawRR) {

      /* Balls */
      std::vector<BallInfo> balls = readFrom(vision, balls);
      std::vector<BallInfo>::iterator ball_i;
      for (ball_i = balls.begin (); ball_i != balls.end (); ++ball_i) {
         painter.drawBallRR(*ball_i, pos);
      }

      /* Posts */
      std::vector<PostInfo> posts = readFrom(vision, posts);
      std::vector<PostInfo>::iterator post_i;
      for (post_i = posts.begin (); post_i < posts.end (); ++ post_i) {
         painter.drawPostRR(*post_i, pos);
      }

      /* Robots */
//       const std::vector<RobotInfo>& robots = readFrom(vision, robots);
//       for(unsigned int robot_i = 0; robot_i < robots.size (); ++ robot_i) {
//          painter.drawRobotRR(robots[robot_i], pos);
//       }

      /* Filtered Robots */
      vector<RobotObstacle> robotObstacles = readFrom(localisation, robotObstacles);
      vector<RobotObstacle>::iterator robotobs_i;
      for(robotobs_i = robotObstacles.begin(); robotobs_i != robotObstacles.end(); ++ robotobs_i) {
         float absX   = robotobs_i->rr.distance() * cos(robotobs_i->rr.heading() + pos.theta());
         float absY   = robotobs_i->rr.distance() * sin(robotobs_i->rr.heading() + pos.theta());
         absX        += pos.x();
         absY        += pos.y();
         AbsCoord absRobot(absX, absY, 0);
         Eigen::Matrix<float, 2, 2> rotation = Eigen::Rotation2D<float>(pos.theta()).toRotationMatrix();         
         absRobot.var.block(0,0,2,2) = rotation * robotobs_i->rr.var.block(0,0,2,2) * rotation.inverse();
         absRobot.var(2,2) = 0.f;
         
         if(robotobs_i->type == RobotInfo::rRed) {
            painter.drawRobotAbs(absRobot, "pink");
         } else if(robotobs_i->type == RobotInfo::rBlue) {
            painter.drawRobotAbs(absRobot, "#7f7fff");
         } else {
            painter.drawRobotAbs(absRobot, "#00ff00");
         }
      }

      /* Field Lines RR */
      std::vector<FieldFeatureInfo> features = readFrom(vision,fieldFeatures);
      std::vector<FieldFeatureInfo>::iterator feature_i;
      for (feature_i = features.begin (); feature_i < features.end ();
            ++ feature_i) {
         painter.drawFeatureRR(*feature_i, pos);
      }
 /*     RRCoord fieldPoints[MAX_FIELD_LINE_POINTS];
      readArray(vision, fieldLinePoints, fieldPoints);
      for (uint16_t i = 0; i < MAX_FIELD_LINE_POINTS; i++) {
         std::cout << "point with d = " << fieldPoints[i].distance()
             << " and h = " << fieldPoints[i].heading() << std::endl;
         CentreCircleInfo c = CentreCircleInfo();
         FieldFeatureInfo f = FieldFeatureInfo(fieldPoints[i], c);
         painter.drawFeatureRR(f, pos);
      }
*/
   }

   setPixmap (*renderPixmap);
   return;
}

