#include <uci.h>
#include <functional>

void UCI::SetGameLoopCallback(std::function<void()> f) {
  winProc.SetGameLoopCallback(f);
}

void UCI::SetHandlerFunction(HandlerFunctions handlerEnum, HandlerFunc f) {
  winProc.SetHandlerFunction(handlerEnum, f);
}
