#pragma once

#include <iostream>
#include <QLabel>
#include <QPixmap>
#include <QPoint>
#include <QPainter>
#include "perception/vision/yuv.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "utils/Logger.hpp"
#include "naoData.hpp"
#include "frame.hpp"
#include "types/RansacTypes.hpp"
#include "types/BroadcastData.hpp"

class FieldView: public QLabel {
   public:
      FieldView();
      ~FieldView();

      void redraw(NaoData *naoData);

      // Function to just draw a single robot coordinate on field
      void redraw(std::vector<AbsCoord> &robotPos);

      void redraw(std::vector<AbsCoord> &robotPos,
                  std::vector<AbsCoord> &ballPos);
      
   private:
      QImage image;
      QPixmap imagePixmap;
      QPixmap *renderPixmap;
};

