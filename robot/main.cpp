#include <boost/program_options.hpp>
#include <signal.h>
#include <errno.h>
#include <vector>
#include <map>
#include <iostream>
#include <string>

#include "soccer.hpp"

#include "utils/NaoVersion.hpp"
#include "utils/Logger.hpp"
#include "utils/options.hpp"

#include "perception/behaviour/python/RegisterConverters.hpp"

#include "thread/ThreadManager.hpp"
#include "motion/MotionAdapter.hpp"
#include "transmitter/OffNao.hpp"
#include "transmitter/Team.hpp"
#include "receiver/Team.hpp"
#include "receiver/RemoteControl.hpp"
#include "gamecontroller/GameController.hpp"
#include "perception/PerceptionThread.hpp"
#include "perception/vision/Vision.hpp"
#include "perception/vision/NaoCamera.hpp"
#include "perception/vision/NaoCameraV4.hpp"
#include "perception/PerceptionThread.hpp"

#define handle_error_en(en, msg) \
   do { \
      errno = en; \
      perror(msg); \
      exit(1); \
   } while (0)

namespace po = boost::program_options;
using namespace std;
using namespace boost;

extern string git_version;

/**
 * A timer function.  After arg seconds, runswift will shut down.
 *
 * @param arg coerced to int.  seconds to shutdown
 * @returns NULL
 */
static void *shutdownTimer(void * arg) {
   int time = (int)arg;
   if (time > 5) {
      sleep(time - 5);
      SAY("shutting down");
      sleep(5);
   } else {
      sleep(time);
   }
   attemptingShutdown = true;
   return NULL;
}

/** Entry point for runswift application */
int main(int argc, char **argv) {

   po::variables_map vm;
   try {
      po::options_description generic("Generic options");
      generic.add_options()
         ("help,h", "produce help message")
         ("version,v", "print version string");

      po::options_description cmdline_options =
         store_and_notify(argc, argv, vm, &generic);

      if (vm.count("help")) {
         cout << cmdline_options << endl;
         return 1;
      }

      if (vm.count("version")) {
         cout << "rUNSWift Nao soccer player " << git_version << endl;
         return 1;
      }

      cout << "rUNSWift V." << git_version << endl;

      options_print(vm);
   } catch (program_options::error& e) {
      cerr << "Error when parsing command line arguments: " << e.what() << endl;
      return 1;
   } catch (std::exception& e) {
      cerr << e.what() << endl;
      return 1;
   }

   offNao = false;
   attemptingShutdown = false;
   Thread::name = "main";

   // Generate date string in yyyy-mm-dd-hh-mm-ss format
   time_t s_since_epoch = time(NULL);
   const struct tm *now = localtime(&s_since_epoch);
   char timestr[20];
   strftime(timestr, 20, "%Y-%m-%d-%H-%M-%S", now);

   Logger::init(vm["debug.logpath"].as<string>() + string("/") + string(timestr),
                vm["debug.log"].as<string>(),
                vm["debug.log.motion"].as<bool>());

   /* Determine the version of the robot before doing anything else.
    * Other initialisation routines depend on the value of naoVersion
    */
   determineNaoVersion();


   Blackboard *blackboard = new Blackboard(vm);

   if (vm["debug.vision"].as<bool>()) {
      if (naoVersion >= nao_v4) {
         llog(INFO) << "Initialising v4 /dev/video0" << std::endl;
         Vision::top_camera = new NaoCameraV4(blackboard, "/dev/video0");

         llog(INFO) << "Initialising v4 /dev/video1" << std::endl;
         Vision::bot_camera = new NaoCameraV4(blackboard, "/dev/video1",
                                              IO_METHOD_MMAP,
                                              AL::kVGA);

         Vision::camera = Vision::bot_camera;
      } else {
         llog(INFO) << "Initialising v3 /dev/video0" << std::endl;
         printf("Initialising v3 /dev/video0\n");
         Vision::camera = new NaoCamera(blackboard, "/dev/video0");
      }
   }

   llog(INFO) << "RUNSWIFT soccer library spinning up!" << endl;

   registerSignalHandlers();

   // create thread managers
   ThreadManager perception("Perception", 0); // as fast as possible, waits on camera read
   ThreadManager motion("Motion", 0); // as fast as possible, waits on agent semaphore
   ThreadManager gameController("GameController", 0); // as fast as possible, waits on udp read
   ThreadManager offnaoTransmitter("OffnaoTransmitter", 50000); // 20fps limit
   ThreadManager teamTransmitter("TeamTransmitter", 200000); // 5fps limit
   ThreadManager teamReceiver("TeamReceiver", 200000); // 5fps limit (to calculate incapacitation)
   //ThreadManager remoteControlReceiver("RemoteControlReceiver", 200000); // 5 fps limit for remote-control updates

   // start threads
   if (vm["debug.perception"].as<bool>()) {
      perception.run<PerceptionThread>(blackboard);
      llog(INFO) << "Perception is running" << endl;
   }
   if (vm["debug.motion"].as<bool>()) {
      motion.run<MotionAdapter>(blackboard);
      llog(INFO) << "Motion is running" << endl;
   }
   if (vm["debug.gamecontroller"].as<bool>()) {
      gameController.run<GameController>(blackboard);
      llog(INFO) << "GameController is running" << endl;
   }
   if (vm["debug.offnaotransmitter"].as<bool>()) {
      offnaoTransmitter.run<OffNaoTransmitter>(blackboard);
      llog(INFO) << "Off-Nao Transmitter is running" << endl;
   }
   if (vm["debug.naotransmitter"].as<bool>()) {
      teamTransmitter.run<TeamTransmitter>(blackboard);
      llog(INFO) << "Nao Transmitter is running" << endl;
   }
   if (vm["debug.naoreceiver"].as<bool>()) {
      teamReceiver.run<TeamReceiver>(blackboard);
      llog(INFO) << "Team Receiver is running" << endl;
   }
//   if (vm["debug.remotecontrol"].as<bool>()) {
//      //pthread_create(&remotecontrol, NULL, &safelyRun<RemoteControlReceiver>,
//      //               NULL);
//      remoteControlReceiver.run<RemoteControlReceiver>(blackboard);         
//      llog(INFO) << "Remote Control Receiver is running" << endl;
//   }

   if (vm["debug.shutdowntime"].as<int>()) {
      pthread_t timer;
      pthread_create(&timer, NULL, &shutdownTimer,
                     (void*)vm["debug.shutdowntime"].as<int>());
      llog(INFO) << "Timer is running" << endl;
   }

   teamReceiver.join();
   teamTransmitter.join();
   offnaoTransmitter.join();
   gameController.join();
   motion.join();
   perception.join();

   delete blackboard;

   if (naoVersion >= nao_v4) {
      delete Vision::top_camera;
      delete Vision::bot_camera;
   } else {
      delete Vision::camera;
   }

   return 0;
}

