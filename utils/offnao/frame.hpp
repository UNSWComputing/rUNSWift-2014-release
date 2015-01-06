#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <stdint.h> // cstdint is c++0x
#include <cstdio>
#include <ctime>
#include <QPoint>

#include <map>
#include <string>

#include "blackboard/Blackboard.hpp"
#include "externaldata/ExternalData.hpp"

/*
 * Here we store all the info we wish to receive from the nao
 */
class Frame {
   public:
      Blackboard *blackboard;
      time_t timestamp;
      std::map<std::string, boost::shared_ptr<ExternalData> > externalData;
      Frame() : blackboard(0) {
         timestamp = time(0);
      }
      ~Frame() {}

      friend class boost::serialization::access;
      /**
      * helper for serialization
      */
      template<class Archive>
      void shallowSerialize(Archive &ar, const unsigned int version);
      /**
      * serialises the blackboard for storing to a file or network
      */
      template<class Archive>
      void save(Archive &ar, const unsigned int version) const;
      /**
      * serialises the blackboard for loading from a file or network
      */
      template<class Archive>
      void load(Archive &ar, const unsigned int version);
      BOOST_SERIALIZATION_SPLIT_MEMBER();
      
};
#include "frame.tcc"
