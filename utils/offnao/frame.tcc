#include "qt_serialisation/boostSerializationQPoint.hpp"
#include "progopts.hpp"


template<class Archive>
void Frame::shallowSerialize(Archive &ar, const unsigned int file_version) {
   ar & timestamp;

   /* Add classes derived from ExternalData here */
   // e.g: ar.register_type(static_cast<VisionTestData *>(NULL));

   ar & externalData;
}

template<class Archive>
void Frame::save(Archive & ar, const unsigned int version) const {
   ar & *blackboard;
   ((Frame*)this)->shallowSerialize(ar, version);
}

template<class Archive>
void Frame::load(Archive & ar, const unsigned int version) {
   blackboard = new Blackboard(config);
   ar & *blackboard;
   shallowSerialize(ar, version);

}
