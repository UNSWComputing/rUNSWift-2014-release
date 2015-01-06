#include <ctime>
#include "receiver/RemoteControl.hpp"
#include "utils/Logger.hpp"
#include "thread/Thread.hpp"

using namespace std;

/*RemoteControlReceiver::RemoteControlReceiver(Blackboard *bb, void(RemoteControlReceiver::*handler)
                           (const boost::system::error_code & error, std::size_t))
   : Adapter(bb), NaoReceiver(this, handler, (bb->config)["remotecontrol.port"].as<int>()) {
      llog(INFO) << "Started remote-control receiver.";
   }*/

RemoteControlReceiver::RemoteControlReceiver(Blackboard *bb, void(RemoteControlReceiver::*handler)
(const boost::system::error_code & error, std::size_t))
: Adapter(bb), NaoReceiver(this, handler, 4000) {
   llog(INFO) << "Started remote-control receiver." << std::endl;
}

void RemoteControlReceiver::receiveHandler
   (const boost::system::error_code &error,
   std::size_t size) {
      if (Thread::name == NULL) {
         Thread::name = "RemoteControlBoostThread";
      }
      
      llog(INFO) << "Remote Control: incoming message of size " << size << std::endl;
      
      if (size == sizeof(BehaviourRequest))
      {
         const BehaviourRequest &br = (const BehaviourRequest &)recvBuffer;
         writeTo(remoteControl, request, br);
         writeTo(remoteControl, time_received, time(NULL));
         llog(INFO) << "Remote Control: message written to blackboard." << std::endl;
      }
      else
      {
         writeTo(remoteControl, time_received, (time_t)0);
      }
      
      startReceive(this, &RemoteControlReceiver::receiveHandler);
}

void RemoteControlReceiver::tick() {
}

