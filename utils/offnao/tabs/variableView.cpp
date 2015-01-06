#include "variableView.hpp"
#include <QTreeWidgetItem>
#include <QStringList>
#include <QString>
#include <string>
#include <utility>
#include "types/ActionCommand.hpp"
#include "blackboard/Blackboard.hpp"
#include "perception/vision/Vision.hpp"


using namespace std;

VariableView::VariableView() {
   perceptionHeading = new QTreeWidgetItem(this, QStringList(QString("Perception")), 1);
   perceptionHeading->setExpanded(true);
   perceptionAverageFPS  = new QTreeWidgetItem(perceptionHeading,
         QStringList(QString("Framerate: ")), 1);
   perceptionKinematicsTime = new QTreeWidgetItem(perceptionHeading,
         QStringList(QString("Kinematics time: ")), 1);
   perceptionVisionTime = new QTreeWidgetItem(perceptionHeading,
         QStringList(QString("Vision time: ")), 1);
   perceptionLocalisationTime = new QTreeWidgetItem(perceptionHeading,
         QStringList(QString("Localisation time: ")), 1);
   perceptionBehaviourTime = new QTreeWidgetItem(perceptionHeading,
         QStringList(QString("Behaviour time: ")), 1);
   perceptionTotalTime = new QTreeWidgetItem(perceptionHeading,
         QStringList(QString("Total time: ")), 1);

   visionHeading = new QTreeWidgetItem(this, QStringList(QString("Vision")), 1);
   visionHeading->setExpanded(true);

   visionTimestamp = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Vision Timestamp = ")), 1);
   visionTurn = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Visual Odom Turn = ")), 1);
   visionDxdy = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Vision DxDy = ")), 1);
   visionFrames = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Odom Missed Frames = ")), 1);
   visionGoalProb = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Away Goal Prob = ")), 1);
   visionHomeMapSize = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Home Map Size = ")), 1);
   visionAwayMapSize = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Away Map Size = ")), 1);
   visionGoal = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Goal Area = ")), 1);
   visionNumBalls = new QTreeWidgetItem(visionHeading,
         QStringList(QString("# Balls = ")), 1);
   visionBallPos = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Ball Pos = ")), 1);
   visionBallPosRobotRelative  = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Robot relative Ball Pos = ")), 1);
   visionPostType1 = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Post Type 1 = ")), 1);
   visionPost1 = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Post 1 @ ")), 1);
   visionPostInfo1 = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Post 1 - ")), 1);
   visionPostType2 = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Post Type 2 = ")), 1);
   visionPost2 = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Post 2 @ ")), 1);
   visionPostInfo2 = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Post 2 - ")), 1);
   visionNumFeet = new QTreeWidgetItem(visionHeading,
         QStringList(QString("NumFeet = ")), 1);
   visionFoot1 = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Foot1 = ")), 1);
   visionFoot2 = new QTreeWidgetItem(visionHeading,
         QStringList(QString("Foot2 = ")), 1);

   this->setHeaderLabel(QString("State variables"));

   localisationHeading = new QTreeWidgetItem(this,
         QStringList(QString("Localisation")), 1);
   localisationHeading->setExpanded(true);
   localisationRobotPos = new QTreeWidgetItem(localisationHeading,
         QStringList(QString("Robot pos = ")), 1);

   motionHeading = new QTreeWidgetItem(this, QStringList(QString("Motion")), 1);
   motionHeading->setExpanded(true);

   motionForwardOdom = new QTreeWidgetItem(motionHeading,
         QStringList(QString("Fwd = ")), 1);
   motionLeftOdom = new QTreeWidgetItem(motionHeading,
         QStringList(QString("Left = ")), 1);
   motionTurnOdom = new QTreeWidgetItem(motionHeading,
         QStringList(QString("Turn = ")), 1);
   motionHeadYaw = new QTreeWidgetItem(motionHeading,
         QStringList(QString("HeadYaw = ")), 1);
   kinematicsHeadYaw = new QTreeWidgetItem(motionHeading,
         QStringList(QString("HeadYawK = ")), 1);

   behaviourHeading = new QTreeWidgetItem(this,
                                          QStringList(QString("Behaviour")), 1);
   behaviourBodyRequest = new QTreeWidgetItem(behaviourHeading,
                     QStringList(QString("request.body.actionType = ")), 1);
   behaviourHeading->setExpanded(true);

   gameControllerHeading = new QTreeWidgetItem(this,
                                          QStringList(QString("GameController")), 1);
   gameControllerTeam = new QTreeWidgetItem(gameControllerHeading,
                     QStringList(QString("gameController.team_red = ")), 1);
   gameControllerHeading->setExpanded(true);

   this->addTopLevelItem(visionHeading);
   this->addTopLevelItem(localisationHeading);
   this->addTopLevelItem(motionHeading);
   this->addTopLevelItem(behaviourHeading);
}

