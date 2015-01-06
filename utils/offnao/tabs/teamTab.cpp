#include <QMenu>
#include <QMenuBar>
#include <QDebug>
#include <QBitmap>

#include <iostream>
#include <time.h>
#include <utility>

#include "blackboard/Blackboard.hpp"
#include "teamTab.hpp"
#include "types/SPLStandardMessage.hpp"

#include <boost/asio/placeholders.hpp>

using namespace std;

TeamTab::TeamTab(QTabWidget *parent, QMenuBar *menuBar) :
   service(),
   socket(service,
          boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(),
          PORT)),
   team(18) {
   initMenu(menuBar);
   init();
   this->parent = parent;

   time_t currentTime = time(NULL);

   for (int i = 0; i < NUM_ROBOTS; ++i) {
      AbsCoord r (0, 3400, 0);
      r.var.setZero();
      robots.push_back(r);
      balls.push_back(r);
      timeOut.push_back(currentTime);
   }
   startListening();
   t = new boost::thread(boost::bind(&boost::asio::io_service::run, &service));
}

TeamTab::~TeamTab() {
   delete layout;
}

void TeamTab::initMenu(QMenuBar *) {
}

void TeamTab::init() {

   layout = new QGridLayout(this);
   setLayout(layout);
   layout->setAlignment(layout, Qt::AlignTop);
   layout->setHorizontalSpacing(5);
   teamNum = new QLineEdit("18");
   teamNum->setFixedWidth(100);
   teamUpdate = new QPushButton("Change Team");

   layout->addWidget(&fieldView, 0, 0);
   layout->addWidget(new QLabel ("Team: "), 0, 1);
   layout->addWidget(teamNum, 0, 2);
   layout->addWidget(teamUpdate, 0, 3);

   /* draw the field with nothing on it */
   fieldView.redraw(NULL);

   connect(this, SIGNAL(update()), this, SLOT(redraw()));
   connect(teamUpdate, SIGNAL(released()), this, SLOT(changeTeam()));
}

void TeamTab::redraw() {
   fieldView.redraw(robots, balls);
}

void TeamTab::changeTeam() {
   team = teamNum->text().toFloat();
}

void TeamTab::newNaoData(NaoData *naoData) {
   // doesn't do anything in this tab
}

void TeamTab::handleReceive(const boost::system::error_code &error,
                              std::size_t size) {
   time_t currentTime = time(NULL);

   const SPLStandardMessage &m = (const SPLStandardMessage &)recvBuffer;
   const BroadcastData &bd = (const BroadcastData &)m.data;
   if (size == sizeof(SPLStandardMessage) &&
       m.numOfDataBytes == sizeof(BroadcastData) &&
       bd.team == team) {
      robots[bd.playerNum-1] = bd.robotPos; // note the -1
      balls[bd.playerNum-1] = bd.ballPosAbs; // note the -1
      timeOut[bd.playerNum-1] = currentTime;
   }

   // Check timeout for idle robots
   double timer = currentTime - 5;
   for (int i = 0; i < NUM_ROBOTS; ++i) {
      if (timeOut[i] < timer) {
         AbsCoord r (0, 3400, 0);
         r.var.setZero();
         robots[i] = r;
         balls[i] = r;
      }
   }

   update();
   startListening();
}

void TeamTab::startListening() {
   //listen->start();
   socket.async_receive_from(
      boost::asio::buffer(boost::asio::mutable_buffer((void *)&recvBuffer,
                                                       sizeof(recvBuffer))),
      remoteEndpoint,
      boost::bind(&TeamTab::handleReceive, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));

}
