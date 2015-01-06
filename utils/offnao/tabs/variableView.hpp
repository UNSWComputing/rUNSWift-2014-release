#pragma once

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <deque>
#include <string>

#include "naoData.hpp"
#include "perception/vision/yuv.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "types/BallInfo.hpp"

class VariableView : public QTreeWidget {
   public:
      VariableView();
      void redraw(NaoData *naoData);

   private:
      template <class T>
         const char* createSufPref(std::string, T, std::string);

      QTreeWidgetItem *behaviourHeading;
      QTreeWidgetItem *behaviourBodyRequest;

      QTreeWidgetItem *motionHeading;

      QTreeWidgetItem *gameControllerHeading;
      QTreeWidgetItem *gameControllerTeam;

      QTreeWidgetItem *perceptionHeading;
      QTreeWidgetItem *perceptionAverageFPS;
      QTreeWidgetItem *perceptionKinematicsTime;
      QTreeWidgetItem *perceptionVisionTime;
      QTreeWidgetItem *perceptionLocalisationTime;
      QTreeWidgetItem *perceptionBehaviourTime;
      QTreeWidgetItem *perceptionTotalTime;


      QTreeWidgetItem *visionTimestamp;
      QTreeWidgetItem *visionTurn;
      QTreeWidgetItem *visionDxdy;
      QTreeWidgetItem *visionFrames;
      QTreeWidgetItem *visionHeading;
      QTreeWidgetItem *visionGoal;
      QTreeWidgetItem *visionGoalProb;
      QTreeWidgetItem *visionHomeMapSize;
      QTreeWidgetItem *visionAwayMapSize;
      QTreeWidgetItem *visionBallPos;
      QTreeWidgetItem *visionBallPosRobotRelative;
      QTreeWidgetItem *visionFrameRate;
      QTreeWidgetItem *visionNumBalls;
      QTreeWidgetItem *visionPostType1;
      QTreeWidgetItem *visionPostType2;
      QTreeWidgetItem *visionPost1;
      QTreeWidgetItem *visionPost2;
      QTreeWidgetItem *visionPostInfo1;
      QTreeWidgetItem *visionPostInfo2;
      QTreeWidgetItem *visionNumFeet;
      QTreeWidgetItem *visionFoot1;
      QTreeWidgetItem *visionFoot2;

      QTreeWidgetItem *motionForwardOdom;
      QTreeWidgetItem *motionLeftOdom;
      QTreeWidgetItem *motionTurnOdom;
      QTreeWidgetItem *motionHeadYaw;
      QTreeWidgetItem *kinematicsHeadYaw;

      QTreeWidgetItem *localisationHeading;
      QTreeWidgetItem *localisationRobotPos;
      std::deque<int> times;
      void updateVision(NaoData *naoData);
      void updateBehaviour(NaoData *naoData);

};
