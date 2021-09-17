#include <windows.h>

#include <Logger\logger.h>

#include <uci.h>

/*
* Entry function for the application
* creates a UCI object and runs.
*/
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PWSTR pCmdLine, int nCmdShow) {
  Logger::Setup();

  UCI uci(hInstance, pCmdLine, nCmdShow);

  uci.Run()
}
