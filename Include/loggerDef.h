#ifndef LOGGER_H
#define LOGGER_H

#include <windows.h>
#include <iostream>

namespace Logger {
  #define RED_FOREGROUND     12
  #define GREEN_FOREGROUND   10
  #define WHITE_FOREGROUND   15
  #define DEFAULT_FOREGROUND 15

  static HANDLE hConsole;

  void Setup();
  void SetColor(WORD);

  template <class T>
  void Log(T);

  template <class T, class... Args>
  void Log(T, Args...);

  template <class T>
  void LogNB(T);

  template <class T, class... Args>
  void LogNB(T, Args...);

  template <class... Args>
  void LogColor(WORD, Args...);

  template <class... Args>
  void LogColorNB(WORD, Args...);

  template <class T, class... Args>
  void LogError(T, Args...);
};

#endif
