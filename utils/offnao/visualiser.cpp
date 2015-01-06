#include <boost/shared_ptr.hpp>
#include <QButtonGroup>
#include <QDebug>
#include <QDockWidget>
#include <QInputDialog>
#include <string>
#include <iostream>
#include <fstream>
#include "visualiser.hpp"
#include "visualiser.tcc"
#include "ui_visualiser.h"
#include "readers/recordReader.hpp"
#include "readers/bbdReader.hpp"
#include <utils/Logger.hpp>
#include <thread/Thread.hpp>
#include <stdlib.h>
#include <time.h>
#include "progopts.hpp"

using namespace boost;
using std::string;
using namespace std;

Visualiser::Visualiser(QWidget *parent)
   : QMainWindow(parent), ui(new Ui::Visualiser),
   reader(0), naoData(0), cb() {
      // call initLogs so that vision et al don't segfault when they llog
      Thread::name = "Visualiser";
      Logger::init(config["debug.logpath"].as<string>(), config["debug.log"].as<string>(), true);

      lastTabIndex = -1;

      srand(time(0));
      extern bool offNao;
      offNao = true;


      vision = new Vision(config["vision.dumpframes"].as<bool>(),
            config["vision.dumprate"].as<int>(),
            config["vision.dumpfile"].as<string>(),
            config["vision.top_calibration"].as<string>(),
            config["vision.bot_calibration"].as<string>(),
            config["vision.goal_map"].as<string>(),
            config["vision.vocab"].as<string>(),
            config["debug.vision"].as<bool>(),
            config["vision.seeBluePosts"].as<bool>(),
            config["vision.seeLandmarks"].as<bool>());

      ui->setupUi(this);
      ui->centralWidget->setMinimumSize(1250, 600);

      initMenu();

      // add the media panel to the bottom of the window
      mediaPanel = new MediaPanel(ui->centralWidget);

      // widgets for connecting to the naos
      connectionBar = new QDockWidget;
      cb.setupUi(connectionBar);
      connect(cb.cbHost, SIGNAL(activated(const QString &)),
              this, SLOT(connectToNao(const QString &)));
      connect(cb.bgMasks, SIGNAL(buttonClicked(QAbstractButton *)),
              this, SLOT(writeToNao(QAbstractButton *)));

      // set up tab holder
      tabs = new QTabWidget(ui->centralWidget);
      tabs->setMinimumSize(600, 450);

      overviewTab =  new OverviewTab(tabs, ui->menuBar, vision);
      calibrationTab =  new CalibrationTab(tabs, ui->menuBar,
                            vision);
      visionTab = new VisionTab(tabs, ui->menuBar, vision);
      localisationTab = new LocalisationTab();
      cameraTab = new CameraTab(tabs, ui->menuBar, vision);
      sensorTab = new SensorTab(tabs, ui->menuBar, vision);
      cameraPoseTab = new CameraPoseTab(tabs, ui->menuBar, vision);
      graphTab = new GraphTab(tabs, ui->menuBar, vision);
      walkTab = new WalkTab(tabs, ui->menuBar, vision);
      jointsTab = new JointsTab(tabs, ui->menuBar, vision);
      zmpTab = new ZMPTab(tabs, ui->menuBar, vision);
      controlTab = new ControlTab(tabs, ui->menuBar, vision);
      surfTab =  new SurfTab(tabs, ui->menuBar, vision);
      icpTab = new ICPTab(tabs, ui->menuBar, vision);
      teamTab =  new TeamTab(tabs, ui->menuBar);
      tabVector.push_back(overviewTab);
      tabVector.push_back(teamTab);
      tabVector.push_back(calibrationTab);
      tabVector.push_back(visionTab);
      tabVector.push_back(surfTab);
      tabVector.push_back(icpTab);
      tabVector.push_back(localisationTab);
      tabVector.push_back(cameraTab);
      tabVector.push_back(sensorTab);
      tabVector.push_back(cameraPoseTab);
      tabVector.push_back(graphTab);
      tabVector.push_back(walkTab);
      tabVector.push_back(jointsTab);
      tabVector.push_back(zmpTab);
      tabVector.push_back(controlTab);
      tabVector.push_back(logsTab = new LogsTab(tabs, cb.cbHost));
			

      /* Set up the tabs */
      foreach (Tab *t, tabVector) {
         tabs->addTab(t, QString(t->metaObject()->className()).
                           remove(QRegExp("Tab$", Qt::CaseInsensitive)));
         connect(t, SIGNAL(showMessage(QString, int)),
                 ui->statusBar, SLOT(showMessage(QString, int)));
      }

      /* Used to redraw tabs when they are in focus */
      connect(tabs, SIGNAL(currentChanged(int)), this,
              SLOT(currentTabChanged(int)));

      ui->rootLayout->addWidget(mediaPanel, 1, 0, 1, 1);
      ui->rootLayout->addWidget(tabs, 0, 0, 1, 1);
   }

