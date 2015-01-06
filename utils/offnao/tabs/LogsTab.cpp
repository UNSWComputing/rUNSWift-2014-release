#include "LogsTab.hpp"
#include <QDebug>

using namespace std;

LogsTab::LogsTab(QTabWidget *parent, QComboBox *host) : Tab(parent) {
   uiLogsTab.setupUi(this);
   connect(host, SIGNAL(activated(const QString &)),
           this, SLOT(connectToNao(const QString &)));
   connect(uiLogsTab.cbDirectories, SIGNAL(activated(const QString &)),
           this, SLOT(openLogs(const QString &)));
   connect(uiLogsTab.cbLevels, SIGNAL(activated(const QString &)),
           this, SLOT(changeLog(const QString &)));
   connect(&logsList, SIGNAL(readyReadStandardOutput()),
           this, SLOT(updateDirList()));
   connect(&logsList, SIGNAL(readyReadStandardError()),
           this, SLOT(readAllStandardError()));
//   connect(&logsList, SIGNAL(stateChanged(QProcess::ProcessState)),
//           this, SLOT(stateChanged(QProcess::ProcessState)));
   connect(&logsList, SIGNAL(error(QProcess::ProcessError)),
           this, SLOT(error(QProcess::ProcessError)));
   connect(&logsList, SIGNAL(finished(int, QProcess::ExitStatus)),
           this, SLOT(finished(int, QProcess::ExitStatus)));
}

void LogsTab::clear() {
   uiLogsTab.twLogs->clear();
   foreach (LogTab *t, vTabs) {
      delete t;
   }
   vTabs.clear();
}

void LogsTab::connectToNao(const QString &naoName) {
   this->naoName = naoName;
   logsList.terminate();
   logsList.waitForFinished(1000);
   logsList.start("ssh nao@" + naoName + " \"while ls -t /var/volatile/runswift/ && sleep 1; do true; done\"",
      QIODevice::ReadOnly | QIODevice::Text);
}

void LogsTab::updateDirList() {
   QStringList newList =
     QString(logsList.readAllStandardOutput()).split("\n",
                                                     QString::SkipEmptyParts);
   if (uiLogsTab.cbDirectories->currentIndex() == -1 ||
       uiLogsTab.cbDirectories->count() == 0 ||
       vTabs.size() == 0 ||
       uiLogsTab.twLogs->count() == 0) {
      uiLogsTab.cbDirectories->clear();
      uiLogsTab.cbDirectories->addItems(newList);
      uiLogsTab.cbDirectories->setCurrentIndex(0);
      openLogs(newList[0]);
   } else {
      int insertedItems = 0;
      int currentIndex = uiLogsTab.cbDirectories->currentIndex();
      foreach (const QString &dir, newList)
         if (uiLogsTab.cbDirectories->findText(dir) == -1)
            uiLogsTab.cbDirectories->insertItem(insertedItems++, dir);
      if (currentIndex == 0 &&
         newList[0] != uiLogsTab.cbDirectories->currentText() &&
         insertedItems) {
         uiLogsTab.cbDirectories->setCurrentIndex(0);
         openLogs(newList[0]);
      }
   }
}

void LogsTab::openLogs(const QString &dirName) {
   LogTab * ltCurrent = dynamic_cast<LogTab *>(uiLogsTab.twLogs->currentWidget());
   QString sCurrent;
   if (ltCurrent)
      sCurrent = ltCurrent->name();
   clear();
   vTabs.push_back(new LogTab("Perception",
                              "ssh nao@"+naoName+" \"until sleep 1 && [ -r /var/volatile/runswift/"+dirName+"/Perception ]; do true; done; tail -f /var/volatile/runswift/"+dirName+"/Perception\""));
   vTabs.push_back(new LogTab("All",
                              "ssh nao@"+naoName+" \"tail -f /var/volatile/runswift/"+dirName+"/*\""));
   //TODO:allow tabs to be closed
   /* Set up the tabs */
   foreach (LogTab *t, vTabs) {
      uiLogsTab.twLogs->addTab(t, t->name());
      if (sCurrent == t->name())
         uiLogsTab.twLogs->setCurrentWidget(t);
   }
}

void LogsTab::changeLog(const QString &logLevel) {
   emit sendCommandToRobot("--debug.log=" + logLevel);//TODO:the robot end of this
}

void LogsTab::readAllStandardError() {
   qDebug() << logsList.readAllStandardError();
}

void LogsTab::stateChanged(QProcess::ProcessState state) {
   qDebug() << __PRETTY_FUNCTION__ << " - " << state;
}

void LogsTab::error(QProcess::ProcessError error) {
   qDebug() << __PRETTY_FUNCTION__ << " - " << error;
}

void LogsTab::finished(int exitCode, QProcess::ExitStatus exitStatus) {
   qDebug() << __PRETTY_FUNCTION__ << " - " << exitCode << " - " << exitStatus;
   qDebug() << logsList.readAllStandardOutput();
   readAllStandardError();   
}

LogsTab::~LogsTab() {
   clear();
}
