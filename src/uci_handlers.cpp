#include <uci.h>
#include <functional>

void UCI::SetGameLoopCallback(std::function<void()> f) {
  winProc.SetGameLoopCallback(f);
}

void UCI::SetHandlerFunction(HandlerFunctions handlerEnum, std::function<void()> f) {
  winProc.SetHandlerFunction(handlerEnum, [=, this]() {f(); });
}
