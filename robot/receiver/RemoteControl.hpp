#pragma once

#include "Nao.hpp"
#include <string>
#include "types/BehaviourRequest.hpp"
#include "blackboard/Blackboard.hpp"
#include "blackboard/Adapter.hpp"

class RemoteControlReceiver : Adapter, NaoReceiver {
  public:
   /**
    * Constructor.  Opens a socket for listening.
    */
   RemoteControlReceiver(Blackboard *bb, void(RemoteControlReceiver::*handler)
	   (const boost::system::error_code & error, std::size_t) = &RemoteControlReceiver::receiveHandler);
   
   /**
    * One cycle of this thread
    */
   void tick();

  private:
   void receiveHandler(const boost::system::error_code &error, std::size_t size);
};