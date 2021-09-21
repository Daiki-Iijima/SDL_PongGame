#include <sdl2/2.0.16/include/SDL2/SDL.h>

//  vec2の構造体
struct Vector2 {
  float x;
  float y;
};

class Game {
public:
  //  コンストラクタ
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

  //  デルタタイム計算用
  Uint32 mTicksCount;

  //  SDLが作成するウィンドウ
  SDL_Window *mWindow;
  //  ゲームの続行を指示する
  bool mIsRunning;
  //  SDLの描画で使用する
  SDL_Renderer *mRenderer;

  //  パドルの座標
  Vector2 mPaddlePos;
  //  パドルの移動ベクトル
  int mPaddleDir;
  //  ボールの座標
  Vector2 mBallPos;
  //  ボールの移動速度(Velocity)
  Vector2 mBallVel;
};
