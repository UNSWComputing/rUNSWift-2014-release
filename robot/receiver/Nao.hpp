#pragma once

#include <boost/system/error_code.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/thread.hpp>

class NaoReceiver {
   protected:
      /**
       * Constructor.  Opens a socket for listening.
       */
      template <class SubClass, typename ReadHandler>
      NaoReceiver(SubClass *scthis, ReadHandler handler, int port);

      /**
       * Destructor. Closes the socket.
       */
      virtual ~NaoReceiver();

      template <class SubClass, typename ReadHandler>
      void startReceive(SubClass *scthis, ReadHandler handler);
      char recvBuffer[1500]; // 1500 is the generally accepted maximum transmission unit on ethernet

   private:
      boost::asio::io_service service;
      boost::asio::ip::udp::socket socket;
      boost::asio::ip::udp::endpoint remoteEndpoint;
      boost::thread *t;
};

#include "Nao.tcc"