Visualiser::~Visualiser() {
   std::cout << "---- about to exit ----" << std::endl << std::flush;
   /*
   delete connectionBar;
   delete ui;
   for (unsigned int i = 0; i > tabVector.size(); i++) {
      delete tabVector[i];
   }
   */
}

void Visualiser::newNaoData(NaoData *naoData) {
   this->naoData = naoData;
}

// sets up the generic menus
void Visualiser::initMenu() {
   fileMenu = new QMenu("&File");
   ui->menuBar->addMenu(fileMenu);

   // set up file menu
   loadAct = new QAction(tr("&Load"), fileMenu);
   saveAsAct = new QAction(tr("&Save As"), fileMenu);

   exitAct = new QAction(tr("&Quit"), fileMenu);


   fileMenu->addAction(tr("Connect to &Nao"), this, SLOT(connectToNao()),
         QKeySequence(tr("Ctrl+N", "File|New")));
   fileMenu->addAction(tr("&Open File"), this, SLOT(openFileDailogue()),
         QKeySequence(tr("Ctrl+O", "File|Open")));
   fileMenu->addAction(tr("Load &Default Files"), this,
                        SLOT(loadDefaultFiles()),
         QKeySequence(tr("Ctrl+D", "File|Open")));
   // This is networkReader specific, but I'm not sure where to put it
   fileMenu->addAction(tr("Send Co&mmand Line String"), this,
                       SLOT(commandLineString()),
                            QKeySequence(tr("F5", "Refresh")));
   fileMenu->addAction(tr("&Save File"), this, SLOT(saveFileDailogue()),
                       QKeySequence(tr("Ctrl+S", "File|Save")));
   fileMenu->addAction(tr("&Close Current Reader"), this,
                       SLOT(disconnectFromNao()),
         QKeySequence(tr("Ctrl+W", "File|Close")));

   fileMenu->addAction(tr("&Quit"), this,
         SLOT(close()),
         QKeySequence(tr("Ctrl+Q", "Close")));

}


void Visualiser::close() {
   delete connectionBar;
   for (unsigned int i = 0; i > tabVector.size(); i++) {
      delete tabVector[i];
   }
   //delete tabs;
   //delete vision;
   delete ui;
   exit(0);
}

void Visualiser::openFileDailogue() {
   QString fileName =
     QFileDialog::getOpenFileName(this, "Open File",
                                  getenv("RUNSWIFT_CHECKOUT_DIR"),
                                  #if BOOST_HAS_COMPRESSION
                                  "Known file types (*.bbd *.yuv *.ofn *.ofn.gz *.ofn.bz2);;"
                                  #else
                                  "Known file types (*.bbd *.yuv *.ofn);;"
                                  #endif
                                  "Dump files (*.yuv);;"
                                  #if BOOST_HAS_COMPRESSION
                                  "Record files (*.ofn *.ofn.gz *.ofn.bz2)"
                                  #else
                                  "Record files (*.ofn);;"
                                  #endif
                                  "Blackboard Dump files (*.bbd)"
                                  );
   openFile(fileName);

}

void Visualiser::openFile (const QString &path)
{
   if (!path.isEmpty()) {
      if(path.endsWith(".bbd")) {
         reconnect<BBDReader, const QString &>(path);
      } else if(path.endsWith(".yuv"))
         reconnect<DumpReader, const QString &>(path);
      else if(path.endsWith(".ofn")
         #if BOOST_HAS_COMPRESSION
         || path.endsWith(".ofn.gz")  || path.endsWith(".ofn.bz2")
         #endif
         )
         reconnect<RecordReader, const QString &>(path);
      else
         ui->statusBar->showMessage(
           QString("Unknown file extension, modify %1:%2:%3").
                   arg(__FILE__, __PRETTY_FUNCTION__,
                       QString::number(__LINE__)));
   }
}

