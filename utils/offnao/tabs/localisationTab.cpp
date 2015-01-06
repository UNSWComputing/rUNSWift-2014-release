#include "localisationTab.hpp"

#include "utils/SPLDefs.hpp"
#include "utils/angles.hpp"
#include "types/ActionCommand.hpp"

#include <QEvent>
#include <QMouseEvent>
#include <QGridLayout>
#include <QRect>
#include <QRadioButton>
#include <QGroupBox>
#include <QLabel>
#include <QPixmap>

#include "perception/localisation/Localiser.hpp"
#include "perception/localisation/VisionUpdateBundle.hpp"

#include <iostream>
#include <vector>

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

using namespace std;

LocalisationTab::LocalisationTab() : image(":/images/spl_field.svg"), 
                                     team_red(false), localiser(NULL),
                                     mouseDown(false), mouseOverObject(NULL),
                                     rng(42) {
   initFieldObjects();
   initInterface();
   initLocalisation();

   redraw();
}

void LocalisationTab::initLocalisation() {
   if (localiser != NULL) {
      delete localiser;
   }
   localiser = new Localiser(1);

   trueRobotPos.x() = trueX->text().toFloat();
   trueRobotPos.y() = trueY->text().toFloat();
   trueRobotPos.theta() = DEG2RAD(trueTheta->text().toFloat());

   trueRobotPos.var.setZero();
}

