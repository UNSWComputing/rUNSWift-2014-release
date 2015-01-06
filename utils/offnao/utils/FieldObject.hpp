#pragma once

#include "types/FieldFeatureInfo.hpp"
#include "types/PostInfo.hpp"
#include "types/AbsCoord.hpp"

struct FieldObject {
   enum {
      POST = 0,
      TWOPOSTS,
      FEATURE
   } type;

   union {
      PostInfo::Type postType;
      FieldFeatureInfo::Type featureType;
   } subType;

   AbsCoord pos;
};
