#pragma once

#include <QGLViewer/qglviewer.h>

class PointCloud : public QGLViewer
{
protected :
  virtual void init();
  virtual void draw();
  virtual QString helpString() const;
public:
   /**
    * a vector of points, where a point is its rgb value and coordinate
    * where 0.0 < rgb < 1.0 and 0.0 < coordinate < 1.0
    */
   std::vector<std::pair<qglviewer::Vec, qglviewer::Vec> > points;
};