void LocalisationTab::initFieldObjects() {
   // positions assume we are on the blue team

   // posts
   FieldObject p1;
   p1.type = FieldObject::POST;
   p1.subType.postType = PostInfo::pAwayLeft;
   p1.pos.x() = GOAL_POST_ABS_X;
   p1.pos.y() = GOAL_WIDTH/2.0;
   p1.pos.theta() = NAN;
   fieldObjects.push_back(p1);

   FieldObject p2;
   p2.type = FieldObject::POST;
   p2.subType.postType = PostInfo::pAwayRight;
   p2.pos.x() = GOAL_POST_ABS_X;
   p2.pos.y() = -GOAL_WIDTH/2.0;
   p2.pos.theta() = NAN;
   fieldObjects.push_back(p2);

   FieldObject p3;
   p3.type = FieldObject::POST;
   p3.subType.postType = PostInfo::pHomeRight;
   p3.pos.x() = -GOAL_POST_ABS_X;
   p3.pos.y() = GOAL_WIDTH/2.0;
   p3.pos.theta() = NAN;
   fieldObjects.push_back(p3);

   FieldObject p4;
   p4.type = FieldObject::POST;
   p4.subType.postType = PostInfo::pHomeLeft;
   p4.pos.x() = -GOAL_POST_ABS_X;
   p4.pos.y() = -GOAL_WIDTH/2.0;
   p4.pos.theta() = NAN;
   fieldObjects.push_back(p4);

   FieldObject p5;
   p5.type = FieldObject::TWOPOSTS;
   p5.subType.postType = PostInfo::pAway;
   p5.pos.x() = FIELD_LENGTH/2.0;
   p5.pos.y() = 0;
   p5.pos.theta() = NAN;
   fieldObjects.push_back(p5);

   FieldObject p6;
   p6.type = FieldObject::TWOPOSTS;
   p6.subType.postType = PostInfo::pHome;
   p6.pos.x() = -FIELD_LENGTH/2.0;
   p6.pos.y() = 0;
   p6.pos.theta() = NAN;
   fieldObjects.push_back(p6);

   //corners

   // Front-left corner
   FieldObject flc;
   flc.type = FieldObject::FEATURE;
   flc.subType.featureType = FieldFeatureInfo::fCorner;
   flc.pos.x() = FIELD_LENGTH/2;
   flc.pos.y() = FIELD_WIDTH/2;
   flc.pos.theta() = -3*M_PI_4;
   fieldObjects.push_back(flc);

   // Front-right corner
   FieldObject frc;
   frc.type = FieldObject::FEATURE;
   frc.subType.featureType = FieldFeatureInfo::fCorner;
   frc.pos.x() = FIELD_LENGTH/2;
   frc.pos.y() = -FIELD_WIDTH/2;
   frc.pos.theta() = 3*M_PI_4;
   fieldObjects.push_back(frc);

   // Rear-left corner
   FieldObject rlc;
   rlc.type = FieldObject::FEATURE;
   rlc.subType.featureType = FieldFeatureInfo::fCorner;
   rlc.pos.x() = -FIELD_LENGTH/2;
   rlc.pos.y() = FIELD_WIDTH/2;
   rlc.pos.theta() = -M_PI_4;
   fieldObjects.push_back(rlc);

   // Rear-right corner
   FieldObject rrc;
   rrc.type = FieldObject::FEATURE;
   rrc.subType.featureType = FieldFeatureInfo::fCorner;
   rrc.pos.x() = -FIELD_LENGTH/2;
   rrc.pos.y() = -FIELD_WIDTH/2;
   rrc.pos.theta() = M_PI_4;
   fieldObjects.push_back(rrc);

   // Front bottom-left goal box
   FieldObject fgbl;
   fgbl.type = FieldObject::FEATURE;
   fgbl.subType.featureType = FieldFeatureInfo::fCorner;
   fgbl.pos.x() = FIELD_LENGTH/2-GOAL_BOX_LENGTH;
   fgbl.pos.y() = GOAL_BOX_WIDTH/2;
   fgbl.pos.theta() = -M_PI_4;
   fieldObjects.push_back(fgbl);

   // Front bottom-right goal box
   FieldObject fgbr;
   fgbr.type = FieldObject::FEATURE;
   fgbr.subType.featureType = FieldFeatureInfo::fCorner;
   fgbr.pos.x() = FIELD_LENGTH/2-GOAL_BOX_LENGTH;
   fgbr.pos.y() = -GOAL_BOX_WIDTH/2;
   fgbr.pos.theta() = M_PI_4;
   fieldObjects.push_back(fgbr);

   // Rear bottom-left goal box
   FieldObject rgbl;
   rgbl.type = FieldObject::FEATURE;
   rgbl.subType.featureType = FieldFeatureInfo::fCorner;
   rgbl.pos.x() = -FIELD_LENGTH/2+GOAL_BOX_LENGTH;
   rgbl.pos.y() = GOAL_BOX_WIDTH/2;
   rgbl.pos.theta() = -3*M_PI_4;
   fieldObjects.push_back(rgbl);

   // Rear bottom-right goal box
   FieldObject rgbr;
   rgbr.type = FieldObject::FEATURE;
   rgbr.subType.featureType = FieldFeatureInfo::fCorner;
   rgbr.pos.x() = -FIELD_LENGTH/2+GOAL_BOX_LENGTH;
   rgbr.pos.y() = -GOAL_BOX_WIDTH/2;
   rgbr.pos.theta() = 3*M_PI_4;
   fieldObjects.push_back(rgbr);

   // t-junctions

   // Left half-way line T
   FieldObject lht;
   lht.type = FieldObject::FEATURE;
   lht.subType.featureType = FieldFeatureInfo::fTJunction;
   lht.pos.x() = 0;
   lht.pos.y() = -FIELD_WIDTH/2;
   lht.pos.theta() = M_PI_2;
   fieldObjects.push_back(lht);

   // Right half-way line T
   FieldObject rht;
   rht.type = FieldObject::FEATURE;
   rht.subType.featureType = FieldFeatureInfo::fTJunction;
   rht.pos.x() = 0;
   rht.pos.y() = FIELD_WIDTH/2;
   rht.pos.theta() = -M_PI_2;
   fieldObjects.push_back(rht);

   // Front-left goal T
   FieldObject flgt;
   flgt.type = FieldObject::FEATURE;
   flgt.subType.featureType = FieldFeatureInfo::fTJunction;
   flgt.pos.x() = FIELD_LENGTH/2;
   flgt.pos.y() = GOAL_BOX_WIDTH/2;
   flgt.pos.theta() = -M_PI;
   fieldObjects.push_back(flgt);

   // Front-right goal T
   FieldObject frgt;
   frgt.type = FieldObject::FEATURE;
   frgt.subType.featureType = FieldFeatureInfo::fTJunction;
   frgt.pos.x() = FIELD_LENGTH/2;
   frgt.pos.y() = -GOAL_BOX_WIDTH/2;
   frgt.pos.theta() = -M_PI;
   fieldObjects.push_back(frgt);

   // Rear-left goal T
   FieldObject rlgt;
   rlgt.type = FieldObject::FEATURE;
   rlgt.subType.featureType = FieldFeatureInfo::fTJunction;
   rlgt.pos.x() = -FIELD_LENGTH/2;
   rlgt.pos.y() = GOAL_BOX_WIDTH/2;
   rlgt.pos.theta() = 0;
   fieldObjects.push_back(rlgt);

   // Rear-right goal T
   FieldObject rrgt;
   rrgt.type = FieldObject::FEATURE;
   rrgt.subType.featureType = FieldFeatureInfo::fTJunction;
   rrgt.pos.x() = -FIELD_LENGTH/2;
   rrgt.pos.y() = -GOAL_BOX_WIDTH/2;
   rrgt.pos.theta() = 0;
   fieldObjects.push_back(rrgt);

   // TODO penalty spots
   // TODO center circle
   FieldObject centreCircle;
   centreCircle.type = FieldObject::FEATURE;
   centreCircle.subType.featureType = FieldFeatureInfo::fCentreCircle;
   centreCircle.pos.x() = 0;
   centreCircle.pos.y() = 0;
   centreCircle.pos.theta() = 0;
   fieldObjects.push_back(centreCircle);

}

