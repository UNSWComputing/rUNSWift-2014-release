#include "ui_visualiser.h"

template<class ReaderClass, typename Void>
bool Visualiser::reconnect(const Void &constructorArg1) {
   Reader *r;
   if (!reader) {
      r = new ReaderClass(constructorArg1);
   } else if (reader->isFinished()) {
      r = new ReaderClass(constructorArg1, reader->naoData);
      emit readerClosed();
      delete reader;
   } else {
      reader->finishUp();
      qDebug("Try to destroy reader. Wait for thread to exit.");
      if(reader->wait(2000)) {
         r = new ReaderClass(constructorArg1, reader->naoData);
         emit readerClosed();
         delete reader;
      } else {
         ui->statusBar->showMessage(QString("Failed to close reader."));
         return false;
      }
   }
   reader = r;
   setUpReaderSignals(reader);
   reader->start();
   return true;
}
