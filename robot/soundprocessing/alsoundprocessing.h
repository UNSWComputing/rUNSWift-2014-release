/**
* @author Gwennael Gate
* Copyright (c) Aldebaran Robotics 2010, 2011
*/


#ifndef SOUNDPROCESSING_H
#define SOUNDPROCESSING_H
#include <string>
#include <rttools/rttime.h>

#include <boost/shared_ptr.hpp>
#include <alvalue/alvalue.h>
#include <alproxies/almemoryproxy.h>
#include <alaudio/alsoundextractor.h>

using namespace AL;

class ALSoundProcessing : public ALSoundExtractor
{

public:

  ALSoundProcessing(boost::shared_ptr<ALBroker> pBroker, std::string pName);
  virtual ~ALSoundProcessing();

  //method inherited from almodule that will be called after constructor
  void init();

public:
  void process(const int & nbOfChannels,
               const int & nbrOfSamplesByChannel,
               const AL_SOUND_FORMAT * buffer,
               const ALValue & timeStamp);

private:
  ALMemoryProxy fProxyToALMemory;
  std::vector<std::string> fALMemoryKeys;
};
#endif