void LocalisationTab::initInterface() {
   fieldLabel = new QLabel();
   imagePixmap = QPixmap(640, 480);
   QRectF target(0.0, 0.0, 640.0, 480.0);
   QRectF source(0.0, 0.0, 740, 540.0);
   QPainter painter(&imagePixmap);
   painter.drawImage(target, image, source);

   QGridLayout *layout = new QGridLayout();
   
   QGroupBox *variance = new QGroupBox(tr("Observation Standard Deviation"));
   QGroupBox *bias = new QGroupBox(tr("Observation Bias"));
   QGroupBox *modes = new QGroupBox(tr("Modes"));
   QGroupBox *setup = new QGroupBox(tr("Setup"));

   this->setLayout(layout);
   layout->setAlignment(Qt::AlignTop);
   layout->addWidget(fieldLabel, 1, 1, 4, 1);
   layout->addWidget(variance, 1, 2);
   layout->addWidget(bias, 2, 2);
   layout->addWidget(modes, 3, 2);
   layout->addWidget(setup, 4, 2);

   QGridLayout *setupGroupLayout = new QGridLayout();
   setup->setLayout(setupGroupLayout);

   setupGroupLayout->addWidget(new QLabel("X: "), 1, 1);
   trueX = new QLineEdit("0.0");
   setupGroupLayout->addWidget(trueX, 1, 2);
   setupGroupLayout->addWidget(new QLabel("Y: "), 1, 3);
   trueY = new QLineEdit("0.0");
   setupGroupLayout->addWidget(trueY, 1, 4);
   setupGroupLayout->addWidget(new QLabel("Theta: "), 1, 5);
   trueTheta = new QLineEdit("0.0");
   setupGroupLayout->addWidget(trueTheta, 1, 6);

   QGridLayout *setupLayout2 = new QGridLayout();
   setupGroupLayout->addLayout(setupLayout2, 2, 1, 1, 6);

   QLabel *teamLabel = new QLabel("Team:");
   setupLayout2->addWidget(teamLabel, 1, 1);
   teamButtonGroup = new QButtonGroup();
   QRadioButton *blueButton = new QRadioButton(QString("blue"), teamLabel);
   blueButton->setChecked(true);
   QRadioButton *redButton = new QRadioButton(QString("red"), teamLabel);
   teamButtonGroup->addButton(blueButton, 0);
   teamButtonGroup->addButton(redButton, 1);
   setupLayout2->addWidget(blueButton, 1, 2);
   setupLayout2->addWidget(redButton, 1, 3);
   initButton = new QPushButton("Initialise filter");
   setupLayout2->addWidget(initButton, 1, 4);

   QLabel *distanceVarianceLabel = new QLabel("Distance");
   QLabel *headingVarianceLabel = new QLabel("Heading");
   QLabel *orientationVarianceLabel = new QLabel("Orientation");

   QGridLayout *varianceGroupLayout = new QGridLayout();
   variance->setLayout(varianceGroupLayout);

   varianceDistance = new QLineEdit("800");
   varianceHeading = new QLineEdit("10");
   varianceOrientation = new QLineEdit("10");
   varianceGroupLayout->addWidget(distanceVarianceLabel, 1, 1);
   varianceGroupLayout->addWidget(headingVarianceLabel, 2, 1);
   varianceGroupLayout->addWidget(orientationVarianceLabel, 3, 1);
   varianceGroupLayout->addWidget(varianceDistance, 1, 2);
   varianceGroupLayout->addWidget(varianceHeading, 2, 2);
   varianceGroupLayout->addWidget(varianceOrientation, 3, 2);

   QLabel *distanceBiasLabel = new QLabel("Distance");
   QLabel *headingBiasLabel = new QLabel("Heading");
   QLabel *orientationBiasLabel = new QLabel("Orientation");


   QGridLayout *biasGroupLayout = new QGridLayout();
   bias->setLayout(biasGroupLayout);
   biasDistance = new QLineEdit("0.0");
   biasHeading = new QLineEdit("0.0");
   biasOrientation = new QLineEdit("0.0");
   biasGroupLayout->addWidget(distanceBiasLabel, 1, 1);
   biasGroupLayout->addWidget(headingBiasLabel, 2, 1);
   biasGroupLayout->addWidget(orientationBiasLabel, 3, 1);
   biasGroupLayout->addWidget(biasDistance, 1, 2);
   biasGroupLayout->addWidget(biasHeading, 2, 2);
   biasGroupLayout->addWidget(biasOrientation, 3, 2);

   QGridLayout *modesGroupLayout = new QGridLayout();
   modes->setLayout(modesGroupLayout);
   modesGroupLayout->addWidget(new QLabel("x, y, theta for each mode goes here"));

   fieldLabel->setMouseTracking(true);
   fieldLabel->installEventFilter(this);

   // set up signals and slots
   connect(initButton, SIGNAL(released()), this, SLOT(initLocalisation()));
   connect(teamButtonGroup, SIGNAL(buttonReleased(int)), this, SLOT(setTeam(int)));

}

