#include "include/utility.h"
#include "update.h"
#include <windows.h>

// extern "C" BOOL WINAPI DllMain(HINSTANCE const instance, DWORD const reason,
// LPVOID const reserved)
//{
//
//	switch (reason)
//	{
//	case DLL_PROCESS_ATTACH:
//		break;
//
//	case DLL_THREAD_ATTACH:
//		break;
//
//	case DLL_THREAD_DETACH:
//		break;
//
//	case DLL_PROCESS_DETACH:
//		break;
//	}
//
//	return TRUE;
// }

int main() {
  common::utility::initLogger("./logs", "liveup");
  common::utility::log(common::utility::LOGLEVEL::info, false,
                       "========== start ==========");

  auto update = std::make_shared<Update>();
  update->update();

  common::utility::log(common::utility::LOGLEVEL::info, false,
                       "========== stop ==========");
  common::utility::closeLogger();
  return 0;
}
