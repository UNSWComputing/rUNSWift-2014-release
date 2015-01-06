
#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QMenuBar>
#include <QInputDialog>
#include <QScrollArea>
#include <vector>
#include <iostream>
#include <utility>
#include <cmath>
#include <sstream>
#include <string>

#include "utils/CPlaneColours.hpp"

#include "../../robot/perception/vision/yuv.hpp"
#include "calibrationTab.hpp"
#include <boost/shared_ptr.hpp>
#include "blackboard/Blackboard.hpp"
#include "perception/vision/Vision.hpp"



using namespace std;

static const char* OverlayNames[] = {
   "none",
   "selected",
   "selected+unclassified",
   "all colours",
   "all + unclassified"
};

/* A handy helper function */
void inRange(int &n, int min, int max) {
   if (n < min) {
      n = min;
   }
   if (n > max) {
      n = max;
   }
}

CalibrationTab::CalibrationTab(
      QTabWidget *parent,
      QMenuBar *menuBar,
      Vision *vision)
   :
      zoomLevel(1), zoomLog(0),
      qdwPointCloud(tr("Point Cloud")),
      mouseX(0), mouseY(0),
      prevMouseX(0), prevMouseY(0),
      prevZoomLevel(1), naoData(0)
{
   initMenu(menuBar);
   init();
   this->vision = vision;
   this->topClassifier = new Classifier();
   this->botClassifier = new Classifier();

   //TODO: get from options?
   topNNMCPath = std::string(getenv("RUNSWIFT_CHECKOUT_DIR")) +
           "/image/home/nao/data/top.nnmc.bz2";
   botNNMCPath = std::string(getenv("RUNSWIFT_CHECKOUT_DIR")) +
           "/image/home/nao/data/bot.nnmc.bz2";

   topKernelPath = std::string(getenv("RUNSWIFT_CHECKOUT_DIR")) +
           "/utils/kernels/top_kernel";

   botKernelPath = std::string(getenv("RUNSWIFT_CHECKOUT_DIR")) +
           "/utils/kernels/bot_kernel";

   this->topClassifier->loadClassificationFile(topKernelPath.c_str());
   this->botClassifier->loadClassificationFile(botKernelPath.c_str());

   this->parent = parent;
}

CalibrationTab::~CalibrationTab() {
    delete topClassifier;
    delete botClassifier;
}

void CalibrationTab::initMenu(QMenuBar *menuBar) {
   calibrationMenu = new QMenu("&Calibration");
   menuBar->addMenu(calibrationMenu);

   saveBothKernelsAct = new QAction(tr("&Save Both Kernels"), calibrationMenu);
   saveAsNNMCAct = new QAction(tr("Save &NNMC"), calibrationMenu);
   undoAct  = new QAction(tr("&Undo Act"), calibrationMenu);



   calibrationMenu->addAction(saveBothKernelsAct);
   calibrationMenu->addSeparator();
   calibrationMenu->addAction(saveAsNNMCAct);
   calibrationMenu->addAction(tr("&Push NNMC To Nao"), this, SLOT(pushNnmc()),
         QKeySequence(tr("Ctrl+P", "File|Print")));

   // connect the actions
   connect(saveBothKernelsAct, SIGNAL(triggered()), this, SLOT(saveBothKernels()));
   connect(saveAsNNMCAct, SIGNAL(triggered()), this, SLOT(saveNnmc()));
   connect(undoAct, SIGNAL(triggered()), this, SLOT(undoAction()));
}