void LocalisationTab::setTeam(int team) {
    // TODO: This does nothing, remove all calls to this function.
}

void LocalisationTab::newNaoData(NaoData *naoData) {
   // we don't care about nao data
}

void LocalisationTab::setTransform(int device_width, int device_height) {
   transform = QTransform();
   // determine the size of the bitmap
   // int device_width = width;
   // int device_height = height;
   // translate to center of field
   transform.translate(device_width/2, device_height/2);
   // make positive y point upwards
   transform.scale(1,-1);
   if (team_red) {
      // x and y flipped for red team
      transform.scale(-1, -1);
   }
   // scale so we can draw using field coordinates
   transform.scale((float)device_width/OFFNAO_FULL_FIELD_LENGTH,
         (float)device_height/OFFNAO_FULL_FIELD_WIDTH);
}

void LocalisationTab::redraw() {
   // draw stuff
   QPixmap fieldPixmap(imagePixmap);

   FieldPainter fieldPainter(&fieldPixmap);
   setTransform(fieldPixmap.width(), fieldPixmap.height());

   fieldPainter.drawRobotAbs(trueRobotPos, "red", false); // ground-truth is red

   AbsCoord robotPos = localiser->getRobotPose();
   //cout << "num modes: " << robotPos.size() << endl;
   fieldPainter.drawRobotAbs(robotPos, "#ffee00"); // pacman yellow

   if (mouseOverObject != NULL) {
      AbsCoord objAbs = mouseOverObject->pos;
      if (team_red) {
         objAbs.x() *= -1;
         objAbs.y() *= -1;
         objAbs.theta() = NORMALISE(objAbs.theta() + M_PI);
      }

      if (mouseOverObject->type == FieldObject::POST) {
         QColor colour;
         if (mouseOverObject->subType.postType & PostInfo::pAway) {
            if (mouseDown) {
               colour = QColor("#FFFF66");
            } else {
               colour = QColor("#808000");
            }
         } else {
            if (mouseDown) {
               colour = QColor("#33FFFF");
            } else {
               colour = QColor("blue");
            }
         }
         fieldPainter.drawPostAbs(objAbs, colour);
      } else if (mouseOverObject->type == FieldObject::FEATURE) {
         fieldPainter.drawFeatureAbs (objAbs, mouseOverObject->subType.featureType);
      } else if (mouseOverObject->type == FieldObject::TWOPOSTS) {
         QColor colour;
         AbsCoord p1 = objAbs;
         p1.y() += GOAL_WIDTH/2.0;
         AbsCoord p2 = objAbs;
         p2.y() -= GOAL_WIDTH/2.0;
         if (mouseOverObject->subType.postType & PostInfo::pAway) {
            if (mouseDown) {
               colour = QColor("#FFFF66");
            } else {
               colour = QColor("#808000");
            }
         } else {
            if (mouseDown) {
               colour = QColor("#33FFFF");
            } else {
               colour = QColor("blue");
            }
         }
         fieldPainter.drawPostAbs(p1, colour);
         fieldPainter.drawPostAbs(p2, colour);
      }
   }

   fieldLabel->setPixmap(fieldPixmap);
}

