#pragma once

/**
 * Classes deriving ExternalData are stored in a map which is associated
 * with each frame. Should be used for storing data that can be associated
 * with a frame but doesnt belong on the blackboard
 * See visionTestTab for example usage
 */

class ExternalData
{
   public:
      ExternalData() {};
      virtual ~ExternalData() {}


      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
      }
};
