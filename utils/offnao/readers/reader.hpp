#pragma once

#include <QThread>
#include <QObject>
#include <QDebug>
#include <QAction>
#include "naoData.hpp"
#include "frame.hpp"
#include "transmitter/TransmitterDefs.hpp"
/*
 * This will be a virtual class that creates and feeds a naoData class.
 * The two intended subclasses to implement this interface will be
 * The diskReader and networkReader classes. edit: now includes dump reader for vision only debugging.
 */
class Reader :  public QThread {
   Q_OBJECT
   public slots:
   virtual void forwardMediaTrigger();
   virtual void backwardMediaTrigger();
   virtual void pauseMediaTrigger();
   virtual void playMediaTrigger();
   virtual void sliderMoved(int amount);
   virtual void refreshNaoData();
   virtual void stopMediaTrigger();
   virtual void recordMediaTrigger();
   /**
    * only for NetworkReader, but not sure how to put it there
    */
   virtual void sendCommandLineString(QString item) {};

   public:
   Reader() {}
   Reader(const NaoData &naoData) : naoData(naoData) {}
   virtual ~Reader() {}

   NaoData naoData;

   /* main loop that runs when the thread starts */
   virtual void run() = 0;
   virtual void finishUp() { isAlive = false;}
   void setSendingMask(OffNaoMask_t mask) {
      this->mask = mask;
   }
   signals:
   void newNaoData(NaoData *naoData);
   virtual void showMessage(const QString &, int timeout = 0);
   /**
    * lets the user select a file to open, then loads the file
    */
   void openFile();
   void disconnectFromNao();
   protected:
   bool isAlive;
   OffNaoMask_t mask;
};
