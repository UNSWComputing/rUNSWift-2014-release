#pragma once

#include <qwt_plot.h>
#include <vector>
#include "types/SensorValues.hpp"
#include "types/Odometry.hpp"
#include "types/XYZ_Coord.hpp"

// PLOT_SIZE ~= 30s at 30fps or 9s at 100fps
#define PLOT_SIZE 301
#define SURF_PLOT_SIZE 20

class DataPlot : public QwtPlot {
   Q_OBJECT

   public:
      DataPlot(QWidget* = NULL);
      // put one frame in front of 
      virtual void push(SensorValues sensors, bool left=false) = 0;
      void push(std::vector<SensorValues> sensors, bool left=false);

   protected:
      void alignScales();
      double t[PLOT_SIZE];
};

class XYZPlot : public QwtPlot {
   Q_OBJECT

   public:
      XYZPlot(QWidget* = NULL);
      // put one frame in front of 
      virtual void push(XYZ_Coord coord, bool left=false) = 0;
      void push(std::vector<XYZ_Coord> coords, bool left=false);

   protected:
      void alignScales();
      double t[PLOT_SIZE];
};

class AccPlot : public DataPlot {
   Q_OBJECT

   public:
      AccPlot(QWidget* = NULL);
      virtual void push(SensorValues sensors, bool left=false);
   private:
      double data_x[PLOT_SIZE];
      double data_y[PLOT_SIZE];
      double data_z[PLOT_SIZE];
};
class FsrPlot : public DataPlot {
   Q_OBJECT

   public:
      FsrPlot(QWidget* = NULL);
      virtual void push(SensorValues sensors, bool left=false);
   private:
      double data_l[PLOT_SIZE];
      double data_r[PLOT_SIZE];
      double data_r2[PLOT_SIZE];
      double data_t[PLOT_SIZE];
};
class TiltPlot : public DataPlot {
   Q_OBJECT

   public:
      TiltPlot(QWidget* = NULL);
      virtual void push(SensorValues sensors, bool left=false);
   private:
      double data_x[PLOT_SIZE];
      double data_y[PLOT_SIZE];
      double data_y2[PLOT_SIZE];
};
class SonarPlot : public DataPlot {
   Q_OBJECT

   public:
      SonarPlot(QWidget* = NULL);
      virtual void push(SensorValues sensors, bool left=false);
   private:
      double data[10][PLOT_SIZE];
};
class OdomPlot : public DataPlot {
   Q_OBJECT

   public:
      OdomPlot(QWidget* = NULL);
      virtual void push(SensorValues sensors, bool left=false);
      virtual void push(Odometry vision, Odometry motion, Odometry combined, bool left=false);
      virtual void push(std::vector<Odometry> vision, std::vector<Odometry> motion, 
            std::vector<Odometry> combined, bool left=false);
   private:
      double visionData[PLOT_SIZE];
      double motionData[PLOT_SIZE];
      double combinedData[PLOT_SIZE];
      double visionDiff[PLOT_SIZE-1];
      double motionDiff[PLOT_SIZE-1];
      double combinedDiff[PLOT_SIZE-1];
};
class ChargePlot : public DataPlot {
   Q_OBJECT

   public:
      ChargePlot(QWidget* = NULL);
      virtual void push(SensorValues sensors, bool left=false);
   private:
      double data[PLOT_SIZE];
      double data2[PLOT_SIZE];
};
class CurrentPlot : public DataPlot {
   Q_OBJECT

   public:
      CurrentPlot(QWidget* = NULL);
      virtual void push(SensorValues sensors, bool left=false);
   private:
      double data[PLOT_SIZE];
      double data2[PLOT_SIZE];
};
class CoronalZMPPlot : public DataPlot {
   Q_OBJECT

   public:
      CoronalZMPPlot(QWidget* = NULL);
      virtual void push(SensorValues sensors, bool left=false);
   private:
      double data_l[PLOT_SIZE];
      double data_r[PLOT_SIZE];
      double data_t[PLOT_SIZE];
};
class SagittalZMPPlot : public DataPlot {
   Q_OBJECT

   public:
      SagittalZMPPlot(QWidget* = NULL);
      virtual void push(SensorValues sensors, bool left=false);
   private:
      double data[PLOT_SIZE];
};
class COMxPlot : public XYZPlot {
   Q_OBJECT

   public:
      COMxPlot(QWidget* = NULL);
      virtual void push(XYZ_Coord com, bool left=false);
   private:
      double data[PLOT_SIZE];
};
class COMyPlot : public XYZPlot {
   Q_OBJECT

   public:
      COMyPlot(QWidget* = NULL);
      virtual void push(XYZ_Coord com, bool left=false);
   private:
      double data[PLOT_SIZE];
};
class SurfPlot : public DataPlot {
   Q_OBJECT

   public:
      SurfPlot(QWidget* = NULL);
      virtual void push(std::vector<float> scores);
      virtual void push(SensorValues sensors, bool left=false);
   private:
      double data[SURF_PLOT_SIZE];
};