bool LocalisationTab::eventFilter(QObject *object, QEvent *event) {
   if (object == fieldLabel) {
      QMouseEvent *mevent = static_cast<QMouseEvent *>(event);
      if (event->type() == QEvent::Enter) {
         return onMouseEnter(mevent);
      } else if (event->type() == QEvent::Leave) {
         return onMouseLeave(mevent);
      } else if (event->type() == QEvent::MouseMove) {
         return onMouseMove(mevent);
      } else if (event->type() == QEvent::MouseButtonPress) {
         return onMouseButtonPress(mevent);
      } else if (event->type() == QEvent::MouseButtonRelease) {
         return onMouseButtonRelease(mevent);
      } else {
         return false;
      }
   } else {
      return false;
   }
}

bool LocalisationTab::onMouseEnter(QMouseEvent *mevent) {
   mouseOverObject = NULL;
   mouseDown = false;
   return true;
}

bool LocalisationTab::onMouseLeave(QMouseEvent *mevent) {
   mouseOverObject = NULL;
   mouseDown = false;
   return true;
}

bool LocalisationTab::onMouseMove(QMouseEvent *mevent) {
   QPoint p = transform.inverted().map(mevent->pos());
   //cout << "MouseMouseMove at " << p.x() << "," << p.y() << endl;

   mouseOverObject = NULL;
   vector<FieldObject>::iterator i;
   for (i = fieldObjects.begin(); i != fieldObjects.end(); ++i) {
      AbsCoord objAbs = i->pos;
      if (team_red) {
         objAbs.x() *= -1;
         objAbs.y() *= -1;
         objAbs.theta() = NORMALISE(objAbs.theta() + M_PI);
      }
      if (  (ABS(objAbs.x() - p.x()) < 200)
         && (ABS(objAbs.y() - p.y()) < 200)) {
         mouseOverObject = &(*i);
         break;
      }
   }
   return true;
}
bool LocalisationTab::onMouseButtonPress(QMouseEvent *mevent) {
   if (mevent->button() == Qt::LeftButton) {
      QPoint p = transform.inverted().map(mevent->pos());
      //cout << "MouseButtonPress at " << p.x() << "," << p.y() << endl;
      mouseDown = true;
      return true;
   } else {
      return false;
   }
}

bool LocalisationTab::onMouseButtonRelease(QMouseEvent *mevent) {
   if (mevent->button() == Qt::LeftButton) {
      QPoint p = transform.inverted().map(mevent->pos());
      //cout << "MouseButtonRelease at " << p.x() << "," << p.y() << endl;
      mouseDown = false;
      if (mouseOverObject != NULL) {
         cout << "You clicked on object at: " << mouseOverObject->pos.x() << " " << mouseOverObject->pos.y() << endl;
         createObservation(mouseOverObject);
         // create observation for mouseOverObject
      }
      return true;
   } else {
      return false;
   }
}

void LocalisationTab::paintEvent(QPaintEvent*) {
   redraw();
}

float LocalisationTab::zeroMeanGaussianSample(float standardDeviation) {
   boost::normal_distribution<> nd(0.0, standardDeviation);
   boost::variate_generator<boost::mt19937&, 
      boost::normal_distribution<> > var_nor(rng, nd);
   float d = var_nor();
   cout << "Gaussian sample: " << d << endl;
   return d;
}

