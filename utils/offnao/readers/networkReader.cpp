#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QInputDialog>
#include <cmath>
#include <string>
#include <sstream>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/shared_ptr.hpp>

#include "readers/networkReader.hpp"
#include "perception/vision/rle.hpp"
#include "utils/Timer.hpp"
#include "progopts.hpp"
#include "blackboard/Blackboard.hpp"
#include "types/AbsCoord.hpp"
#include "thread/Thread.hpp"

using namespace std;
using namespace boost;

NetworkReader::NetworkReader(const QString &robotName, OffNaoMask_t mask) :
   connection_(0), received(), isRecording(false), robotName(robotName) {
   this->mask = mask;
   this->naoData.setPaused(false);
   isAlive = true;
}

NetworkReader::NetworkReader(pair<const QString &, OffNaoMask_t> robotNameMask):
   connection_(0), received(), isRecording(false),
   robotName(robotNameMask.first) {
   this->mask = robotNameMask.second;
   this->naoData.setPaused(false);
   isAlive = true;
}

NetworkReader::NetworkReader(pair<const QString &, OffNaoMask_t> robotNameMask,
                             const NaoData &naoData) :
   Reader(naoData), connection_(0), received(), isRecording(false),
   robotName(robotNameMask.first) {
   this->mask = robotNameMask.second;
   this->naoData.setPaused(false);
   isAlive = true;
}

NetworkReader::~NetworkReader() {
   if (isRecording) disconnect();
   isAlive = false;
}

/// Handle completion of a connect operation.
void NetworkReader::handle_connect(const boost::system::error_code& e,
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
   qDebug("Connected!");
   if (Thread::name == NULL) {
      Thread::name = "NetworkReader";
   }

   if (!e) {
      /* Successfully established connection. Start operation to read the list
       * of Blackboards. The connection::async_read() function will
       * automatically decode the data that is read from the underlying socket.
       */
      received.blackboard = new Blackboard(config);
      connection_->async_read(*received.blackboard,
            boost::bind(&NetworkReader::handle_read, this,
               boost::asio::placeholders::error));

      // if (!(rand() % 10))
      write(mask);
      emit showMessage(QString("Connected! Now streaming"));
   } else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator()) {
      // Try the next endpoint.
      connection_->socket().close();
      boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
      connection_->socket().async_connect(endpoint,
            boost::bind(&NetworkReader::handle_connect, this,
               boost::asio::placeholders::error, ++endpoint_iterator));
   } else {
      /* An error occurred. Log it and return. Since we are not starting a new
       * operation the io_serviceasync_read will run out of work to do and the
       * wirelessClient will exit.
       */
      std::cerr << e.message() << std::endl;
      emit showMessage(QString::fromStdString(e.message()));
   }
}

/// Handle completion of a read operation.
void NetworkReader::handle_read(const boost::system::error_code& e) {
   if (Thread::name == NULL) {
      Thread::name = "NetworkReader";
   }
   if (!isAlive) {
      return;  // this class has already been destroyed.
   }
   if (!e) {
      static int counter = 0;
      static Timer t;
      emit showMessage(QString("average ms per packets: ") +
            QString::number(t.elapsed_ms()/++counter));

      try{
         naoData.appendFrame(received);
         if(!naoData.getIsPaused()) {
            naoData.setCurrentFrame(naoData.getFramesTotal()-1);
         }
         // TODO(jayen): breaks over midnight
         static uint64_t lastnew = 0;
         struct timeval now;
         gettimeofday(&now, NULL);
         uint64_t now2 = now.tv_sec * 1000000ull + now.tv_usec;
         if (now2 >= lastnew + 250000) {
            emit newNaoData(&naoData);
            lastnew = now2;
         }
         received.blackboard = new Blackboard(config);
         connection_->async_read(*received.blackboard,
                                 boost::bind(&NetworkReader::handle_read, this,
                                             boost::asio::placeholders::error));
      } catch(boost::system::system_error &se) {
         qDebug() << "Error in receiving wireless data. " <<
               se.what() << endl;
         emit showMessage(se.what());
         // disconnect();
      }
   } else {
      qDebug("Error in receiving wireless data. Disconnected");
      std::cout << "Problem is " << e.message() << std::endl;
      emit showMessage("Error in data transmission. Disconnected...");
   }
}

