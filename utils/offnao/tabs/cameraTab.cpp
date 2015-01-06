#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>

#include <iostream>
#include "blackboard/Blackboard.hpp"
#include "cameraTab.hpp"
#include "yuvHistogram.hpp"
#include "perception/vision/yuv.hpp"
#include "perception/vision/NaoCamera.hpp"

using namespace std;

extern __u32 controlIds[NUM_CONTROLS];

/*
static const char* CameraNames[] = {
    "Top",
    "Bottom"
};
*/

static const char* ControlNames[NUM_CONTROLS] = {
   "Vertical Flip",
   "Horizontal Flip",
   "Auto Gain",
   "Gain",
   "Auto Exposure Correction",
   "Exposure",
   "Exposure Correction",
   "Auto White Balance",
   "Blue Balance",
   "Red Balance",
   "Brightness",
   "Contrast",
   "Saturation",
   "Hue"
};
/*
   "Horizontal Center",
   "Vertical Center",
   "Sharpness"
*/

static const int cNUM_CAM_CONTROLS = sizeof(ControlNames)/sizeof(char *);

struct controlAttributes {
    __u32 control;
    string name;
    __s32 min;
    __s32 max;
    __s32 current;
    bool tryIncrease;
};

static struct controlAttributes ControlValues[] = {
    {V4L2_CID_GAIN,                "Gain",                 0,  255, 0, false},
    {V4L2_CID_EXPOSURE,            "Exposure",             0, 4096, 0, false},
    {V4L2_CID_EXPOSURE_CORRECTION, "Exposure Correction", -6,    6, 0, false},
    {V4L2_CID_BLUE_BALANCE,        "Blue Balance",         0,  255, 0, false},
    {V4L2_CID_RED_BALANCE,         "Red Balance",          0,  255, 0, false},
    {V4L2_CID_BRIGHTNESS,          "Brightness",           0,  255, 0, false},
    {V4L2_CID_CONTRAST,            "Contrast",             0,  127, 0, false},
    {V4L2_CID_SATURATION,          "Saturation",           0,  256, 0, false},
    {V4L2_CID_HUE,                 "Hue",               -180,  180, 0, false},
    {V4L2_CID_SHARPNESS,           "Sharpness",            0,   31, 0, false}
};

static const int cNUM_CONTROL_ATTRIBUTES = sizeof(ControlValues)/sizeof(struct controlAttributes);

CameraTab::CameraTab(QTabWidget *parent,
      QMenuBar *menuBar, Vision *vision)  {
   initMenu(menuBar);
   init();
   this->vision = vision;
   currentFrame = NULL;
   topFrame = NULL;
   botFrame = NULL;
   this->parent = parent;
   framesSinceReset = INT_MAX;
}


void CameraTab::initMenu(QMenuBar *menuBar) {
   cameraMenu = new QMenu("Camera");
   menuBar->addMenu(cameraMenu);
}

