#include <logger.h>

void Logger::SetColor(WORD color) {
  SetConsoleTextAttribute(hConsole, color);
}

void Logger::Setup() {
  Log("Setting up Logger");

  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}