void NetworkReader::run()  {
   if (Thread::name == NULL) {
      Thread::name = "NetworkReader";
   }
   Frame frame;
   int currentFrame = 0;
   emit showMessage(
        tr("Started session with nao. Hit record to begin stream..."));

   while (isAlive) {
//       if(!isRecording) {
//          // TODO(brockw): make this work with the camera tab
//          if (!naoData.getIsPaused() && naoData.getCurrentFrameIndex() <
//               naoData.getFramesTotal() - 1) {
//             naoData.nextFrame();
//             emit newNaoData(&naoData);
//          } else if (naoData.getFramesTotal() != 0) {
//             emit newNaoData(&naoData);
//          }
//       }
      currentFrame = naoData.getCurrentFrameIndex();
      if (currentFrame != naoData.getFramesTotal() - 1) {
         msleep(250);
      } else {
         msleep(200);  // this may be different soon. Hence the condition
      }
   }
   emit newNaoData(NULL);
}


   void NetworkReader::stopMediaTrigger() {
      if (isRecording)
         isRecording = disconnect();
      naoData.setPaused(true);
      emit showMessage(QString("Disconnected. Hit record to continue."));
   }

   void NetworkReader::recordMediaTrigger() {
      if (!isRecording) {
         isRecording = connect();
      }
      naoData.setPaused(false);
   }

   void NetworkReader::sendCommandLineString(QString item) {
      std::cout << item.toStdString() << std::endl;
      OffNaoMask_t sendingMask = COMMAND_MASK;
      if (connection_ && connection_->socket().is_open()) {
         boost::asio::write(connection_->socket(),
                            boost::asio::buffer(&sendingMask,
                                                sizeof(sendingMask)));
         connection_->sync_write(item.toStdString());
      }
   }

   bool NetworkReader::disconnect() {
      std::cerr << "Try to disconnect!!" << std::endl;
      try {
         if (ioservice)
            ioservice->stop();

         if (cthread) {
            cthread->join();
            delete cthread;
            cthread = NULL;
         }

         if (connection_ && connection_->socket().is_open())
            connection_->socket().close();

         if (connection_) {
            delete connection_;
            connection_ = NULL;
         }
         if (ioservice) {
            delete ioservice;
            ioservice = NULL;
         }
         if (resolver) {
            delete resolver;
            resolver = NULL;
         }
         if (query) {
            delete query;
            query = NULL;
         }
      } catch(boost::system::system_error &se) {
         emit showMessage(QString("Could not disconnect to robot!"));
      }

      return false;
   }

   bool NetworkReader::connect() {
      std::cerr << "Try to connect!" << std::endl;
      try {
         ioservice = new boost::asio::io_service();
         connection_ = new Connection(ioservice);
         resolver = new boost::asio::ip::tcp::resolver(*ioservice);
         query = new boost::asio::ip::tcp::resolver::query(robotName.toStdString()
               , "10125");
         boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
            resolver->resolve(*query);
         boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

         connection_->socket().async_connect(endpoint,
               boost::bind(&NetworkReader::handle_connect, this,
                  boost::asio::placeholders::error, ++endpoint_iterator));
         cthread = new boost::thread(boost::bind(&boost::asio::io_service::run,
                  ioservice));
         std::cerr <<"Connected!" << std::endl;
      } catch(boost::system::system_error &se) {
         emit showMessage(QString("Could not connect to robot!"));
         return false;
      }
      return true;
   }

   void NetworkReader::write(const msg_t &msg) {
      if (connection_ && connection_->socket().is_open())
         ioservice->post(boost::bind(&NetworkReader::do_write, this, msg));
   }

   void NetworkReader::do_write(msg_t msg) {
      bool write_in_progress = !write_msgs_.empty();
      write_msgs_.push_back(msg);
      if (!write_in_progress) {
         boost::asio::async_write(connection_->socket(),
               boost::asio::buffer(&(write_msgs_.front()),
                  sizeof(write_msgs_.front())),
               boost::bind(&NetworkReader::handle_write, this,
                  boost::asio::placeholders::error));
      }
   }

   void NetworkReader::handle_write(const boost::system::error_code &error) {
      if (!error) {
         if (!write_msgs_.empty()) {
            boost::asio::async_write(connection_->socket(),
                  boost::asio::buffer(&(write_msgs_.front()),
                     sizeof(write_msgs_.front())),
                  boost::bind(&NetworkReader::handle_write, this,
                     boost::asio::placeholders::error));
            write_msgs_.pop_front();
         }
      } else {
         do_close();
      }
   }

   void NetworkReader::do_close() {
      connection_->socket().close();
   }
