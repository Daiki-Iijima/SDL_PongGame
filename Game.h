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
  bool Initialize(int userCount);

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

  //  User1パドルの座標
  Vector2 mPaddlePos1;
  //  User1パドルの移動ベクトル
  int mPaddleDir1;
  //  User2パドルの座標
  Vector2 mPaddlePos2;
  //  User2パドルの移動ベクトル
  int mPaddleDir2;

  //  ボールの座標
  Vector2 mBallPos;
  //  ボールの移動速度(Velocity)
  Vector2 mBallVel;

  //  プレイヤーの数
  int mUserCount;
};
