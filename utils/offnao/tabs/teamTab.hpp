#pragma once


#include <QColor>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QTabWidget>
#include <QThread>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QWidget>
#include <QVBoxLayout>

#include <cstdio>
#include <deque>

#include "blackboard/Blackboard.hpp"
#include "tabs/tab.hpp"
#include "utils/Logger.hpp"
#include "fieldView.hpp"
#include "mediaPanel.hpp"

#include <boost/system/error_code.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/thread.hpp>

#define PORT 11801
#define NUM_ROBOTS 5 

class Blackboard;

class TeamTab : public Tab {
   Q_OBJECT
   public:
      TeamTab(QTabWidget *parent, QMenuBar *menuBar);
      ~TeamTab();

      void startListening();
      void handleReceive(const boost::system::error_code &error,
                         std::size_t size);

      QPixmap *renderPixmap;
      QLabel *renderWidget;

      // Receiver buffer
      // 1500 is the generally accepted maximum transmission unit on ethernet
      char recvBuffer[1500];

      std::vector<AbsCoord> robots;
      std::vector<AbsCoord> balls;
      std::vector<double> timeOut;

   private:
      void init();
      void initMenu(QMenuBar *menuBar);

      QGridLayout *layout;
      QLineEdit *teamNum;
      QPushButton *teamUpdate;

      /* These variables are used to present the debug variables from the nao*/
      FieldView fieldView;

      // Boost socket stuff
      boost::asio::io_service service;
      boost::asio::ip::udp::socket socket;
      boost::asio::ip::udp::endpoint remoteEndpoint;
      boost::thread *t;

      // Team
      int team;

   public slots:
      void changeTeam();
      void newNaoData(NaoData *naoData);
      void redraw();

   signals:
      void update();
};
