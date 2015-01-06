#include "readers/reader.hpp"
#include <iostream>

#include "utils/basic_maths.hpp"

void Reader::forwardMediaTrigger() {
   naoData.nextFrame();
   emit newNaoData(&naoData);
   naoData.setPaused(true);
}

void Reader::backwardMediaTrigger() {
   naoData.prevFrame();
   emit newNaoData(&naoData);
   naoData.setPaused(true);
}

void Reader::pauseMediaTrigger() {
   naoData.setPaused(true);
}

void Reader::playMediaTrigger() {
   naoData.setPaused(false);
}
void Reader::sliderMoved(int amount) {
   int frame = round(((amount/100.0) * naoData.getFramesTotal()));
   naoData.setCurrentFrame(frame);
   naoData.setPaused(true);
   refreshNaoData();
}

void Reader::refreshNaoData() {
   if (naoData.getFramesTotal() > 0) {
      emit newNaoData(&naoData);
   }
}

void Reader::stopMediaTrigger() {
}

void Reader::recordMediaTrigger() {
}
