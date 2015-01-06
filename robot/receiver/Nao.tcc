#include "utils/Logger.hpp"
#include <boost/asio/placeholders.hpp>

template <class SubClass, typename ReadHandler>
NaoReceiver::NaoReceiver(SubClass *scthis, ReadHandler handler, int port)
   : service(), socket(service,
                       boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(),
                                                      port)) {
   llog(INFO) << "Nao Receiver constructed" << std::endl;
   startReceive(scthis, handler);
   t = new boost::thread(boost::bind(&boost::asio::io_service::run, &service));
   llog(INFO) << "Listening for data on port " << port << std::endl;
}

template <class SubClass, typename ReadHandler>
void NaoReceiver::startReceive(SubClass *scthis, ReadHandler handler) {
   socket.async_receive_from(
      boost::asio::buffer((boost::asio::mutable_buffer((void *)&recvBuffer,
                                                       sizeof(recvBuffer)))), remoteEndpoint,
      boost::bind(handler, scthis,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}