void CalibrationTab::createKernelGroup() {
    kernelBox = new QGroupBox(tr("Kernel Options"));
    kernelLayout = new QGridLayout();

    kernelBox->setLayout(kernelLayout);

    QPushButton *newKernelButton = new QPushButton("New Kernel");

    topCamera = new QRadioButton("Top Camera");
    botCamera = new QRadioButton("Bot Camera");

    topCamera->setChecked(true);

    QPushButton *saveButton= new QPushButton("Save");
    QPushButton *saveAsButton = new QPushButton("Save as...");
    QPushButton *loadButton = new QPushButton("Load");
    loadButton->setFlat(false);

    kernelLayout->addWidget(topCamera, 0, 0, 1, 1);
    kernelLayout->addWidget(botCamera, 0, 1, 1, 1);
    kernelLayout->addWidget(newKernelButton, 1, 0, 1, 1);
    kernelLayout->addWidget(loadButton, 1, 1, 1, 1);
    kernelLayout->addWidget(saveButton, 2, 0, 1, 1);
    kernelLayout->addWidget(saveAsButton, 2, 1, 1, 1);


    connect(newKernelButton, SIGNAL(clicked(bool)), this, SLOT(newKernel()));
    connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(saveKernel()));
    connect(saveAsButton, SIGNAL(clicked(bool)), this, SLOT(saveAsKernel()));
    connect(loadButton, SIGNAL(clicked(bool)), this, SLOT(loadKernel()));



    checkboxLayout->addWidget(kernelBox);

    connect(topCamera, SIGNAL(toggled(bool)), this, SLOT(redrawSlot()));

}

void CalibrationTab::init() {
   // Camera Image
   imagePixmap = QPixmap(IMAGE_ROWS, IMAGE_COLS);
   imagePixmap.fill(Qt::darkGray);
   camLabel  = new QLabel();
   camLabel->setPixmap(imagePixmap);


   // Colour Picker & Overlays
   checkboxLayout = new QVBoxLayout();

   createKernelGroup();
   colourGroup = new QButtonGroup();

   colourBox = new QGroupBox(tr("Colour"));
   colourGroupLayout = new QVBoxLayout();
   colourBox->setLayout(colourGroupLayout);
   checkboxLayout->addWidget(colourBox);

   for (int i = 0; i < cNUM_COLOURS; ++i) {
      if (i == 1) {
         colours[i] = new QRadioButton(QString("goal and robot blue"), colourBox);
         colourGroupLayout->addWidget(colours[i]);
         colourGroup->addButton(colours[i], i);
      } else if (i == 3) {
      } else if (i == cBODY_PART) {
          //should not be able to assign body part colour
      } else {
         colours[i] = new QRadioButton(QString(ColourNames[i]), colourBox);
         colourGroupLayout->addWidget(colours[i]);
         colourGroup->addButton(colours[i], i);
      }
   }
   colours[0]->setChecked(true);

   overlayGroup = new QButtonGroup();
   overlayGroupLayout = new QVBoxLayout();
   overlayBox = new QGroupBox(tr("Overlays"));
   overlayBox->setLayout(overlayGroupLayout);
   checkboxLayout->addWidget(overlayBox);

   for (int i = 0; i < oNUM_OVERLAYS; ++i) {
      overlays[i] = new QRadioButton(QString(OverlayNames[i]), overlayBox);
      overlayGroupLayout->addWidget(overlays[i]);
      overlayGroup->addButton(overlays[i], i);
   }
   overlays[0]->setChecked(true);

   checkVisionOverlay = new QCheckBox("vision module", this);
   checkVisionOverlay->setChecked(false);
   overlayGroupLayout->addWidget(checkVisionOverlay);

   checkAutoWeight = new QCheckBox(
                  "Auto-Weight Guassians (good for fine-tuning)", this);
   checkAutoWeight->setChecked(false);
   overlayGroupLayout->addWidget(checkAutoWeight);

   undo = new QPushButton(QString("Undo"));
   checkboxLayout->addWidget(undo);

   checkboxLayout->addStretch(1);

   qrbControls[0] = new QRadioButton(tr("&Selected"));
   qrbControls[1] = new QRadioButton(tr("&All"));
   qrbControls[1]->setChecked(true);
   qhblControls.addWidget(qrbControls[0]);
   qhblControls.addWidget(qrbControls[1]);
   qhblControls.addStretch(1);
   qglPointCloud.addWidget(&pointCloud, 0, 0, 1, 1);
   qglPointCloud.addLayout(&qhblControls, 1, 0, 1, 1);
   qwPointCloud.setLayout(&qglPointCloud);
   qdwPointCloud.setAllowedAreas(Qt::BottomDockWidgetArea);
   qdwPointCloud.setWidget(&qwPointCloud);

   // Lay them all out
   layout = new QGridLayout();
   layout->setAlignment(layout, Qt::AlignTop);
   this->setLayout(layout);
   layout->addWidget(camLabel, 0, 0, 1, 1);

   //commented out because I don't know what it does and I want the space
   //layout->addWidget(&qdwPointCloud, 1, 0, 1, 1);

   layout->addLayout(checkboxLayout, 0, 1, 1, 1);
   camLabel->setAlignment(Qt::AlignTop);
   camLabel->setMinimumSize(IMAGE_COLS, IMAGE_ROWS);
   camLabel->setMaximumSize(IMAGE_COLS, IMAGE_ROWS);

   // Set up signals
   connect(colourGroup, SIGNAL(buttonReleased(int)), this,
                               SLOT(setColour(int)));
   connect(overlayGroup, SIGNAL(buttonReleased(int)), this,
                               SLOT(setOverlay(int)));
   connect(undo, SIGNAL(released()), this,
                               SLOT(undoAction()));
   connect(checkVisionOverlay, SIGNAL(stateChanged(int)), this,
                               SLOT(redrawSlot()));
   connect(qrbControls[0], SIGNAL(toggled(bool)), this, SLOT(redrawSlot()));

   // Even filters
   camLabel->installEventFilter(this);

   // Data
   colour = (Colour) 0;
   overlay = (Overlay) 0;

   dumpFile = NULL;
   currentFrame = NULL;

   setMouseTracking(true);
   camLabel->setMouseTracking(true);
   camLabel->setCursor(QCursor(Qt::BlankCursor));

   progressDialog = new QProgressDialog(this);
   connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelUpload()));
}


