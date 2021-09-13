#include <logger.h>

#include <iostream>
#include <windows.h>
#include <type_traits>



void Logger::SetColor(WORD color) {
  SetConsoleTextAttribute(hConsole, color);
}

template <class T>
void Logger::Log(T t) {
  LogNB(t, '\n');
}

template <class T, class... Args>
void Logger::Log(T t, Args... args) {
  LogNB(t, args..., '\n');
}

template <class T>
void Logger::LogNB(T t) {
  std::cout << t;
}

template <class T, class... Args>
void Logger::LogNB(T t, Args... args) {
  std::cout << t << ' ';
  LogNB(args...);
}

template <class... Args>
void Logger::LogColor(WORD textColor, Args... args) {
  SetColor(textColor);
  Log(args...);
  SetColor(DEFAULT_FOREGROUND);
}

template <class... Args>
void Logger::LogColorNB(WORD textColor, Args... args) {
  SetColor(textColor);
  LogNB(args...);
  SetColor(DEFAULT_FOREGROUND);
}


template <class T, class... Args>
void Logger::LogError(T t, Args... args) {
  LogColorNB(RED_FOREGROUND, "ERROR: ");
  Log(t, args...);
}

void Logger::Setup() {
  Log("Setting up Logger");

  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  return;
}

int Logger::LogExit(int pExit) {
  WORD textColor = (pExit == 0) ? GREEN_FOREGROUND : RED_FOREGROUND;
  std::string successFailText = (pExit == 0) ? "Program Succeeded" : "Program failed";

  LogColor(textColor, successFailText);

  return pExit;
}
