/// <summary>
/// This file has been generated automatically through the module generator provided in the sdk.
///  This file handle the creation of the dynamic library (local use) or the executable (remote use).
/// </summary>
/// <remarks> Copyright 2010 A. Dupont </remarks>


#include <signal.h>
#include <boost/shared_ptr.hpp>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>

// include the modules
#include "alsoundprocessing.h"

#ifdef SOUNDPROCESSING_IS_REMOTE
# define ALCALL
#else
# ifdef _WIN32
#  define ALCALL __declspec(dllexport)
# else
#  define ALCALL
# endif
#endif

extern "C"
{

  ALCALL int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
  {
    // init broker with the main broker instance
    // from the parent executable
    AL::ALBrokerManager::setInstance(pBroker->fBrokerManager.lock());
    AL::ALBrokerManager::getInstance()->addBroker(pBroker);

    // create module instances
    AL::ALModule::createModule<ALSoundProcessing>(pBroker,"ALSoundProcessing");
    return 0;
  }

  ALCALL int _closeModule()
  {
    return 0;
  }

} // extern "C"


#ifdef SOUNDPROCESSING_IS_REMOTE

int main(int argc, char *argv[] )
{
  // pointer to createModule
  TMainType sig;
  sig = &_createModule;

  // call main
  ALTools::mainFunction("alsoundprocessing", argc, argv, sig);
}

#endif