void VariableView::redraw(NaoData *naoData) {
   Frame frame = naoData->getCurrentFrame();
   Blackboard *blackboard = (frame.blackboard);
   if(!blackboard) return;

   // struct RobotPos wrapperPos[4];
   // readArray(localisation, robot, wrapperPos);
   AbsCoord pos = readFrom(localisation, robotPos);
   stringstream spos;

   uint32_t ballLostCount = readFrom(localisation, ballLostCount);
   AbsCoord ballPos = readFrom(localisation, ballPos);
   AbsCoord ballVelRRC = readFrom(localisation, ballVelRRC);
   
   spos << " Robot @ (" << pos.x() << ", " << pos.y() << ")" << endl;
   spos << "Heading: " << pos.theta() << " (" << RAD2DEG(pos.theta()) << " deg)" << endl;
   spos << "Variance: " << pos.var(0,0) << ", " << pos.var(1,1) << ", " << pos.var(2,2) << endl;

   spos << "ballLostCount: " << ballLostCount << endl;
   spos << "ball pos; x:" << ballPos.x() << ", y:" << ballPos.y() << endl;
   spos << "ball var; xx:" << ballPos.var(0,0) << ", xy:" << ballPos.var(0,1) << endl;
   spos << "              yx:" << ballPos.var(1,0) << ", yy:" << ballPos.var(1,1) << endl;
   spos << "ball vel rrc; x':" << ballVelRRC.x() << ", y':" << ballVelRRC.y() << endl;
   spos << "ball vel rrc var; x'x':" << ballVelRRC.var(0,0) << ", x'y':" << ballVelRRC.var(0,1) << endl;
   spos << "              y'x':" << ballVelRRC.var(1,0) << ", y'y':" << ballVelRRC.var(1,1) << endl;

   localisationRobotPos->setText(0, spos.str().c_str());

   updateVision(naoData);
   updateBehaviour(naoData);
   stringstream steam;
   steam << "gamecontroller.team_red = " << readFrom(gameController, team_red) << endl;
   gameControllerTeam->setText(0, steam.str().c_str());
}

template <class T>
const char* VariableView::createSufPref(string pref, T t, string suff) {
   stringstream s;
   s << pref << t << suff;
   return s.str().c_str();
}

void VariableView::updateBehaviour(NaoData *naoData) {
   Blackboard *blackboard = (naoData->getCurrentFrame().blackboard);
   if (!blackboard) return;

   string actionName;
   //switch (readFrom(behaviour, request[readFrom(behaviour, readBuf)]).actions.body.actionType) {
   switch (readFrom(motion, active).body.actionType) {
   case ActionCommand::Body::NONE: actionName = "NONE"; break;
   case ActionCommand::Body::STAND: actionName = "STAND"; break;
   case ActionCommand::Body::WALK: actionName = "WALK"; break;
   case ActionCommand::Body::DRIBBLE: actionName = "DRIBBLE"; break;
   case ActionCommand::Body::GETUP_FRONT: actionName = "GETUP_FRONT"; break;
   case ActionCommand::Body::GETUP_BACK: actionName = "GETUP_BACK"; break;
   case ActionCommand::Body::KICK: actionName = "KICK"; break;
   case ActionCommand::Body::INITIAL: actionName = "INITIAL"; break;
   case ActionCommand::Body::DEAD: actionName = "DEAD"; break;
   case ActionCommand::Body::REF_PICKUP: actionName = "REF_PICKUP"; break;
   case ActionCommand::Body::OPEN_FEET: actionName = "OPEN_FEET"; break;
   case ActionCommand::Body::THROW_IN: actionName = "THROW_IN"; break;
   case ActionCommand::Body::GOALIE_SIT: actionName = "GOALIE_SIT"; break;
   case ActionCommand::Body::GOALIE_DIVE_RIGHT: actionName = "GOALIE_DIVE_RIGHT"; break;
   case ActionCommand::Body::GOALIE_DIVE_LEFT: actionName = "GOALIE_DIVE_LEFT"; break;
   case ActionCommand::Body::GOALIE_CENTRE: actionName = "GOALIE_CENTRE"; break;
   case ActionCommand::Body::GOALIE_UNCENTRE: actionName = "GOALIE_UNCENTRE"; break;
   case ActionCommand::Body::GOALIE_STAND: actionName = "GOALIE_STAND"; break;
   case ActionCommand::Body::GOALIE_INITIAL: actionName = "GOALIE_INITIAL"; break;
   case ActionCommand::Body::GOALIE_AFTERSIT_INITIAL: actionName = "GOALIE_AFTERSIT_INITIAL"; break;

   default: actionName = "Other"; break;
   }
   behaviourBodyRequest->setText(0, createSufPref("request.body.actionType = ",
                                                  actionName, ""));
}

