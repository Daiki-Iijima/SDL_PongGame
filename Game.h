#include <sdl2/2.0.16/include/SDL2/SDL.h>

class Game {
public:
  Game();

  //  ゲームの初期化
  bool Initialize();

  //  ゲームオーバーまでのループ
  void RunLoop();

  //  ゲームをシャットダウンする
  void Shutdown();

private:
  //  RunLoopのヘルパー関数群
  void ProcessInput();
  void UpdateGame();
  void GenerateOutput();

  //  SDLが作成するウィンドウ
  SDL_Window *mWindow;
  //  ゲームの続行を指示する
  bool mIsRunning;
  //  SDLの描画で使用する
  SDL_Renderer *mRenderer;
};