void CameraTab::init() {
   int i;

   // Lay them all out
   layout = new QGridLayout();
   this->setLayout(layout);

   // Set up pixmaps for camera image and RGB, YUV, HSV histograms
   for (i = 0; i < NUM_IMG_TYPES; i++) {
      // Create pixmap and label
      imagePixmaps[i] = QPixmap(320, 240);
      imagePixmaps[i].fill(Qt::darkGray);
      imageLabels[i] = new QLabel();
      imageLabels[i]->setPixmap(imagePixmaps[i]);

      // Set alignment and size of pixmaps
      imageLabels[i]->setAlignment(Qt::AlignTop);
      imageLabels[i]->setMinimumSize(IMAGE_COLS/2, IMAGE_ROWS/2);
      imageLabels[i]->setMaximumSize(IMAGE_COLS/2, IMAGE_ROWS/2);
   }

   // Position image and histogram pixmaps
   layout->addWidget(imageLabels[CAMERA], 0,0,1,1); // Camera image top left
   //layout->addWidget(imageLabels[RGB], 1,0,1,1); // RGB histogram below camera
   layout->addWidget(&pointCloud,      1,0,1,1);
   layout->addWidget(imageLabels[YUV], 0,1,1,1); // YUV histogram to right
   layout->addWidget(imageLabels[HSV], 1,1,1,1); // HSV histogram below YUV

   // Add controls
   QVBoxLayout *controlLayout = new QVBoxLayout();
   QButtonGroup *cameraGroup = new QButtonGroup();

   // Camera selector
   QGroupBox *cameraBox = new QGroupBox(tr("Camera"));
   QVBoxLayout *cameraGroupLayout = new QVBoxLayout();
   cameraBox->setLayout(cameraGroupLayout);
   controlLayout->addWidget(cameraBox);
   cameraGroupLayout->setAlignment(Qt::AlignTop);
   // Add top and bottom camera selector radio buttons
   QRadioButton *radioButton = new QRadioButton(QString("Top"), cameraBox);
   cameraGroupLayout->addWidget(radioButton);
   cameraGroup->addButton(radioButton, 0);
   radioButton = new QRadioButton(QString("Bottom"), cameraBox);
   cameraGroupLayout->addWidget(radioButton);
   cameraGroup->addButton(radioButton, 0);

   // Camera controls
   QGroupBox *controlsBox = new QGroupBox(tr("Camera Controls"));
   QGridLayout *controlsGroupLayout = new QGridLayout();
   controlsBox->setLayout(controlsGroupLayout);
   controlLayout->addWidget(controlsBox);

   for (i = 0; i < cNUM_CAM_CONTROLS; i++) {
      //QHBoxLayout *settingsGroupLayout = new QHBoxLayout();
      QLabel *nameLabel = new QLabel(ControlNames[i]);
      //QLabel *minLabel = new QLabel("0");
      QPushButton *decButton = new QPushButton("-");
      decButton->setMaximumSize(30,30);
      QLineEdit *currentValue =
            new QLineEdit(QString::number(controlValues[TOP_CAMERA][i]));
      QPushButton *incButton = new QPushButton("+");
      incButton->setMaximumSize(30,30);
      //QLabel *maxLabel = new QLabel("255");

      connect(decButton, SIGNAL(clicked(bool)),
              new CameraButtonHandler(this, i, decButton, currentValue),
              SLOT(clicked(bool)));
      connect(incButton, SIGNAL(clicked(bool)),
              new CameraButtonHandler(this, i, incButton, currentValue),
              SLOT(clicked(bool)));
      connect(currentValue, SIGNAL(editingFinished()),
              new CameraButtonHandler(this, i, NULL, currentValue),
              SLOT(clicked()));

      controlsGroupLayout->addWidget(nameLabel, i * 3, 0, 3, 1);
      //controlsGroupLayout->addWidget(minLabel, i * 3, 1, 3, 1);
      controlsGroupLayout->addWidget(decButton, i * 3, 2, 3, 1);
      controlsGroupLayout->addWidget(currentValue, i * 3, 3, 3, 1);
      controlsGroupLayout->addWidget(incButton, i * 3, 4, 3, 1);
      //controlsGroupLayout->addWidget(maxLabel, i * 3, 5, 3, 1);
      //controlsGroupLayout->addWidget(settingsGroupLayout);
   }

   // Add auto tuning button
   QPushButton *autoTuneButton = new QPushButton("Auto Tune");
   controlsGroupLayout->addWidget(autoTuneButton, i * 3, 2, 3, 3);
   connect(autoTuneButton, SIGNAL(clicked(bool)), this,
              SLOT(autoTuner()));
   layout->addLayout(controlLayout, 0,2,2,1);

   // Save default camera values so we can reset them after auto tuning
   for (int i = 0; i < cNUM_CONTROL_ATTRIBUTES; i++) {
      // THIS ASSUMES WE'RE ONLY USING THE BOTTOM CAMERA - FIX! morri
      for (int j = 0; j < NUM_CONTROLS; j++) {
         if (ControlValues[i].control == controlIds[j]) {
            ControlValues[i].current = controlValues[TOP_CAMERA][j];
         }
      }
   }

   // Initialise entropy
   bestEntropy = 0.0;
}