void VariableView::updateVision(NaoData *naoData) {
   Blackboard *blackboard = (naoData->getCurrentFrame().blackboard);
   if (!blackboard) return;

   std::vector<FootInfo>         feet          = readFrom (vision, feet);
   std::vector<BallInfo>         balls         = readFrom (vision, balls);
   BallHint                      ballHint      = readFrom (vision, ballHint);
   std::vector<PostInfo>         posts         = readFrom (vision, posts);
   std::vector<RobotInfo>        robots        = readFrom (vision, robots);
   std::vector<FieldEdgeInfo>    fieldEdges    = readFrom (vision, fieldEdges);
   std::vector<FieldFeatureInfo> fieldFeatures = readFrom (vision, fieldFeatures);
   Odometry                      dualOdometry  = readFrom (vision, dualOdometry);
   Odometry                      motionOdometry  = readFrom (motion, odometry);
   bool                          caughtLeft    = readFrom (vision, caughtLeft);
   bool                          caughtRight   = readFrom (vision, caughtRight);
   std::pair<int, int>           dxdy          = readFrom (vision, dxdy);

   float headYaw  = readFrom(motion, sensors).joints.angles[Joints::HeadYaw];
   float headYawK  = readFrom(kinematics, sensorsLagged).joints.angles[Joints::HeadYaw];
   uint32_t total = readFrom(perception, total);

   times.push_back(total);
   if (times.size() > 10) times.pop_front();
   int sum = 0;
   for (unsigned int i = 0; i < times.size(); i++) sum += times[i];
   float fps = 1000000.0/total;


   perceptionAverageFPS->setText(0, createSufPref("Framerate: ", fps, " fps"));
   perceptionKinematicsTime->setText(0, createSufPref("Kinematics time: ", readFrom(perception, kinematics), ""));
   perceptionVisionTime->setText(0, createSufPref("Vision time: ", readFrom(perception, vision), ""));
   perceptionLocalisationTime->setText(0, createSufPref("Localisation time: ", readFrom(perception, localisation), ""));
   perceptionBehaviourTime->setText(0, createSufPref("Behaviour time: ", readFrom(perception, behaviour), ""));
   perceptionTotalTime->setText(0, createSufPref("Total time: ", total, ""));

   visionTimestamp->setText(0, createSufPref("Timestamp: ", readFrom(vision, timestamp), ""));
   if(caughtLeft){
      visionTurn->setText(0, createSufPref("Caught on LHS!\nDual Odom Turn: ", RAD2DEG(dualOdometry.turn), " deg"));
   } else if (caughtRight){
      visionTurn->setText(0, createSufPref("Caught on RHS!\nDual Odom Turn: ", RAD2DEG(dualOdometry.turn), " deg"));
   } else {
      visionTurn->setText(0, createSufPref("Not Caught.\nDual Odom Turn: ", RAD2DEG(dualOdometry.turn), " deg"));
   }
   visionFrames->setText(0, createSufPref("Odom missed frames: ", readFrom(vision, missedFrames), ""));
   visionGoalProb->setText(0, createSufPref("Away Goal Prob: ", readFrom(vision, awayGoalProb), ""));
   visionHomeMapSize->setText(0, createSufPref("Home Map Size: ", readFrom(vision, homeMapSize), ""));
   visionAwayMapSize->setText(0, createSufPref("Away Map Size: ", readFrom(vision, awayMapSize), ""));
   visionGoal->setText(0, createSufPref("Goal Area: ", PostInfo::TypeName[readFrom(vision, goalArea)], ""));

   stringstream sdxdy;
   sdxdy << "Vision DxDy: (" << dxdy.first << "," << dxdy.second << ")";
   visionDxdy->setText(0, sdxdy.str().c_str());


   stringstream sBallPos;

   sBallPos << "BallHint: " << BallHint::TypeName[ballHint.type] << "\n";

   int numBalls = balls.size ();
   if (numBalls == 0) {
   } else {
      Point ballLocation = balls[0].imageCoords;
      sBallPos << "Ball is at (" << ballLocation.x() <<
         ", " << ballLocation.y () << ")";
   }
   visionBallPos->setText(0, sBallPos.str().c_str());

   stringstream s;

   int numEdges = fieldEdges.size ();
   RANSACLine edges[MAX_FIELD_EDGES];

   for (int i = 0; i < numEdges; ++ i) {
      edges[i] = fieldEdges[i].rrEdge;
   }


   s << "numEdges " << numEdges << endl;
   for (int i = 0; i < numEdges; i++) {
      s << "Edge 1: " << edges[i].t1 << "x + " << edges[i].t2 <<
         "y + " << edges[i].t3 << endl;
   }

   s.str ("");
   s << "Post Type 1 = " << PostInfo::TypeName[PostInfo::pNone]; 
   visionPostType1->setText (0, s.str ().c_str ());

   s.str ("");
   s << "Post Type 2 = " << PostInfo::TypeName[PostInfo::pNone]; 
   visionPostType2->setText (0, s.str ().c_str ());

   visionPost1->setText (0, "Post 1 @ ");
   visionPost2->setText (0, "Post 2 @ ");
   visionPostInfo1->setText (0, "P1 - ");
   visionPostInfo2->setText (0, "P2 - ");


   if (posts.size () > 0) {
      PostInfo p = posts[0];
      RRCoord rr = posts[0].rr;

      s.str ("");
      s << "Post Type 1 = " << PostInfo::TypeName[posts[0].type]; 
      visionPostType1->setText (0, s.str ().c_str ());

      s.str ("");
      s << "Post 1 @ " 
        << rr.distance() << ", " << RAD2DEG(rr.heading()) << ", " << rr.heading()
        << ", (" << rr.var(0,0) << ", " << rr.var(1,1) << ")";
      visionPost1->setText (0, s.str ().c_str ());

      s.str ("");
      s << "P1 - Trust = " << p.trustDistance;
      s << ", dir = " << PostInfo::DirName[p.dir];
      visionPostInfo1->setText (0, s.str ().c_str ());
   }

   if (posts.size () > 1) {
      PostInfo p = posts[1];
      RRCoord rr = posts[1].rr;

      s.str ("");
      s << "Post Type 2 = " << PostInfo::TypeName[posts[1].type]; 
      visionPostType2->setText (0, s.str ().c_str ());

      s.str ("");
      s << "Post 2 @ " 
        << rr.distance() << ", " << RAD2DEG(rr.heading()) << ", " << rr.heading()
        << ", (" << rr.var(1,1) << ", " << rr.var(2,2) << ")";
      visionPost2->setText (0, s.str ().c_str ());

      s.str ("");
      s << "P2 - Trust = " << p.trustDistance;
      s << ", dir = " << PostInfo::DirName[p.dir];
      visionPostInfo2->setText (0, s.str ().c_str ());

   }

   stringstream sBallPosRelative;
   if(numBalls > 0) {
      sBallPosRelative << "Ball @ " << balls[0].rr.distance() << ", " <<
         RAD2DEG(balls[0].rr.heading()) << ", " << balls[0].rr.heading();
   }
   visionBallPosRobotRelative->setText(0, sBallPosRelative.str().c_str());

   this->visionNumBalls->setText(0,createSufPref("# Balls: ", numBalls, ""));

   int numFeet = feet.size();
   this->visionNumFeet->setText(0,createSufPref("# Feet: ", numFeet, ""));

   stringstream foot1;
   foot1 << "Foot 1: ";
   stringstream foot2;
   foot2 << "Foot 2: ";
   if (numFeet > 0) {
      foot1 << "(" << feet[0].imageCoords[0] << "," << feet[0].imageCoords[1] << ")" << endl;
      foot1 << "  d:" << feet[0].rr.distance() << " h:" << feet[0].rr.heading();
      if (numFeet > 1) {
         foot2 << "(" << feet[1].imageCoords[0] << "," << feet[1].imageCoords[1] << ")" << endl;
         foot2 << "  d:" << feet[1].rr.distance() << " h:" << feet[1].rr.heading();
      }
   }
   visionFoot1->setText(0, foot1.str().c_str());
   visionFoot2->setText(0, foot2.str().c_str());

   stringstream odom;
   odom.str("");
   odom << "Fwd = " << motionOdometry.forward;
   motionForwardOdom->setText (0, odom.str ().c_str ());

   odom.str("");
   odom << "Left = " << motionOdometry.left;
   motionLeftOdom->setText (0, odom.str ().c_str ());

   odom.str("");
   odom << "Turn = " << motionOdometry.turn;
   motionTurnOdom->setText (0, odom.str ().c_str ());

   stringstream head;
   head.str("");
   head << "HeadYaw = " << headYaw;
   motionHeadYaw->setText (0, head.str ().c_str ());

   head.str("");
   head << "HeadYawK = " << headYawK;
   kinematicsHeadYaw->setText (0, head.str ().c_str ());

}