void CalibrationTab::newKernel() {
   getCurrentClassifier()->newClassificationFile();
   redraw();
}


void CalibrationTab::loadKernel() {
   QString fileName = QFileDialog::getOpenFileName(this, "Load Kernel File");
   emit showMessage("Loading kernel file...", 0);
   if (fileName != NULL) {
      getCurrentClassifier()->loadClassificationFile(fileName.toStdString());
   }
   emit showMessage("Kernel file loaded.", 5000);
   this->redraw();
}

void CalibrationTab::loadKernelFile(std::string f) {
   emit showMessage("Loading kernel file...", 0);
   getCurrentClassifier()->loadClassificationFile(f);
   emit showMessage("Kernel file loaded.", 5000);
   this->redraw();
}

void CalibrationTab::saveBothKernels() {
    topClassifier->saveClassificationFile(topKernelPath);
    topClassifier->saveNnmc(topNNMCPath);
    botClassifier->saveClassificationFile(botKernelPath);
    botClassifier->saveNnmc(botNNMCPath);
}

void CalibrationTab::saveKernel() {
   if (getCurrentClassifier()->classificationFileOpened()) {
       std::string kernelPath;
       std::string nnmcPath;
      if (isTop()) {
          kernelPath = topKernelPath;
          nnmcPath = topNNMCPath;
      } else {
          kernelPath = botKernelPath;
          nnmcPath = botNNMCPath;
      }

      emit showMessage("Saving kernel file...", 1000);
      getCurrentClassifier()->saveClassificationFile(kernelPath);
      getCurrentClassifier()->saveNnmc(nnmcPath);
      emit showMessage((std::string("Kernel file saved to: ") + kernelPath).c_str(), 5000);
   } else {
      QMessageBox::warning(this, "Cannot save Kernel", "Kernel file not open");
   }
}

void CalibrationTab::saveAsKernel() {
   if (getCurrentClassifier()->classificationFileOpened()) {
      cout << "In save as kernel" << endl;
      cout << this << endl;
      QString fileName = QFileDialog::getSaveFileName(this, "Save Kernel File");
      if (fileName != NULL) {
         getCurrentClassifier()->saveClassificationFile(fileName.toStdString());
      }
   } else {
      QMessageBox::warning(this, "Cannot save Kernel", "Kernel file not open");
   }
}


