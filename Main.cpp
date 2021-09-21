#include "Game.h"
#include <cstdlib>

int main(int argc, char *argv[]) {
  Game game;
  bool success = game.Initialize(atoi(argv[1]));
  if (success) {
    game.RunLoop();
  }

  game.Shutdown();
  return 0;
}
