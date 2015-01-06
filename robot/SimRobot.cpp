#include <boost/program_options.hpp>
#include <vector>
#include <string>
#include "soccer.hpp"
#include "SimRobot.hpp"
#include "utils/Logger.hpp"
#include "utils/options.hpp"
#include "thread/ThreadManager.hpp"

#include "transmitter/OffNao.hpp"
#include "blackboard/Blackboard.hpp"
#include "motion/MotionAdapter.hpp"
#include "perception/PerceptionThread.hpp"
#include "perception/vision/Vision.hpp"
#include "perception/vision/SimCamera.hpp"

namespace po = boost::program_options;
using namespace std;

SimRobot::SimRobot() : offNaoInUse(false) {
   Vision::camera = new SimCamera();

   vector<string> str;
   po::variables_map vm;
   po::options_description cmdline_options = store_and_notify(str, vm);
   options_print(vm);
   Logger::init(vm["debug.logpath"].as<string>(),
                vm["debug.log"].as<string>(),
                vm["debug.log.motion"].as<bool>());

   // Initialize oracle for translation between robot and simulation
   simOracle = new Oracle();
   simOracle->init();
   oracle = simOracle;

   // Initialize blackboard for the sim robot
   simBlackboard = new Blackboard(vm);

   // Initialize motion adapter for the sim robot
   motionAdapter = new MotionAdapter(simBlackboard);

   // Initialize perception for the sim robot
   perceptionThread = new PerceptionThread(simBlackboard);

   offNaoInUse = vm["debug.offnaotransmitter"].as<bool>();

   if (offNaoInUse) {
      pthread_create(&offnaotransmitter, NULL, &safelyRun<OffNaoTransmitter>,
                     NULL);
      llog(INFO) << "Off-Nao Transmitter is running" << endl;
   }
}

SimRobot::~SimRobot() {
   attemptingShutdown = true;
   if (offNaoInUse) {
      pthread_join(offnaotransmitter, NULL);
      llog(INFO) << "Off-Nao Transmitter thread joined" << endl;
   }

   delete simOracle;
   delete simBlackboard;
   delete motionAdapter;
   delete perceptionThread;
}

void SimRobot::run() {
   oracle = simOracle;

   motionAdapter->tick();
   perceptionThread->tick();
}
