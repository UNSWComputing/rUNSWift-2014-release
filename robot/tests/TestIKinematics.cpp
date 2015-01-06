#include <motion/IKinematics.hpp>
#include <Eigen/Eigen>

using namespace IKinematics;
using namespace Eigen;

NaoFootTarget fKinematics(const NaoLegChain &chain)
{
   double a;

   a = chain.hip;
   AngleAxis<double> Rh  = AngleAxis<double>(a, Vector3d(0, 1, 1).normalized());

   a = chain.thighRoll;
   AngleAxis<double> Rtr = AngleAxis<double>(a, Vector3d(1, 0, 0));

   a = chain.thighPitch;
   AngleAxis<double> Rtp = AngleAxis<double>(a, Vector3d(0, 1, 0));

   a = chain.kneePitch;
   AngleAxis<double> Rkp = AngleAxis<double>(a, Vector3d(0, 1, 0));

   a = chain.ankleRoll;
   AngleAxis<double> Rar = AngleAxis<double>(a, Vector3d(1, 0, 0));

   a = chain.anklePitch;
   AngleAxis<double> Rap = AngleAxis<double>(a, Vector3d(0, 1, 0));

   Translation3d Tt(0, 0, -0.10000);
   Translation3d Ts(0, 0, -0.10290);
   Translation3d Tf(0, 0, -0.04519);

   Matrix4d m = (Rh * Rtr * Rtp * Tt * Rkp * Ts * Rap * Rar * Tf).matrix();

   NaoFootTarget r;
   r.x     = m(0,3);
   r.y     = m(1,3);
   r.z     = m(2,3);

   r.roll  = chain.thighRoll + chain.ankleRoll;
   r.pitch = -(chain.thighPitch + chain.kneePitch + chain.anklePitch);
   r.yaw   = chain.hip;

   return r;

}

int main(int argc, char **argv)
{
   NaoFootTarget target;

   target.roll = target.pitch = target.yaw = 0.f;

   target.roll  = 0.0;
   target.pitch = 0.0;
   target.yaw   = 0.0;

   const float m = 0.038;

   int errors = 0;
   int runs = 0;
   /* Test by drawing a cube with the foot */
   for (target.yaw = -0.2; target.yaw < M_PI_2 / 1.5; target.yaw += 0.1) {
      for (target.x = -0.2; target.x < 0.2; target.x += 0.01) {
         for (target.y = -0.2; target.y < 0.2; target.y += 0.01) {
            for (target.z = -0.22; target.z < -.10; target.z += 0.1) {
               if (target.x*target.x + target.y*target.y + target.z*target.z > m) {
                  continue;
               }
               ++ runs;

               NaoLegChain   chain   = NaoSolve(target);
               NaoFootTarget forward = fKinematics(chain);

               if ((fabsf(forward.x     - target.x    ) < 0.02) &&
                   (fabsf(forward.y     - target.y    ) < 0.02) &&
                   (fabsf(forward.z     - target.z    ) < 0.02) &&
                   (fabsf(forward.pitch - target.pitch) < 0.02) &&
                   (fabsf(forward.yaw   - target.yaw  ) < 0.02) &&
                   (fabsf(forward.roll  - target.roll ) < 0.02))
               {
                   continue;
               }

               std::cout << "=== ERROR ===" << std::endl;
               std::cout << chain;
               std::cout << target;
               std::cout << fKinematics(chain);

               ++ errors;
            }
         }
      }
   }

   std::cout << errors << " errors from " << runs << " runs." << std::endl;
}
