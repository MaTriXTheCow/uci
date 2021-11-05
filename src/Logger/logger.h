#include <windows.h>

#ifndef LOGGER_H
#define LOGGER_H

#define RED_FOREGROUND 12
#define GREEN_FOREGROUND 10
#define WHITE_FOREGROUND 15
#define DEFAULT_FOREGROUND 15

class Logger {
private:
  inline static HANDLE hConsole;
public:
  static void Setup();
  static int LogExit(int);
  static void SetColor(WORD);

  template <class T>
  static void Log(T);

  template <class T, class... Args>
  static void Log(T, Args...);

  template <class T>
  static void LogNB(T);

  template <class T, class... Args>
  static void LogNB(T, Args...);

  template <class... Args>
  static void LogColor(WORD, Args...);

  template <class... Args>
  static void LogColorNB(WORD, Args...);

  template <class T, class... Args>
  void Logger::LogError(T, Args...);
};

#endif
