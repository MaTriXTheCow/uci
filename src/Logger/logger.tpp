/*
* File for keeping template functions so they are defined in the
* header file. It is included in the bottom of the header file.
*/

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
