#pragma once

#include "perception/vision/Camera.hpp"

class SimCamera : public Camera {
   public:
      explicit SimCamera();
      ~SimCamera();

      const uint8_t *get(const int colourSpace);
      bool setCamera(WhichCamera whichCamera);
      WhichCamera getCamera();
      bool setControl(const uint32_t id, const int32_t value);
};
