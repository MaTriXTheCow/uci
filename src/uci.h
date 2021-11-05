#include <windows.h>

#ifndef UCI_H
#define UCI_H

class UCI {
public:
  UCI(HINSTANCE, PWSTR, int);

  int Run();
};

#endif