void CalibrationTab::saveNnmc() {
   if (getCurrentClassifier()->classificationFileOpened()) {
      QString fileName = QFileDialog::getSaveFileName(this, "Save NNMC File");
      if (fileName != NULL) {
         getCurrentClassifier()->saveNnmc(fileName.toStdString());
      }
   } else {
      QMessageBox::warning(this, "Cannot save NNMC", "Kernel file not open");
   }
}

void CalibrationTab::pushNnmc() {
   if (getCurrentClassifier()->classificationFileOpened()) {
      QString fileName = "/tmp/nnmc.cal";
      getCurrentClassifier()->saveNnmc(fileName.toStdString());
#ifndef QT_NO_CURSOR
      setCursor(Qt::WaitCursor);
#endif
      QStringList items;
      items << tr("robot1.local") << tr("robot2.local") <<
              tr("localhost");
      items.sort();

      bool ok;
      QString item = QInputDialog::getItem(this, tr(""),
                                           tr("Connect to:"), items, 0, false, &ok);

      if (!ok) {
          return;
      }

      ftp = new QFtp(this);
      connect(ftp, SIGNAL(commandFinished(int, bool)),
            this, SLOT(ftpCommandFinished(int, bool)));
      connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)),
            this, SLOT(updateDataTransferProgress(qint64, qint64)));

      ftp->setProxy("roborouter.ai.cse.unsw.edu.au", 2121);
      ftp->connectToHost(item);
      ftp->login("root", "");
      ftp->cd("/etc/runswift");

      emit showMessage(tr("Connecting to FTP server %1...").arg("soldier"));
      nnmcFile = new QFile(fileName);
      if (!nnmcFile->open(QIODevice::ReadOnly)) {
         QMessageBox::information(this, tr("FTP"),
               tr("Unable to load the file %1: %2.").
               arg(fileName).arg(nnmcFile->errorString()));
         delete nnmcFile;
         return;
      }

      ftp->put(nnmcFile, "nnmc.cal");

      progressDialog->setLabelText(tr("Uploading %1...").arg(fileName));
      progressDialog->exec();
   } else {
      QMessageBox::warning(this, "Cannot save NNMC", "Kernel file not open");
   }
}

void CalibrationTab::giveUp() {
   exit(0);
}


void CalibrationTab::setColour(int radio_id) {
   this->colour = (Colour) radio_id;
   redraw();
}


void CalibrationTab::setOverlay(int radio_id) {
   this->overlay = (Overlay) radio_id;
   redraw();
}

void CalibrationTab::undoAction() {
   cout << "Undo action pushed" << endl;
   if (getCurrentClassifier()->canUndo()) {
      getCurrentClassifier()->undo();
      redraw();
   }
}

QPixmap CalibrationTab::drawCrosshair() {
   bool top = isTop();
   int R = (top) ? TOP_IMAGE_ROWS : BOT_IMAGE_ROWS;
   int C = (top) ? TOP_IMAGE_COLS : BOT_IMAGE_COLS;

   QPixmap preRender = QPixmap::fromImage(
         lastRendering.scaled(BOT_IMAGE_COLS, BOT_IMAGE_ROWS));
   if (top) {
      preRender = QPixmap::fromImage(
            lastRendering.scaled(TOP_IMAGE_COLS, TOP_IMAGE_ROWS));
   }
   if (!naoData || naoData->getIsPaused() == false || !currentFrame) {
      camLabel->setCursor(QCursor());
      return preRender;
   }

   camLabel->setCursor(QCursor(Qt::BlankCursor));

   QPainter p(&preRender);
   p.setBrush(QBrush(QColor(255, 255, 255)));
   p.drawLine(mousePosition.x(), 0, mousePosition.x(),
      mousePosition.y()-2);
   p.drawLine(mousePosition.x(), mousePosition.y()+2,
      mousePosition.x(), R);
   p.drawLine(0, mousePosition.y(), mousePosition.x()-2, mousePosition.y());
   p.drawLine(mousePosition.x()+2, mousePosition.y(),
              C, mousePosition.y());

   if (mousePosition.x() >= 0 && mousePosition.x() < C &&
         mousePosition.y() >= 0 && mousePosition.y() < R) {
      stringstream message;
      QPoint translatedMousePos = translateToZoomedImageCoords(
                                 QPoint(mousePosition.x(), mousePosition.y()));

      message << "(" << translatedMousePos.x() <<
                 "," << translatedMousePos.y() << ")";
      QColor colour = QColor(this->getRGB(translatedMousePos.x(),
                             translatedMousePos.y(), currentFrame, C));

      message << " - (" << colour.red() << ", " << colour.green()
              << ", " << colour.blue() << ")";
      emit showMessage(QString(message.str().c_str()), 0);
   }
   return preRender;
}

