#include "Nao.hpp"

using namespace boost::asio;
using namespace std;

NaoReceiver::~NaoReceiver() {
   service.stop();
   t->join();
   delete t;
   if (socket.is_open()) {
      socket.close();
   }
}
