#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <string>
#include <sstream>
#include <cmath>
#include "utils/basic_maths.hpp"
#include "readers/dumpReader.hpp"
#include "blackboard/Blackboard.hpp"
#include "progopts.hpp"

DumpReader::DumpReader(QString fileName) {
   dumpFile = fopen(fileName.toAscii().data(), "r");
   if (dumpFile == NULL) {
      // QMessageBox::warning(this, "Error", "Could not open dump file");
   }
}

DumpReader::DumpReader(QString fileName, const NaoData &naoData) :
Reader(naoData) {
   dumpFile = fopen(fileName.toAscii().data(), "r");
   if (dumpFile == NULL) {
      // QMessageBox::warning(this, "Error", "Could not open dump file");
   }
}

void DumpReader::run() {
   Frame frame;
   int currentIndex = 0;

   // load whole file into memory  dodgey for now.
   int frameLoaded = 1;
   do {
      uint8_t* vcf =
            (uint8_t *) malloc(sizeof(uint8_t)*(IMAGE_COLS*IMAGE_ROWS*2));
      if (fread(vcf, IMAGE_COLS*IMAGE_ROWS*2, 1, dumpFile) != 1) {
         free(vcf);
         break;
      } else {
         Blackboard *blackboard = new Blackboard(config);
         OffNaoMask_t mask = RAW_IMAGE_MASK;
         // writeTo(, mask, mask);
         blackboard->write(&(blackboard->mask), mask);
         writeTo(vision, currentFrame, (const uint8_t*) vcf);
         frame.blackboard = blackboard;
         naoData.appendFrame(frame);
         std::stringstream s;
         s << "Loading frame " << frameLoaded << " from YUV dump.";
         emit showMessage(s.str().c_str(), 2000);
         frameLoaded++;
      }
   } while (1);
   std::stringstream s;
   s << "Finshed loading YUV dump which consisted of " <<
        frameLoaded << " frames.";
   emit showMessage(s.str().c_str(), 5000);
   emit newNaoData(&naoData);

   isAlive = true;
   while (isAlive) {
      if (!naoData.getIsPaused() && naoData.getCurrentFrameIndex() <
           naoData.getFramesTotal() - 1) {
         naoData.nextFrame();
         emit newNaoData(&naoData);
      } else if (currentIndex != naoData.getCurrentFrameIndex()) {
         emit newNaoData(&naoData);
      }
      currentIndex = naoData.getCurrentFrameIndex();
      msleep(500);
   }
   emit newNaoData(NULL);
}


void DumpReader::stopMediaTrigger() {
}