void CameraTab::redraw() {
   // cout << "Calling redraw\n";
   QImage image = QImage(IMAGE_COLS, IMAGE_ROWS, QImage::Format_RGB32);
   pointCloud.points.resize(0);
   if(topFrame && botFrame) {
      //display normal image
      for (unsigned int row = 0; row < IMAGE_ROWS; ++row) {
         for (unsigned int col = 0; col < IMAGE_COLS; ++col) {
            QRgb rgb = getRGB(col, row, topFrame, TOP_IMAGE_COLS);
            image.setPixel(col, row, rgb);
            int r, g, b;
            QColor::fromRgb(rgb).getRgb(&r, &g, &b);
            pointCloud.points.push_back(make_pair(qglviewer::Vec(r / 255.0,
                                                                 g / 255.0,
                                                                 b / 255.0),
                                                  qglviewer::Vec
                                       (gety(topFrame, row, col) / 255.0,
                                        getu(topFrame, row, col) / 255.0,
                                        getv(topFrame, row, col) / 255.0)));
         }
      }
   } else {
      image.fill(Qt::darkGray);
   }

   QPixmap imagePixmap = QPixmap(QPixmap::fromImage(image.scaled(IMAGE_COLS/2, IMAGE_ROWS/2)));
   imageLabels[CAMERA]->setPixmap(imagePixmap);

   // Create histograms
   //Histogram RGBHistogram(0x00000000U, 0x00ffffffU);
   YUVHistogram yuvHistogram(0x00000000U, 0x00ffffffU);
   //Histogram redHistogram(0x00U, 0xffU);
   //Histogram greenHistogram(0x00U, 0xffU);
   //Histogram blueHistogram(0x00U, 0xffU);

   // Process image
   for (unsigned int row = 0; row < IMAGE_ROWS; ++row) {
      for (unsigned int col = 0; col < IMAGE_COLS; ++col) {
         unsigned int YUVValue = 0x00ffffff &
                                 ((gety(topFrame, row, col) << 16) |
                                  (getu(topFrame, row, col) << 8) |
                                   getv(topFrame, row, col));

         //RGBHistogram.addDatapoint(getRGB(col, row, currentFrame) & MAX_RGB_VALUE);
         yuvHistogram.addDatapoint(YUVValue);
         //redHistogram.addDatapoint((getRGB(col, row, currentFrame) & RED_MASK) >> 16);
         //greenHistogram.addDatapoint((getRGB(col, row, currentFrame) & GREEN_MASK) >> 8);
         //blueHistogram.addDatapoint(getRGB(col, row, currentFrame) & BLUE_MASK);
      }
   }

/*******************************************************************************
REMOVED FOR EFFICIENCY
   // Print statistics to terminal
   ostringstream ost;
   ost << "RGB Histogram ";
   //RGBHistogram.printStatistics(ost);
   ost << "YUV Histogram ";
   //yuvHistogram.printStatistics(ost);
   ost << "Red Histogram ";
   redHistogram.printStatistics(ost);
   ost << "Green Histogram ";
   greenHistogram.printStatistics(ost);
   ost << "Blue Histogram ";
   blueHistogram.printStatistics(ost);
   // cout << ost.str();
*******************************************************************************/

   // Draw histograms
   //  set initial background of pixmaps
   imagePixmaps[RGB].fill(Qt::darkGray);
   imagePixmaps[YUV].fill(Qt::darkGray);
   imagePixmaps[HSV].fill(Qt::darkGray);
   //  draw each histogram
   //RGBHistogram.drawHistogram(imagePixmaps[RGB], (unsigned int)(IMAGE_COLS/2), (unsigned int)(IMAGE_ROWS/2), Histogram::eRGB);
   yuvHistogram.drawHistogram(imagePixmaps[YUV], (unsigned int)(IMAGE_COLS/2), (unsigned int)(IMAGE_ROWS/2), YUVHistogram::eYUV);
   //redHistogram.drawHistogram(imagePixmaps[HSV], (unsigned int)(IMAGE_COLS/2), (unsigned int)(IMAGE_ROWS/2), Histogram::eRED);
   //greenHistogram.drawHistogram(imagePixmaps[HSV], (unsigned int)(IMAGE_COLS/2), (unsigned int)(IMAGE_ROWS/2), Histogram::eGREEN);
   //blueHistogram.drawHistogram(imagePixmaps[HSV], (unsigned int)(IMAGE_COLS/2), (unsigned int)(IMAGE_ROWS/2), Histogram::eBLUE);
   //  display them in camera tab
   imageLabels[YUV]->setPixmap(imagePixmaps[YUV]);
   //imageLabels[RGB]->setPixmap(imagePixmaps[RGB]);
   //imageLabels[HSV]->setPixmap(imagePixmaps[HSV]); // USE HSV PIXMAP FOR TESTING - CHANGE LATER


   // Display entropy in message bar
   
/*******************************************************************************************************
   ostringstream est;
   est << "Your entropy is ";
   est << redHistogram.getEntropyValue();
   est << " + ";
   est << greenHistogram.getEntropyValue();
   est << " + ";
   est << blueHistogram.getEntropyValue();
   est << " = ";
   est << redHistogram.getEntropyValue()
          + greenHistogram.getEntropyValue()
          + blueHistogram.getEntropyValue();
   est << " That's a mess!";
   //est << RGBHistogram.getEntropyValue();
   emit showMessage(QString(est.str().c_str()));
*******************************************************************************************************/

   if (autoTuneOn) {
      tuneValue(yuvHistogram.getEntropyValue());
/*******************************************************************************************************
      tuneValue(redHistogram.getEntropyValue() +
                greenHistogram.getEntropyValue() +
                blueHistogram.getEntropyValue());
*******************************************************************************************************/
   }
}

// TODO(brockw): see if this can be genericized into tab.cpp, so it's not in
// every tab
void CameraTab::newNaoData(NaoData *naoData) {
   if (!naoData || !naoData->getCurrentFrame().blackboard) {
      topFrame = NULL;
      botFrame = NULL;
     // currentFrame = NULL;
   } else {
      Blackboard *blackboard = naoData->getCurrentFrame().blackboard;
      if (((topFrame = readFrom(vision, topFrame)) != NULL) && 
          ((botFrame = readFrom(vision, botFrame)) != NULL)) 
         if (parent->currentIndex() == parent->indexOf(this))
            redraw();
   }
   framesSinceReset++;
}

