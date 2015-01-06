#pragma once

#include <QObject>
#include <QWidget>
#include <QMenuBar>
#include <QString>
#include <QRgb>
#include <QPaintDevice>
#include <QTabWidget>

#include <vector>
#include <utility>

#include "perception/vision/yuv.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "utils/Logger.hpp"

#include "types/FootInfo.hpp"
#include "types/BallInfo.hpp"
#include "types/PostInfo.hpp"
#include "types/Ipoint.hpp"

#include "naoData.hpp"

class Vision;

class Tab : public QWidget {
   Q_OBJECT

   public:
      Tab() : vision(0), currentFrame(0) {}
      Tab(QTabWidget *parent) : QWidget(parent), parent(parent), vision(0),
                                topFrame(0), botFrame(0) {}

   protected:
      /* Returns the RGB value of the pixel at row, col
       * @arg: num_cols is used to vary the image resolution
       */
      virtual QRgb getRGB(unsigned int col, unsigned int row,
                          const uint8_t *yuv, int num_cols);

      QTabWidget *parent;

      /*  vision module from libsoccer */
      Vision *vision;

      /* Current working image
       * If you working with vision you need a frame.
       */
      const uint8_t *currentFrame;
      const uint8_t *topFrame;
      const uint8_t *botFrame;

      /*
       * Generic draw overlays function
       * Supply Null to any of the arguments if you do not wish to draw
       * a particular overlay. topImage is always required.
       */
      void drawOverlaysGeneric (QPaintDevice *topImage,
            QPaintDevice                        *botImage,
            const std::pair<int, int>           *horizon,
            const std::vector<FootInfo>         *feet,            
            const std::vector<BallInfo>         *balls,
            const std::vector<PostInfo>         *posts,
            const std::vector<RobotInfo>        *robots,
            const std::vector<FieldEdgeInfo>    *fieldEdges,
            const std::vector<FieldFeatureInfo> *fieldFeatures,
            const std::vector<Ipoint>           *landmarks,
            float scale
      );

      virtual void tabSelected();
      virtual void tabDeselected();

   signals:
      virtual void showMessage(const QString &, int timeout = 0);

   public slots:
      virtual void newNaoData(NaoData *naoData) = 0;
      virtual void readerClosed() {}

   friend class Visualiser;
};
