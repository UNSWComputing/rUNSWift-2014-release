#include "tab.hpp"

#include "utils/OverlayPainter.hpp"
#include "perception/vision/Vision.hpp"
#include "classifier.hpp"

using namespace std;

QRgb Tab::getRGB(unsigned int col,
                 unsigned int row,
                 const uint8_t *yuv,
                 int num_cols) {
   return Classifier::yuv2rgb(gety(yuv, row, col, num_cols),
                              getu(yuv, row, col, num_cols),
                              getv(yuv, row, col, num_cols));
}

void Tab::drawOverlaysGeneric (QPaintDevice *topImage,
      QPaintDevice                        *botImage,
      const std::pair<int, int>           *horizon,
      const std::vector<FootInfo>         *feet,            
      const std::vector<BallInfo>         *balls,
      const std::vector<PostInfo>         *posts,
      const std::vector<RobotInfo>        *robots,
      const std::vector<FieldEdgeInfo>    *fieldEdges,
      const std::vector<FieldFeatureInfo> *fieldFeatures,
      const std::vector<Ipoint>           *landmarks,
      float scale)
{
   OverlayPainter topPainter;
   OverlayPainter botPainter;
   
   if (topImage) {
      topPainter.begin(topImage);
      topPainter.scale (scale, scale);
   }
   
   if (botImage){      
      botPainter.begin(botImage);
      botPainter.scale (scale, scale);
      botPainter.translate(0, -TOP_IMAGE_ROWS);
   }

   if (topImage && horizon) {
      topPainter.drawHorizon(*horizon);
   }

   if (botImage && feet) {
      std::vector<FootInfo>::const_iterator i;
      for (i = feet->begin (); i != feet->end (); ++ i) {
         botPainter.drawFootOverlay (*i);
      }
   }
 
   if (balls) {
      std::vector<BallInfo>::const_iterator i;
      for (i = balls->begin (); i != balls->end (); ++ i) {
         if (topImage) topPainter.drawBallOverlay (*i);
         if (botImage) botPainter.drawBallOverlay (*i);
      }
   }

   if (posts) {
      std::vector<PostInfo>::const_iterator i;
      for (i = posts->begin (); i != posts->end (); ++ i) {
         if (topImage) topPainter.drawPostOverlay (*i);
         if (botImage) botPainter.drawPostOverlay (*i);
      }
   }

   if (robots) {
      std::vector<RobotInfo>::const_iterator i;
      for (i = robots->begin (); i != robots->end (); ++ i) {
         if (topImage) topPainter.drawRobotOverlay (*i);
         if (botImage) botPainter.drawRobotOverlay (*i);
      }
   }

   if (fieldEdges) {
      std::vector<FieldEdgeInfo>::const_iterator i;
      for (i = fieldEdges->begin (); i != fieldEdges->end (); ++ i) {
         if (topImage) topPainter.drawFieldEdgeOverlay (*i);
         if (botImage) botPainter.drawFieldEdgeOverlay (*i);
      }
   }

   if (fieldFeatures) {
      std::vector<FieldFeatureInfo>::const_iterator i;
      for (i = fieldFeatures->begin (); i != fieldFeatures->end (); ++ i) {
         if (topImage) topPainter.drawFieldFeatureOverlay (*i);
         if (botImage) botPainter.drawFieldFeatureOverlay (*i);
      }
   }

   if (landmarks && topImage) {
      std::vector<Ipoint>::const_iterator i;
      for (i = landmarks->begin (); i != landmarks->end (); ++ i) {
         topPainter.drawLandmarkOverlay (*i, *horizon);
      }
   }

   if (topImage) topPainter.end();
   if (botImage) botPainter.end();
}

void Tab::tabSelected()
{
}

void Tab::tabDeselected()
{
}