void Visualiser::saveFileDailogue() {
   QString fileName =
   QFileDialog::getSaveFileName(this, "Save File",
                                getenv("RUNSWIFT_CHECKOUT_DIR"),
                                       #if BOOST_HAS_COMPRESSION
                                       "Known file types ("/**.yuv */"*.ofn *.ofn.gz *.ofn.bz2);;"
                                       #else
                                       "Known file types ("/**.yuv */"*.ofn);;"
                                       #endif
                                       // "Dump files (*.yuv);;"
                                       #if BOOST_HAS_COMPRESSION
                                       "Record files (*.ofn *.ofn.gz *.ofn.bz2)"
                                       #else
                                       "Record files (*.ofn)"
                                       #endif
                                       );
                                       
   saveFile(fileName);
}

void Visualiser::saveFile (const QString &path)
{
   if (!path.isEmpty()) {
      if(path.endsWith(".ofn")
         #if BOOST_HAS_COMPRESSION
         || path.endsWith(".ofn.gz")  || path.endsWith(".ofn.bz2")
         #endif
         )
         RecordReader::write(path, *naoData);
      // else if(path.endsWith(".yuv"))
      //   saveDump(path);
      else {
         ui->statusBar->showMessage(
         QString("Unknown file extension, saving as %1.ofn").
         arg(path));
         RecordReader::write(path + ".ofn", *naoData);
      }
   }
}

void Visualiser::keyPressEvent(QKeyEvent *event)
{
   //((QMainWindow*)this)->keyPressEvent(event);
   if (!event->isAutoRepeat())
   {
      controlTab->keyPressEvent(event);
      //std::cout << "_" << event->key() << "\n";
   }
}

void Visualiser::keyReleaseEvent(QKeyEvent *event)
{
   //((QMainWindow*)this)->keyReleaseEvent(event);
   if (!event->isAutoRepeat())
   {
      controlTab->keyReleaseEvent(event);
      //std::cout << "^" << event->key() << "\n";
   }
}

void Visualiser::loadDefaultFiles() {
   if (reader) {
      QMessageBox::warning(this, "Close current session!",
      "Please close current session from the file menu.");
      return;
   }
   std::string line;
   std::ifstream f("../../../../def.txt");
   if (f.is_open()) {
      std::getline(f, line);
      std::ifstream fout(line.c_str());
      if (fout) {
         fout.close();
         QString name(line.c_str());
         reader = new DumpReader(name);
         setUpReaderSignals(reader);
         reader->start();
      } else {
         std::cout << "Could not find dump file" << std::endl;
      }

      std::getline(f,line);
      std::ifstream f2(line.c_str());
      if (f2) {
         f2.close();
         calibrationTab->loadKernelFile(line);
      } else {
         std::cout << "Could not find the kernel file" << std::endl;
      }

      std::getline(f,line);
      std::ifstream f3(line.c_str());
      if (f3) {
         f3.close();
         visionTab->loadNnmcFile(TOP_CAMERA,    line.c_str());
         visionTab->loadNnmcFile(BOTTOM_CAMERA, line.c_str());
      } else {
         std::cout << "Could not find the nnmc file" << std::endl;
      }

   }
   f.close();
}

/*
 * Not yet used. Probably will be for the network reader.
 */
void Visualiser::startRecording(QAction *action) {
}

void Visualiser::currentTabChanged(int tabIndex) {
   if (lastTabIndex != -1) {
      tabVector[lastTabIndex]->tabDeselected();
   }
   tabVector[tabIndex]->tabSelected();

   emit refreshNaoData();
}

OffNaoMask_t Visualiser::transmissionMask(const QAbstractButton *) {
   OffNaoMask_t mask = 0;
   // TODO(jayen): make more efficient by just twiddling qab in the prior mask
   QList<QAbstractButton *> buttons = cb.bgMasks->buttons();
   while (!buttons.isEmpty()) {
      QAbstractButton *button = buttons.takeFirst();
      if (button->isChecked()) {
         if (button == cb.cbBlackboard)
            mask |= BLACKBOARD_MASK;
         else if (button == cb.cbSaliency)
            mask |= SALIENCY_MASK;
         else if (button == cb.cbRaw)
            mask |= RAW_IMAGE_MASK;
         else if (button == cb.cbParticles)
            mask |= PARTICLE_FILTER_MASK;
         else if (button == cb.cbRobotFilter)
            mask |= ROBOT_FILTER_MASK;
         else if (button == cb.cbLandmarks)
            mask |= LANDMARKS_MASK;
         else
            ui->statusBar->showMessage("Error in parsing mask checkboxes!");
      }
   }
   return mask;
}

