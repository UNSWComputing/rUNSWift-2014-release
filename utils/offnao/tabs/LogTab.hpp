#pragma once

#include "ui_LogTab.h"
#include <QProcess>

class LogTab : public QMainWindow {
   Q_OBJECT
   const QString sName;
   QProcess process;
   Ui::LogTab uiTab;
public:
   LogTab(const QString &name, const QString &command);
   virtual ~LogTab();
   const QString &name() const;
public slots:
   void readAllStandardOutput();
   void readAllStandardError();
   void stateChanged(QProcess::ProcessState);
   void error(QProcess::ProcessError);
   void finished(int, QProcess::ExitStatus);
   void find(const QString &text);
   void unfind(const QString &text);
};
