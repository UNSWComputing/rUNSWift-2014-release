#pragma once

#include "Nao.hpp"
#include <string>
#include "types/BroadcastData.hpp"
#include "blackboard/Blackboard.hpp"
#include "blackboard/Adapter.hpp"

class TeamReceiver : Adapter, NaoReceiver {
   public:
      /**
       * Constructor.  Opens a socket for listening.
       */
      TeamReceiver(Blackboard *bb, void(TeamReceiver::*handler)
                   (const boost::system::error_code & error, std::size_t) =
                      &TeamReceiver::naoHandler);

      /**
       * One cycle of this thread
       */
      void tick();

   private:
      void naoHandler(const boost::system::error_code &error, std::size_t size);
      void stdoutHandler(const boost::system::error_code &error, std::size_t size);
};
