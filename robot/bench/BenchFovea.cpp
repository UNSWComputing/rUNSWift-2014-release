#include <string.h>

#include <iostream>

#include "tests/TestFoveaData.hpp"
#include "utils/Timer.hpp"
#include "perception/vision/Fovea.hpp"

int main()
{
{
   FoveaT<hGoals, eGrey> fovea(BBox(Point(0,0), Point(20, 30)), 1);
   memcpy(fovea._grey, random_in, sizeof(random_in));

   Timer timer;

   int i;
   for (i = 0; i < 100000; ++ i) { 
      fovea.blurGrey();
   }

   unsigned int us = timer.elapsed_us();

   std::cout << "Grey took " << us << "us" << std::endl;
}

{
   FoveaT<hGoals, eGrey> fovea(BBox(Point(0,0), Point(20, 30)), 1);
   memcpy(fovea._grey, random_in, sizeof(random_in));
   fovea.blurGrey();

   Timer timer;

   int i;
   for (i = 0; i < 100000; ++ i) { 
      fovea.makeEdge();
   }

   unsigned int us = timer.elapsed_us();

   std::cout << "Edge took " << us << "us" << std::endl;
}

   return 0;
}