void CalibrationTab::redraw() {
   bool top = isTop();
   currentFrame = topFrame;
   lastRendering = QImage(TOP_IMAGE_COLS, TOP_IMAGE_ROWS,
                          QImage::Format_RGB32);
   if (!top) {
      currentFrame = botFrame;
      lastRendering = QImage(BOT_IMAGE_COLS, BOT_IMAGE_ROWS,
                             QImage::Format_RGB32);
   }
   if (currentFrame) {
      QImage *image = new QImage(IMAGE_COLS, IMAGE_ROWS, QImage::Format_RGB32);

      drawImage(image);
      drawOverlays(image);
      drawPointCloud();

      QTransform prevTransform;
      prevTransform = prevTransform.translate(+prevMouseX, +prevMouseY);
      prevTransform = prevTransform.scale(1.0/prevZoomLevel, 1.0/prevZoomLevel);
      prevTransform = prevTransform.translate(-prevMouseX, -prevMouseY);

      QRectF realMouseCoords = prevTransform.mapRect(
                               QRectF(mouseX, mouseY, 0, 0));
      mouseX = realMouseCoords.left();
      mouseY = realMouseCoords.top();

      QTransform transform;
      transform = transform.translate(+mouseX, +mouseY);
      transform = transform.scale(1.0/zoomLevel, 1.0/zoomLevel);
      transform = transform.translate(-mouseX, -mouseY);
      imagePixmap = QPixmap(QPixmap::fromImage(
                           image->scaled(IMAGE_COLS, IMAGE_ROWS)));
      QPainter painter(&lastRendering);
      QRectF bound = transform.mapRect(QRectF(0, 0, IMAGE_COLS, IMAGE_ROWS));

      painter.drawImage(QRectF(0, 0, IMAGE_COLS, IMAGE_ROWS),
                        imagePixmap.toImage(), bound);


      prevZoomLevel = zoomLevel;
      prevMouseX = mouseX;
      prevMouseY = mouseY;
      delete image;
   } else {
      imagePixmap = QPixmap(IMAGE_COLS, IMAGE_ROWS);
      imagePixmap.fill(Qt::darkGray);
      lastRendering = imagePixmap.toImage();
   }

   QPixmap preRender = drawCrosshair();
   preRender = preRender.scaled(BOT_IMAGE_COLS, BOT_IMAGE_ROWS);
   camLabel->setPixmap(preRender);
}

void CalibrationTab::mouseMoveEvent(QMouseEvent* event) {
   bool top = isTop();
   mousePosition = event->pos();
   if (top) mousePosition *= 2;
   mousePosition -= camLabel->pos();

   if (currentFrame) {
      QPixmap preRender = drawCrosshair();
      preRender = preRender.scaled(BOT_IMAGE_COLS, BOT_IMAGE_ROWS);
      camLabel->setPixmap(preRender);
   } else {
      camLabel->setCursor(QCursor());
   }
}

Classifier* CalibrationTab::getCurrentClassifier() {
    if (isTop()) {
        return topClassifier;
    } else {
        return botClassifier;
    }
}

bool CalibrationTab::isTop() {
    return topCamera->isChecked();
}

void CalibrationTab::drawImage(QImage *image) {
   bool top = isTop();
   int R = (top) ? TOP_IMAGE_ROWS : BOT_IMAGE_ROWS;
   int C = (top) ? TOP_IMAGE_COLS : BOT_IMAGE_COLS;
   for (int row = 0; row < R; ++row) {
      for (int col = 0; col < C; ++col) {
         if (top) {
            image->setPixel(col, row, getRGB(col, row, topFrame, C));
         } else {
            image->setPixel(col, row, getRGB(col, row, botFrame, C));
         }
      }
   }
}


