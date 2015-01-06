#include <boost/serialization/vector.hpp>

template<class Archive>
void NaoData::serialize(Archive &ar, const unsigned int file_version) {
   ar & frames;
   ar & currentFrame;
   ar & timeRecorded;
   ar & isPaused;
}
