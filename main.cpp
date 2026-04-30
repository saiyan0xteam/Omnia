#include "Class/CGame.h"

int main() {
#ifdef _DEBUG
  Logger::Initialize("Omnia Debug Console");
#endif

  CGame game;
  game.Init();
  game.Run();
  game.Shutdown();

  return 0;
}