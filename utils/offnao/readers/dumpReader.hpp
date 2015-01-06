#pragma once
#include <QString>
#include <cstdio>
#include "readers/reader.hpp"
#include "../../robot/perception/vision/yuv.hpp"
/*
 *  Simple reader that reads in video photage from file.
 */
class DumpReader : public Reader {
   Q_OBJECT
   public:
      explicit DumpReader(QString filename);
      explicit DumpReader(QString filename, const NaoData &naoData);
      virtual void run();
   private:
      FILE *dumpFile;
   public slots:
      virtual void stopMediaTrigger();
};
