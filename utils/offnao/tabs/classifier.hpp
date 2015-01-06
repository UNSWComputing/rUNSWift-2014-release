#pragma once

#include <stdio.h>
#include <stdint.h>
#include <QRgb>
#include <fstream>
#include <deque>
#include <iostream>
#include <vector>
#include <string>
#include "../../robot/perception/vision/VisionDefs.hpp"

static const int YMAX = 128;
static const int UMAX = 128;
static const int VMAX = 128;
static const int CMAX = cNUM_COLOURS;

static const int Y_KERNEL_RADIUS = 10;
static const int Y_KERNEL_RADIUS_SQUARED = Y_KERNEL_RADIUS * Y_KERNEL_RADIUS;
static const int Y_KERNEL_RADIUS_POW_4 = Y_KERNEL_RADIUS_SQUARED*
                                         Y_KERNEL_RADIUS_SQUARED;
static const double INV_Y_KERNEL_RADIUS_SQUARED = 1.0/
                              (Y_KERNEL_RADIUS_SQUARED);
static const double INV_Y_KERNEL_RADIUS_POW_4 = 1.0 / (Y_KERNEL_RADIUS_POW_4);


static const int U_KERNEL_RADIUS = 10;
static const int U_KERNEL_RADIUS_SQUARED = U_KERNEL_RADIUS * U_KERNEL_RADIUS;
static const double INV_U_KERNEL_RADIUS_SQUARED = 1.0 /
                              (U_KERNEL_RADIUS_SQUARED);

static const int V_KERNEL_RADIUS = 10;
static const int V_KERNEL_RADIUS_SQUARED = V_KERNEL_RADIUS * V_KERNEL_RADIUS;
static const double INV_V_KERNEL_RADIUS_SQUARED = 1.0/
                              (V_KERNEL_RADIUS_SQUARED);

static const float INITIAL_NONE_VALUE = 0.001f;

struct YuvTriple {
   unsigned char y, u, v;
};

struct Weights {
   float w[CMAX];
};

struct YuvGaussian {
   YuvGaussian() {
      yuv.y = 255;
      yuv.u = 255;
      yuv.v = 255;
      weight = 0;
      hacks = false;
      classified = cBACKGROUND;
   }

   YuvGaussian(YuvTriple yuv, double weight, Colour classified, int yRadius,
               int uRadius, int vRadius, bool hacks) {
      this->yuv = yuv;

      this->weight = weight;
      this->yRadius = yRadius;
      this->uRadius = uRadius;
      this->vRadius = vRadius;
      this->classified = classified;
      this->hacks = hacks;
   }

   YuvTriple yuv;
   double weight;
   int yRadius, uRadius, vRadius;
   bool hacks;
   Colour classified;
};

// An action is a list of gaussians
struct Action {
   std::vector<YuvGaussian> gaussians;
   void clear(void) {
      gaussians.clear();
   }
};

// This class lets you make a classification file
class Classifier {
   public:

      /* Converts a YUV pixel spec to RGB */
      static QRgb yuv2rgb(int y, int u, int v);

      Classifier(void);
      ~Classifier(void);

      void newClassificationFile(void);
      void saveClassificationFile(std::string filename);
      bool loadClassificationFile(std::string filename);

      void beginAction(void);  // Wrap your Classify calls inside these
      void endAction(void);    // so undo will work.

      bool isMostlyClassified(int y, int u, int v, Colour c);
      void classify(int y, int u, int v, double weight, Colour c, int yRadius,
                    int uRadius, int vRadius, bool hacks);

      bool canUndo(void);
      void getUndoStatus(int& levels, Colour& lastColour);
      void undo(void);

      void saveNnmc(std::string filename);

      bool classificationFileOpened(void);

      Colour getClassifiedColour(int y, int u, int v);
      float getColourMargin(int y, int u, int v);
      void colourInfo(int y, int u, int v, float weights[CMAX]);
      float rawKernelVal(int dY, int dU, int dV, int yRadius, int uRadius,
            int vRadius, bool hacks);

      // The next few guys are for doing hacks with visualisation and
      // stuff like that
      unsigned char* getNnmcPointer(void);
      void setNnmcPointer(unsigned char* nnmc);
      void setUpdateLiveNnmc(bool);

   private:
      void applyAction(Action &a, bool undo);
      void applyGaussian(YuvGaussian&, bool undo);
      void addWeight(YuvTriple yuv, float amount, Colour c, bool undo);

      bool fileOpened;

      void clearFile(void);
      Colour maxWeightIndex(Weights&);
      void makeNnmc(void);
      void getWeights(YuvTriple, Weights&);
      void setWeights(YuvTriple, Weights&);
      int64_t yuvToFilePos(YuvTriple);
      std::deque<Action> undoBuffer;
      Action currentAction;

      unsigned char nnmc[VMAX][UMAX][YMAX];
      unsigned char* liveNnmc;
      bool useLiveNnmc;
      Weights weight[YMAX][UMAX][VMAX];
};