void CalibrationTab::drawOverlays(QImage *image) {
   bool top = isTop();
   int C = (top) ? TOP_IMAGE_COLS : BOT_IMAGE_COLS;
   uint8_t y, u, v;
   switch (overlay) {
      case oNONE:
         break;
      case oSELECTED:
         for (int i = 0; i < IMAGE_COLS; ++i) {
            for (int j = 0; j < IMAGE_ROWS; ++j) {
               y = gety(currentFrame, j, i, C);
               u = getu(currentFrame, j, i, C);
               v = getv(currentFrame, j, i, C);
               if (getCurrentClassifier()->getClassifiedColour(y, u, v) == colour) {
                  QColor classifiedColour = CPLANE_COLOURS[
                        getCurrentClassifier()->getClassifiedColour(y, u, v)];
                  image->setPixel(i, j, classifiedColour.rgb());
               }
            }
         }
         break;
      case oSELECTED_UNCLASSIFIED:
         for (int i = 0; i < IMAGE_COLS; ++i) {
            for (int j = 0; j < IMAGE_ROWS; ++j) {
               y = gety(currentFrame, j, i, C);
               u = getu(currentFrame, j, i, C);
               v = getv(currentFrame, j, i, C);
               if (getCurrentClassifier()->getClassifiedColour(y, u, v) == cUNCLASSIFIED ||
                        getCurrentClassifier()->getClassifiedColour(y, u, v) == colour) {
                  image->setPixel(i, j, CPLANE_COLOURS[
                        getCurrentClassifier()->getClassifiedColour(y, u, v)].rgb());
               }
            }
         }
         break;
      case oALL:
         for (int i = 0; i < IMAGE_COLS; ++i) {
            for (int j = 0; j < IMAGE_ROWS; ++j) {
               y = gety(currentFrame, j, i, C);
               u = getu(currentFrame, j, i, C);
               v = getv(currentFrame, j, i, C);
               if (getCurrentClassifier()->getClassifiedColour(y, u, v) != cUNCLASSIFIED) {
                  image->setPixel(i, j, CPLANE_COLOURS[
                     getCurrentClassifier()->getClassifiedColour(y, u, v)].rgb());
               }
            }
         }
         break;
      case oALL_UNCLASSIFIED:
         for (int i = 0; i < IMAGE_COLS; ++i) {
            for (int j = 0; j < IMAGE_ROWS; ++j) {
               y = gety(currentFrame, j, i, C);
               u = getu(currentFrame, j, i, C);
               v = getv(currentFrame, j, i, C);
               image->setPixel(i, j,
               CPLANE_COLOURS[getCurrentClassifier()->getClassifiedColour(y, u, v)].rgb());
            }
         }
         break;
      default:
         QMessageBox::warning(this, "Error",
                     "Somebody broke the code, check the overlays");
   }

   if (checkVisionOverlay->isChecked()) {
      vision->currentFrame = currentFrame;
      static boost::shared_array<uint8_t> ptr(reinterpret_cast<uint8_t*>(getCurrentClassifier()->getNnmcPointer()));
      vision->nnmc_top.nnmc = ptr;
      vision->nnmc_bot.nnmc = ptr;
      // vision->nnmc = getCurrentClassifier()->getNnmcPointer();
      vision ->processFrame();
      QPainter painter(image);
      painter.setPen(QColor(1, 0, 0));
      painter.setBrush(QBrush(QColor(255, 255, 0)));

      vector<Point>::const_iterator p;
      for (p = vision->fieldEdgeDetection.edgePointsTop.begin();
           p != vision->fieldEdgeDetection.edgePointsTop.end(); ++p) {
         painter.drawEllipse(QPoint((*p).x(), (*p).y()), 2, 2);
      }
      for (p = vision->fieldEdgeDetection.edgePointsBot.begin();
           p != vision->fieldEdgeDetection.edgePointsBot.end(); ++p) {
         painter.drawEllipse(QPoint((*p).x(), (*p).y()), 2, 2);
      }

      
      for (unsigned int i = 0; i < vision->fieldEdgeDetection.fieldEdges.size(); ++i) {
         const RANSACLine &line = vision->fieldEdgeDetection.fieldEdges[i].imageEdge;
         painter.setPen(QColor(255, 255, 0));
         painter.drawLine(
               0,
               -1*(float)line.t3 / line.t2,
               IMAGE_COLS-1,
               ((float)(-line.t3 - line.t1) * (IMAGE_COLS-1)) / line.t2);
      }
   }
}

