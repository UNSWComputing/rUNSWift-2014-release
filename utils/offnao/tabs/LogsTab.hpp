#pragma once

#include "tab.hpp"
#include <QComboBox>
#include "LogTab.hpp"
#include "ui_LogsTab.h"

class LogsTab : public Tab {
   Q_OBJECT
   std::vector<LogTab *> vTabs;
   Ui::LogsTab uiLogsTab;
   void clear();
   QString naoName;
   QProcess logsList;
   public:
      LogsTab(QTabWidget *parent, QComboBox *hostname);
      virtual ~LogsTab();
   public slots:
      void newNaoData(NaoData *) {}
      /**
       * connects to the robot's latest logs
       */
      void connectToNao(const QString &naoName);
      /**
       * clears current tabs and connects to different logs
       */
      void openLogs(const QString &dirName);
      /**
       * changes the log level on the robot
       */
      void changeLog(const QString &logLevel);
      /**
       * updates the list of log directories
       */
      void updateDirList();
      void readAllStandardError();
      void stateChanged(QProcess::ProcessState);
      void error(QProcess::ProcessError);
      void finished(int, QProcess::ExitStatus);
   signals:
      void sendCommandToRobot(QString item);
};
