#include <windows.h>

#include <Board\board.h>

#ifndef UCI_H
#define UCI_H

class UCI {
private:
  Board board;

public:
  UCI(HINSTANCE, PWSTR, int);

  int Run();
};

#endif