void CalibrationTab::drawPointCloud() {
   pointCloud.points.resize(0);
   if (getCurrentClassifier()->classificationFileOpened()) {
      bool selectedOnly = qrbControls[0]->isChecked();
      static const int SPACING = 3;
      for(int y = 0; y < 256; y += SPACING)
         for(int u = 0; u < 256; u += SPACING)
            for(int v = 0; v < 256; v += SPACING) {
               Colour colour = getCurrentClassifier()->getClassifiedColour(y, u, v);
               if(colour != cUNCLASSIFIED &&
                  (!selectedOnly || colour == this->colour)) {
                  QColor classifiedColour = CPLANE_COLOURS[colour];
                  int r, g, b;
                  classifiedColour.getRgb(&r, &g, &b);
                  pointCloud.points.push_back(make_pair(
                              qglviewer::Vec(r / 255.0, g / 255.0, b / 255.0),
                              qglviewer::Vec(y / 255.0, u / 255.0, v / 255.0)));
               }
            }
   }
}

bool CalibrationTab::eventFilter(QObject *object, QEvent *event) {
   if ((object == camLabel) && (event->type() == QEvent::MouseButtonPress)) {
      return classifyMouseEvent(static_cast<QMouseEvent*>(event));
   } else if ((object == camLabel) && (event->type() == QEvent::Wheel)) {
      return classifyWheelEvent(static_cast<QWheelEvent*>(event));
   } else {
      return false;
   }
}

bool CalibrationTab::classifyWheelEvent(QWheelEvent *e) {
   bool top = isTop();
   int oldZoomLog = zoomLog;
   zoomLog += e->delta()/100;

   if (zoomLog < 0) zoomLog = 0;

   if (zoomLog > ZOOM_LIMIT) zoomLog = ZOOM_LIMIT;

   zoomLevel = 1u<<zoomLog;


   mouseX = e->x();
   mouseY = e->y();

   if (top) {
      mouseX*=2;
      mouseY*=2;
   }

   if (zoomLog != oldZoomLog)  redraw();
   return true;
}

bool CalibrationTab::classifyMouseEvent(QMouseEvent *e) {
   uint8_t y = 0, u = 0, v = 0;
   int imageX, imageY;
   bool top = isTop();

   if (e->button() == Qt::RightButton) {
      QString fileName = QFileDialog::getSaveFileName(this, "Save image");
      if (fileName != "") {
         QImage(this->lastRendering).save(fileName);
      }
   }

   if (e->button() != Qt::LeftButton) {
      // Classify on left click only
      return false;
   }
   if (!getCurrentClassifier()->classificationFileOpened()) {
      QMessageBox::warning(this, "Error", "No Kernel file, can't classify.");
      return false;
   }
   if (!currentFrame) {
      QMessageBox::warning(this, "Error",
            "No frame, what are you classifying?");
      return false;
   }
   QPoint mouse (e->x(), e->y());
   if (top) mouse *= 2;
   
   QPoint translatedMousePos = translateToZoomedImageCoords(mouse);
   imageX = translatedMousePos.x();
   imageY = translatedMousePos.y();


   const int COLS = (top) ? TOP_IMAGE_COLS : BOT_IMAGE_COLS;

   getCurrentClassifier()->beginAction();
   y = gety(currentFrame, imageY, imageX, COLS);
   u = getu(currentFrame, imageY, imageX, COLS);
   v = getv(currentFrame, imageY, imageX, COLS);

   // update radii
   float weight = 1;
   int yRadius = 10, uRadius = 20, vRadius = 20;
   if (checkAutoWeight->isChecked()) {
      float weights[CMAX];
      getCurrentClassifier()->colourInfo(y, u, v, weights);
      float totalWeight = 0.0;
      for (int i = 0; i < CMAX; i++) {
         totalWeight += weights[i];
      }
      yRadius = static_cast<int>(10.0 / (1.0 + totalWeight));
      uRadius = static_cast<int>(20.0 / (1.0 + totalWeight));
      vRadius = static_cast<int>(20.0 / (1.0 + totalWeight));
      inRange(yRadius, 1, 10);
      inRange(uRadius, 1, 20);
      inRange(vRadius, 1, 20);
      weight = 1.0 + (totalWeight / 10.0);
   }

   // classify!
   if (!getCurrentClassifier()->isMostlyClassified(y, u, v, colour)) {
      getCurrentClassifier()->classify(y, u, v, weight, colour, yRadius,
                           uRadius, vRadius, false);
   }
   getCurrentClassifier()->endAction();
   redraw();
   return true;
}

