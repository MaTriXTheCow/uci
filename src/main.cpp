#include <windows.h>

#include <logger.h>

#include <uci.h>
#include <util.h>

/*
* Entry function for the application
* creates a UCI object and runs.
*/
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*prevInstance*/, PWSTR pCmdLine, int nCmdShow) {
  Logger::Setup();

  UCI uci(hInstance, pCmdLine, nCmdShow);

  uci.Run();
}