RRCoord LocalisationTab::absToRR(AbsCoord obsAbs) {
   RRCoord obsRR;
   obsRR.distance() = sqrt(SQUARE(obsAbs.x()-trueRobotPos.x()) + SQUARE(obsAbs.y()-trueRobotPos.y()));
   obsRR.heading() = NORMALISE(atan2(obsAbs.y()-trueRobotPos.y(),
                                     obsAbs.x()-trueRobotPos.x()) - trueRobotPos.theta());
   obsRR.orientation() = NORMALISE(obsRR.heading() + trueRobotPos.theta() - obsAbs.theta() + M_PI);
   //obsRR.orientation() = NORMALISE(obsAbs.theta() - trueRobotPos.theta());
   obsRR.distance() += biasDistance->text().toFloat();
   obsRR.distance() += zeroMeanGaussianSample(varianceDistance->text().toFloat()/4.0);
   obsRR.heading() += DEG2RAD(biasHeading->text().toFloat());
   obsRR.heading() += zeroMeanGaussianSample(DEG2RAD(varianceHeading->text().toFloat())/4.0);
   obsRR.orientation() += DEG2RAD(biasOrientation->text().toFloat());
   obsRR.orientation() += zeroMeanGaussianSample(DEG2RAD(varianceOrientation->text().toFloat()))/4.0;

   obsRR.var.setZero();
   obsRR.var(0,0) = SQUARE(varianceDistance->text().toFloat());
   obsRR.var(1,1) = SQUARE(DEG2RAD(varianceHeading->text().toFloat()));
   obsRR.var(2,2) = SQUARE(DEG2RAD(varianceOrientation->text().toFloat()));
   cout << "Generating observation: (" << obsRR.distance() << "," << RAD2DEG(obsRR.heading()) << "," << RAD2DEG(obsRR.orientation()) << ")" << endl;
   return obsRR;
}

void LocalisationTab::createObservation(FieldObject *obj) {

   vector<FieldEdgeInfo> fieldEdges;
   vector<FieldFeatureInfo> fieldFeatures;
   vector<PostInfo> posts;
   vector<RobotInfo> robots;
   vector<AbsCoord> teammatePoses;
   AbsCoord ball;
   TeamBallInfo teamBall;

   Odometry odometry;

   AbsCoord obsAbs = obj->pos;

   if (team_red) {
      obsAbs.x() *= -1;
      obsAbs.y() *= -1;
      obsAbs.theta() = NORMALISE(obsAbs.theta() + M_PI);
   }

   if (obj->type == FieldObject::POST) {
      RRCoord obsRR = absToRR(obsAbs);
      PostInfo p;
      p.rr = obsRR;
      p.type = obj->subType.postType;
      posts.push_back(p);
   } else if (obj->type == FieldObject::TWOPOSTS) {
      AbsCoord a1 = obsAbs;
      a1.y() += GOAL_WIDTH/2.0;
      AbsCoord a2 = obsAbs;
      a2.y() -= GOAL_WIDTH/2.0;
      if (obj->subType.postType == PostInfo::pHome) {
         a1.y() *= -1;
         a2.y() *= -1;
      }
      if (team_red) {
         a1.y() *= -1;
         a2.y() *= -1;
      }

      RRCoord rr1 = absToRR(a1);
      RRCoord rr2 = absToRR(a2);
      PostInfo p1;
      PostInfo p2;
      p1.rr = rr1;
      p2.rr = rr2;
      cout << "======================" << endl;
      cout << "left post abs coords: " << a1.x() << " " << a1.y() << endl;
      cout << "left post rr coords: " << p1.rr.distance() << " " << RAD2DEG(p1.rr.heading()) << endl;
      cout << "right post abs coords: " << a2.x() << " " << a2.y() << endl;
      cout << "right post rr coords: " << p2.rr.distance() << " " << RAD2DEG(p2.rr.heading()) << endl;
      p1.type = (PostInfo::Type) (obj->subType.postType | PostInfo::pLeft);
      p2.type = (PostInfo::Type) (obj->subType.postType | PostInfo::pRight);
      posts.push_back(p1);
      posts.push_back(p2);
   } else {
      RRCoord obsRR = absToRR(obsAbs);
      FieldFeatureInfo f;
      f.rr = obsRR;
      f.type = obj->subType.featureType;
      fieldFeatures.push_back(f);
   }

   /* Changing to localiser */
   ActionCommand::All action;

   VisionUpdateBundle visionUpdateBundle(
      fieldEdges,
      fieldFeatures,
      posts,
      robots,
      teammatePoses,
      false,
      0.5f,
      0.0f,
      std::vector<BallInfo>(),
      true, true);

   localiser->localise(LocaliserBundle(odometry, visionUpdateBundle, action, 
      false, 3 /*state=playing*/, 1 /*state2=normal*/, 1.0f / 30.0f), true);
}