QPoint CalibrationTab::translateToZoomedImageCoords(QPoint point) {
   QTransform prevTransform;
   prevTransform = prevTransform.translate(+prevMouseX, +prevMouseY);
   prevTransform = prevTransform.scale(1.0/prevZoomLevel, 1.0/prevZoomLevel);
   prevTransform = prevTransform.translate(-prevMouseX, -prevMouseY);
   QRectF newMousePos = prevTransform.mapRect(QRectF(point.x(),
                                             point.y(), 0, 0));
   return QPoint(newMousePos.left(), newMousePos.top());
}

// TODO(brockw): see if this can be genericized into tab.cpp, so it's not in
// every tab
void CalibrationTab::newNaoData(NaoData *naoData) {
   if (!naoData || !naoData->getCurrentFrame().blackboard) {
      imagePixmap.fill(Qt::darkGray);
      camLabel->setPixmap(imagePixmap);
      currentFrame = NULL;
      topFrame = NULL;
      botFrame = NULL;
   } else {
      this->naoData = naoData;
      Blackboard *blackboard = naoData->getCurrentFrame().blackboard;
      if (((topFrame = readFrom(vision, topFrame)) != NULL) && 
          ((botFrame = readFrom(vision, botFrame)) != NULL)) 
         if (parent->currentIndex() == parent->indexOf(this))
            redraw();
   }
}

void CalibrationTab::redrawSlot() {
   redraw();
}

void CalibrationTab::ftpCommandFinished(int, bool error) {
#ifndef QT_NO_CURSOR
   setCursor(Qt::ArrowCursor);
#endif

   bool closeFtp = false;

   if (ftp->currentCommand() == QFtp::ConnectToHost) {
      if (error) {
         QMessageBox::information(this, tr("FTP"),
               tr("Unable to connect to the FTP server "
                  "at %1. Please check that the host "
                  "name is correct.").arg("soldier"));
         closeFtp = true;
      } else {
         emit showMessage(tr("Logged onto %1.").arg("soldier"));
      }
   } else if (ftp->currentCommand() == QFtp::Put) {
      if (error) {
         emit showMessage(tr("Canceled upload of %1.").
               arg(nnmcFile->fileName()));
         nnmcFile->close();
         nnmcFile->remove();
      } else {
         emit showMessage(tr("Uploaded %1 to /etc/runswift.").
               arg(nnmcFile->fileName()));
         nnmcFile->close();
      }
      delete nnmcFile;
      progressDialog->hide();
      closeFtp = true;
   }
   if (closeFtp) {
      ftp->abort();
      ftp->deleteLater();
      ftp = 0;
#ifndef QT_NO_CURSOR
      setCursor(Qt::ArrowCursor);
#endif
   }
}

void CalibrationTab::updateDataTransferProgress(qint64 readBytes,
      qint64 totalBytes) {
   progressDialog->setMaximum(totalBytes);
   progressDialog->setValue(readBytes);
}

void CalibrationTab::cancelUpload() {
   ftp->abort();
}