void Visualiser::connectToNao(const QString &naoName) {
   if(reconnect<NetworkReader, pair<const QString &, OffNaoMask_t> >
      (make_pair(naoName, transmissionMask(NULL)))) {
      NetworkReader *reader = dynamic_cast<NetworkReader *>(this->reader);
      Q_ASSERT(reader);
      connect(this, SIGNAL(sendCommandLineString(const QString&)), reader,
              SLOT(sendCommandLineString(QString)));
      connect(cameraPoseTab, SIGNAL(sendCommandToRobot(QString)),
              reader, SLOT(sendCommandLineString(QString)));
      connect(cameraTab, SIGNAL(sendCommandToRobot(QString)),
              reader, SLOT(sendCommandLineString(QString)));
      connect(logsTab, SIGNAL(sendCommandToRobot(QString)),
              reader, SLOT(sendCommandLineString(QString)));

      this->reader = reader;
      setUpReaderSignals(this->reader);
      this->reader->start();
      controlTab->setNao(naoName);
   }
}

void Visualiser::writeToNao(QAbstractButton *qab) {
   if (reader)
      // TODO(jayen): check reader is a networkreader
      ((NetworkReader*)reader)->write(transmissionMask(qab));
}
   
void Visualiser::connectToNao() {
   addDockWidget(Qt::TopDockWidgetArea, connectionBar);
   connectionBar->show();
   cb.cbHost->setFocus();
   cb.cbHost->lineEdit()->selectAll();
}

// This is networkReader specific, but I'm not sure where to put it
void Visualiser::commandLineString() {
   QString item = QInputDialog::getText(NULL, "Send String",
                                           tr("Command FieldEdge String:"));
   emit sendCommandLineString(item);
}

void Visualiser::disconnectFromNao() {
   if (reader && reader->isFinished() == false) {
      connect(reader, SIGNAL(finished()), this, SLOT(disconnectFromNao()));
      reader->finishUp();
      qDebug("Try to destroy reader. Wait for thread to exit.");
   } else if (reader) {
      emit readerClosed();
      delete reader;
      reader = 0;
      qDebug("Finished destroying reader");
      ui->statusBar->showMessage(QString("Reader destroyed."));
   }
}

void Visualiser::setUpReaderSignals(Reader *reader) {

   for (unsigned int i = 0; i < tabVector.size(); i++) {
      connect(reader, SIGNAL(newNaoData(NaoData*)), tabVector[i],
            SLOT(newNaoData(NaoData *)));
      connect(this, SIGNAL(readerClosed()), tabVector[i],
            SLOT(readerClosed()));
   }

   connect(reader, SIGNAL(newNaoData(NaoData*)), mediaPanel,
         SLOT(newNaoData(NaoData *)));
   connect(reader, SIGNAL(newNaoData(NaoData*)), this,
           SLOT(newNaoData(NaoData *)));
                 
   connect(mediaPanel->forwardAct, SIGNAL(triggered()), reader,
         SLOT(forwardMediaTrigger()));
   connect(mediaPanel->backAct, SIGNAL(triggered()), reader,
         SLOT(backwardMediaTrigger()));
   connect(mediaPanel->playAct, SIGNAL(triggered()), reader,
         SLOT(playMediaTrigger()));
   connect(mediaPanel->pauseAct, SIGNAL(triggered()), reader,
         SLOT(pauseMediaTrigger()));
   connect(mediaPanel->stopAct, SIGNAL(triggered()), reader,
         SLOT(stopMediaTrigger()));
   connect(mediaPanel->recordAct, SIGNAL(triggered()), reader,
         SLOT(recordMediaTrigger()));
   connect(mediaPanel->frameSlider, SIGNAL(valueChanged(int)),
         reader, SLOT(sliderMoved(int)));

   connect(this, SIGNAL(refreshNaoData()), reader, SLOT(refreshNaoData()));
   connect(mediaPanel->frameSlider, SIGNAL(sliderReleased()), reader,
         SLOT(refreshNaoData()));
   connect(reader, SIGNAL(showMessage(QString, int)), ui->statusBar,
           SLOT(showMessage(QString, int)));
   connect(reader, SIGNAL(openFile()), this, SLOT(openFileDailogue()));
   connect(reader, SIGNAL(disconnectFromNao()), this,
           SLOT(disconnectFromNao()));
   mediaPanel->recordButton->setFocus();
}