void CameraTab::setControls(const vector<struct v4l2_control> &controls){
   stringstream ss;
   ss << "--vision.camera_controls=";
   for(vector<struct v4l2_control>::const_iterator ci = controls.begin();
       ci != controls.end(); ++ci)
      ss << (ci->id - V4L2_CID_BASE) << ":" << ci->value << ",";
   emit sendCommandToRobot(QString(ss.str().c_str()));
}

void CameraTab::autoTuner(void) {
   vector<struct v4l2_control> vals;
   // Set camera back to default attribute values
   for (int i = 0; i < NUM_CONTROLS; i++) {
      // THIS ASSUMES WE'RE ONLY USING THE BOTTOM CAMERA - FIX! morri
      struct v4l2_control val = {controlIds[i], controlValues[TOP_CAMERA][i]};
      vals.push_back(val);
   }
   setControls(vals);


   if (autoTuneOn) { // Then turn it off
      qDebug() << "Autotuning off!";
   } else { // Otherwise turn it on
      qDebug() << "Autotuning on!";
      bestEntropy = 0.0; // Reset best entropy value - DO WE WANT THIS? morri
   }
   autoTuneOn = !autoTuneOn;
   srandom(time(NULL)); // (Re)Seed random number generator
   framesSinceReset = 0;
}

void CameraTab::tuneValue(float entropy) {
   vector<struct v4l2_control> vals;
   static unsigned int iterations = 0;
   static int control = -1;
   static unsigned int temperature = 12;

   // wait for new frame with updated image
   if (framesSinceReset > 3) {
      // Get new entropy value
      if (entropy < bestEntropy) {
         // If new entropy value is worse, revert back to previous attribute value for control
         struct v4l2_control val = {ControlValues[prevControl].control, prevControlValue};
         vals.push_back(val);
         // Next time we try this control, try the other direction
         ControlValues[prevControl].tryIncrease = !ControlValues[prevControl].tryIncrease;
         ControlValues[prevControl].current = prevControlValue;
         qDebug() << "Decreased entropy: " << entropy;
      } else {
         bestEntropy = entropy;
         qDebug() << "Increased entropy: " << entropy;
         printCameraControlValues();
      }

/******************************************************************************************************
      // Select random attribute
      int control = random() % cNUM_CONTROL_ATTRIBUTES;
******************************************************************************************************/
      // Select next control to manipulate
      control = (control + 1) % cNUM_CONTROL_ATTRIBUTES;
      // Save current values in case they need to be reset
      prevControl = control;
      prevControlValue = ControlValues[control].current;
      // Select random value for this attribute
//      int newValue = ControlValues[control].min
//                     + random() % (ControlValues[control].max - ControlValues[control].min);
      int degreeOfChange = (ControlValues[control].max - ControlValues[control].min)/temperature;
      int newValue = prevControlValue + (ControlValues[control].tryIncrease ? degreeOfChange : -degreeOfChange);
      int clippedNewValue = min(newValue, ControlValues[control].max);
      clippedNewValue = max(clippedNewValue, ControlValues[control].min);
      if (clippedNewValue == prevControlValue)
        ControlValues[control].tryIncrease = !ControlValues[control].tryIncrease;
      struct v4l2_control val = {ControlValues[control].control, clippedNewValue};
      ControlValues[control].current = clippedNewValue;
      vals.push_back(val);
      setControls(vals);

      framesSinceReset = 0;
      iterations++;
   } else {
      framesSinceReset++;
   }
   if ((iterations % (2 * cNUM_CONTROL_ATTRIBUTES)) == 0) {
      temperature *= 2;
   }
}

void CameraTab::printCameraControlValues(void) {
   qDebug() << "Camera control settings";

   for (int i = 0; i < cNUM_CONTROL_ATTRIBUTES; i++) {
      qDebug() << "\t" << ControlValues[i].name.c_str() << "=" << ControlValues[i].current;
   }
}

CameraButtonHandler::CameraButtonHandler(CameraTab *camtab, int control,
                                          QPushButton *qpb, QLineEdit *qle) :
      camtab(camtab), control(control), qpb(qpb), qle(qle) {
}

void CameraButtonHandler::clicked(bool) {
   __s32 diff = qpb ? (qpb->text() == "-" ? -1 : 1) : 0;
   __s32 newval = qle->text().toInt() + diff;
   qle->setText(QString::number(newval));
   struct v4l2_control val = {controlIds[control], newval};
   vector<struct v4l2_control> vals(1, val);
   camtab->setControls(vals);
}

