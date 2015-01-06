#pragma once

#include "Oracle.h"

class Blackboard;
class MotionAdapter;
class PerceptionThread;

class SimRobot {
   public:
      explicit SimRobot();
      ~SimRobot();

      void run();

   private:
      bool offNaoInUse;
      pthread_t offnaotransmitter;

      Oracle *simOracle;
      Blackboard *simBlackboard;
      MotionAdapter *motionAdapter;
      PerceptionThread *perceptionThread;
};