#if 0
      vector<std::pair<uint16_t, uint16_t> > *edgePoints,
      FieldEdge edgeLines[2],
      uint8_t numEdgeLines,
      uint16_t *goals,
      uint16_t numPosts,
      WhichPosts posts,
      ImageRegion *regions,
      uint16_t numRegions,
      uint16_t radius,
      std::pair<uint16_t, uint16_t> ballCentre,
      std::pair<uint16_t, uint16_t> *ballEdgePoints,
      uint16_t numBallEdgePoints,
      RobotRegion **robotRegions,
      uint16_t numRobotRegions,
      ImageRegion **lineRegions,
      uint16_t numLineRegions,
      uint16_t numFeet,
      uint16_t *footImageCoords,
      float scale) {
   QPainter painter(image);
   if (edgePoints != 0) {
      painter.setPen(QColor(1, 0, 0));
      painter.setBrush(QBrush(QColor(255, 255, 0)));
      vector<pair<uint16_t, uint16_t> >::const_iterator p;
      for (p = edgePoints->begin(); p != edgePoints->end(); ++p) {
         painter.drawEllipse(QPoint((*p).first*scale, (*p).second)*scale, 2, 2);
      }
   }

   if (edgeLines != 0) {
      painter.setPen(QColor(255, 255, 0));
      for (int i = 0; i < numEdgeLines; ++i) {
         painter.drawLine(
               0, -1*(float)edgeLines[i].t3/edgeLines[i].t2 * scale,
               (IMAGE_COLS-1)*scale,
               scale*((-1*(float)edgeLines[i].t3
                - ((float)edgeLines[i].t1)*(IMAGE_COLS-1))
               /edgeLines[i].t2));
               painter.setPen(QColor(255, 0, 0));
      }
   }

   if (numPosts != 0 && goals != 0) {
      if (posts >= p_legacy_YELLOW_LEFT) {
         painter.setPen(QColor(255, 255, 0));
      } else {
         painter.setPen(QColor(0, 255, 255));
      }
      for (uint16_t p = 0; p < numPosts*4; p += 4) {
         QLineF lines[4] = {
            QLineF(scale*goals[p], scale*goals[p+1], scale*goals[p+2],
                   scale*goals[p+1]),
            QLineF(scale*goals[p+2], scale*goals[p+1], scale*goals[p+2],
                   scale*goals[p+3]),
            QLineF(scale*goals[p+2], scale*goals[p+3], scale*goals[p],
                   scale*goals[p+3]),
            QLineF(scale*goals[p], scale*goals[p+3], scale*goals[p],
                   scale*goals[p+1])
         };
         painter.drawLines(lines, 4);
      }
   }

   if (radius != 0) {
      painter.setPen(QColor(0, 0, 0));
      painter.setBrush(QBrush(Qt::NoBrush));
      painter.drawEllipse(QPoint(scale * ballCentre.first,
              scale * ballCentre.second),
              (int)(scale * radius), (int)(scale * radius));
      painter.setPen(QColor(255, 255, 255));
      int i;
      for (i = 0; i < numBallEdgePoints; i++) {
         painter.drawEllipse(QPoint(ballEdgePoints[i].first,
                  ballEdgePoints[i].second), 1, 1);
      }
      painter.setPen(QColor(0,255,0));
      /*if (numBallEdgePoints != 0) {
         for (; i < numBallEdgePoints + NUM_CENTRE_REPEATS; i++) {
            painter.drawEllipse(QPoint(ballEdgePoints[i].first,
                     ballEdgePoints[i].second), 1, 1);
         }
      }*/
      /*painter.drawEllipse(QPoint(scale*ballCentre.first,
                          scale*ballCentre.second),
                          (int)(scale*radius), (int)(scale*radius));*/
   }

   if (numFeet > 0){
      painter.setPen(QColor(0, 0, 255));
      painter.setBrush(QBrush(Qt::NoBrush));
      painter.drawEllipse(QPoint(scale * footImageCoords[0],
              scale * footImageCoords[1]),
              (int)(scale * FOOT_RADIUS), (int)(scale * FOOT_RADIUS));
      if (numFeet > 1){
         painter.drawEllipse(QPoint(scale * footImageCoords[2],
              scale * footImageCoords[3]),
              (int)(scale * FOOT_RADIUS), (int)(scale * FOOT_RADIUS));
      }
   }

   if (numRobotRegions != 0) {
      painter.setPen(QColor(255, 255, 255));
      for (uint16_t p = 0; p < numRobotRegions; p++) {

         if (robotRegions[p]->type == RED_ROBOT) {
            painter.setPen(QColor(255, 0, 0));
         } else if (robotRegions[p]->type == BLUE_ROBOT) {
            painter.setPen(QColor(0, 0, 255));
         } else {
            painter.setPen(QColor(0, 50, 0));
         }

         QLineF lines[4] = {
            QLineF(robotRegions[p]->leftMost * SALIENCY_DENSITY * scale, 
                  robotRegions[p]->bottomMost * SALIENCY_DENSITY * scale, 
                  robotRegions[p]->leftMost * SALIENCY_DENSITY * scale,
                  robotRegions[p]->topMost * SALIENCY_DENSITY * scale),
            QLineF(robotRegions[p]->leftMost * SALIENCY_DENSITY * scale,
                  robotRegions[p]->topMost * SALIENCY_DENSITY * scale,
                  robotRegions[p]->rightMost * SALIENCY_DENSITY * scale,
                  robotRegions[p]->topMost * SALIENCY_DENSITY * scale),
            QLineF(robotRegions[p]->rightMost * SALIENCY_DENSITY * scale,
                  robotRegions[p]->topMost * SALIENCY_DENSITY * scale,
                  robotRegions[p]->rightMost * SALIENCY_DENSITY * scale,
                  robotRegions[p]->bottomMost * SALIENCY_DENSITY * scale),
            QLineF(robotRegions[p]->leftMost * SALIENCY_DENSITY * scale,
                  robotRegions[p]->bottomMost * SALIENCY_DENSITY * scale,
                  robotRegions[p]->rightMost * SALIENCY_DENSITY * scale,
                  robotRegions[p]->bottomMost * SALIENCY_DENSITY * scale)
            };
            painter.drawLines(lines, 4);
      }
   }

   if (numLineRegions != 0 && numRegions == 0) {
      painter.setPen(QColor(255, 0, 255));
      for (uint16_t p = 0; p < numLineRegions; p++) {
         for (uint16_t i = 0; i < lineRegions[p]->numScanPoints; i++) {
            painter.drawEllipse(
                 lineRegions[p]->startScans[i].first * SALIENCY_DENSITY,
                  lineRegions[p]->startScans[i].second * SALIENCY_DENSITY,
                  2, 2
                  );
            painter.drawEllipse(
                  lineRegions[p]->endScans[i].first * SALIENCY_DENSITY,
                  lineRegions[p]->endScans[i].second * SALIENCY_DENSITY,
                  2, 2
                  );
         }
      }
   }

   if (numRegions != 0) {
      for (uint16_t p = 0; p < numRegions; p++) {
         RegionType status = regions[p].classification;
         if (status == rBALL) {
            painter.setPen(QColor(128, 20, 20));
         } else if (status == rROBOT) {
            painter.setPen(QColor(255, 255, 255));
         } else if (status == rMAYBE_ROBOT) {
            painter.setPen(QColor(255, 255, 0));
         } else if (status == rFIELD_LINE) {
            painter.setPen(QColor(255, 0, 255));
         } else if (status == rCONNECTED_ROBOT) {
            painter.setPen(QColor(0, 255, 0));
         } else {
            painter.setPen(QColor(0, 255, 255));
         }
         if (!(regions[p].deleted)) {
            /*painter.drawEllipse(
                  regions[p].centreGravity.first * SALIENCY_DENSITY,
                  regions[p].centreGravity.second * SALIENCY_DENSITY,
                  2, 2
                  );*/
            // Draw the field lines
            // if (status == rFIELD_LINE) {
               for (uint16_t i = 0; i < regions[p].numScanPoints; i++) {
                  painter.drawEllipse(
                        regions[p].startScans[i].first * SALIENCY_DENSITY,
                        regions[p].startScans[i].second * SALIENCY_DENSITY,
                        2, 2
                        );
                  painter.drawEllipse(
                        regions[p].endScans[i].first * SALIENCY_DENSITY,
                        regions[p].endScans[i].second * SALIENCY_DENSITY,
                        2, 2
                        );
               }
            // }

            QLineF lines[4] = {
               QLineF(regions[p].leftMost * SALIENCY_DENSITY, 
                     regions[p].bottomMost.second * SALIENCY_DENSITY, 
                     regions[p].leftMost * SALIENCY_DENSITY,
                     regions[p].topMost.second * SALIENCY_DENSITY),
               QLineF(regions[p].leftMost * SALIENCY_DENSITY,
                     regions[p].topMost.second * SALIENCY_DENSITY,
                     regions[p].rightMost * SALIENCY_DENSITY,
                     regions[p].topMost.second * SALIENCY_DENSITY),
               QLineF(regions[p].rightMost * SALIENCY_DENSITY,
                     regions[p].topMost.second * SALIENCY_DENSITY,
                     regions[p].rightMost * SALIENCY_DENSITY,
                     regions[p].bottomMost.second * SALIENCY_DENSITY),
               QLineF(regions[p].leftMost * SALIENCY_DENSITY,
                     regions[p].bottomMost.second * SALIENCY_DENSITY,
                     regions[p].rightMost * SALIENCY_DENSITY,
                     regions[p].bottomMost.second * SALIENCY_DENSITY)
            };
            painter.drawLines(lines, 4);
         }
      }
   }
}
#endif

