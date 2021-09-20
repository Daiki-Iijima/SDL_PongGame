#include "Game.h"
#include <sdl2/2.0.16/include/SDL2/SDL_log.h>
#include <sdl2/2.0.16/include/SDL2/SDL_render.h>
#include <sdl2/2.0.16/include/SDL2/SDL_scancode.h>
#include <sdl2/2.0.16/include/SDL2/SDL_timer.h>

const int thickness = 15;
const float paddleHeight = 100.0f;

Game::Game()
    : mWindow(nullptr), mIsRunning(true), mRenderer(nullptr), mTicksCount(0),
      mPaddleDir(0) {}

bool Game::Initialize() {

  //  SDL初期化
  int sdlResult = SDL_Init(SDL_INIT_VIDEO);
  if (sdlResult != 0) {
    SDL_Log("SDLを初期化できませんでした:%s", SDL_GetError());
    return false;
  }

  //  Window作成
  mWindow = SDL_CreateWindow("Fist Window",          //  タイトル
                             SDL_WINDOWPOS_CENTERED, //  画面のx座標の中心
                             SDL_WINDOWPOS_CENTERED, //  画面のy座標の中心
                             1024,                   //  幅
                             768,                    //  高さ
                             0                       //  フラグ
  );

  if (!mWindow) {
    SDL_Log("ウィンドウの初期化に失敗しました:%s", SDL_GetError());
    return false;
  }

  //  レンダラーの作成
  mRenderer = SDL_CreateRenderer(
      mWindow,
      -1, //  複数のウィンドウがある場合に-1以外の数字を入れる時がある
      SDL_RENDERER_ACCELERATED |
          SDL_RENDERER_PRESENTVSYNC); // GPUを使用する |
                                      // vsync(垂直同期)をONにする

  if (!mRenderer) {
    SDL_Log("レンダラーの初期化に失敗しました:%s", SDL_GetError());
    return false;
  }

  //  パドルとボールの座標の初期化
  mPaddlePos.x = 0.0f;
  mPaddlePos.y = 768.0f / 2.0f;
  mBallPos.x = 1024.0f / 2.0f;
  mBallPos.y = 768.0f / 2.0f;

  return true;
}

void Game::Shutdown() {
  SDL_DestroyWindow(mWindow);
  SDL_DestroyRenderer(mRenderer);
  SDL_Quit();
}

void Game::RunLoop() {
  while (mIsRunning) {
    ProcessInput();
    UpdateGame();
    GenerateOutput();
  }
}

void Game::ProcessInput() {
  //  OSからのイベントの処理を行う
  //  このキューの処理をしないとウィンドウが表示されない
  SDL_Event ev;
  while (SDL_PollEvent(&ev)) {
    switch (ev.type) {
      //  ユーザーがウィンドウを閉じようとしたとき
    case SDL_QUIT:
      mIsRunning = false;
      break;
    }
  }

  //  Escキーでウィンドウを閉じる
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_ESCAPE]) {
    mIsRunning = false;
  }

  mPaddleDir = 0;
  //  キー入力
  if (state[SDL_SCANCODE_S]) {
    mPaddleDir -= 1;
  }
  if (state[SDL_SCANCODE_W]) {
    mPaddleDir += 1;
  }
}

void Game::UpdateGame() {

  //  フレームレート制限を設ける
  //  左の引数が、右の引数を超えていればtrueを返す
  //  1fが0.016ms(60FPS)より早かった場合、0.16を超えるまで待つ
  float frame = (1.0 / 60.0) * 1000;
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + frame))
    ;

  float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

  //  デルタタイムが大きくなりすぎることを防ぐ
  //  1fが0.05ms(20FPS)より遅かった場合、下限を20FPSにする
  if (deltaTime > 0.05f) {
    deltaTime = 0.05f;
  }

  mTicksCount = SDL_GetTicks();

  // float fps = 1 / deltaTime;
  // SDL_Log("deltaTime:%f", deltaTime);
  // SDL_Log("fps:%f", fps);

  //  パドルの移動
  mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;

  //  パドルの移動制限
  //  パドルとボールは原点が中心座標
  //  thicknessを含めているのは、壁を加味するため
  if (mPaddleDir == -1 && mPaddlePos.y < paddleHeight / 2 + thickness) {
    mPaddlePos.y = paddleHeight / 2 + thickness;
  }
  if (mPaddleDir == 1 && mPaddlePos.y > 768 - (paddleHeight / 2) - thickness) {
    mPaddlePos.y = 768 - (paddleHeight / 2) - thickness;
  }
}

void Game::GenerateOutput() {
  //  === バックバッファに描画 ===
  //  色の設定
  SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
  //  バックバッファを塗りつぶす
  SDL_RenderClear(mRenderer);

  //  壁(四角)を描画する
  //  色の設定(白)
  SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

  //  上の壁を生成
  //  長方形のRect構造体を生成
  SDL_Rect wall{
      0,        // 左上隅からのx
      0,        // 左上隅からのy
      1024,     // 幅
      thickness // 高さ
  };

  //  中を塗りつぶした長方形を描画する
  SDL_RenderFillRect(mRenderer, &wall);

  //  下の壁を生成
  wall.y = 768 - thickness;
  SDL_RenderFillRect(mRenderer, &wall);

  //  右の壁を生成
  wall.x = 1024 - thickness;
  wall.y = 0;
  wall.w = thickness;
  wall.h = 768;
  SDL_RenderFillRect(mRenderer, &wall);

  //  パドルの生成
  SDL_Rect paddle{
      static_cast<int>(mPaddlePos.x),                    // x
      static_cast<int>(mPaddlePos.y - paddleHeight / 2), // y
      thickness,                                         // w
      static_cast<int>(paddleHeight)                     // h
  };

  SDL_RenderFillRect(mRenderer, &paddle);

  //  ボールの生成
  SDL_Rect ball{
      static_cast<int>(mBallPos.x - thickness / 2), // x
      static_cast<int>(mBallPos.y - thickness / 2), // y
      thickness,                                    // w
      thickness                                     // h
  };

  SDL_RenderFillRect(mRenderer, &ball);

  //  フロントバッファとバックバッファの入れ替え
  //  すべての描画処理が終わって最後に処理する
  SDL_RenderPresent(mRenderer);
}
