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

class ICPFieldView: public QLabel {
   public:
      ICPFieldView();
      ~ICPFieldView();

      void redraw();
      void redraw(Blackboard *blackboard,
                  const AbsCoord &robotPos,
                  int icpResult,
                  const AbsCoord &icpObs,
                  const AbsCoord &ballRRC = AbsCoord(NAN,NAN,NAN),
                  const AbsCoord &teamBall = AbsCoord(NAN,NAN,NAN));
      
   private:
      QImage image;
      QPixmap imagePixmap;
      QPixmap *renderPixmap;
};

