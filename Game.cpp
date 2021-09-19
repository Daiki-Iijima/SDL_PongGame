#include "Game.h"
#include <sdl2/2.0.16/include/SDL2/SDL_log.h>
#include <sdl2/2.0.16/include/SDL2/SDL_render.h>
#include <sdl2/2.0.16/include/SDL2/SDL_timer.h>

const int thickness = 15;

Game::Game()
    : mWindow(nullptr), mIsRunning(true), mRenderer(nullptr), mTicksCount(0) {}

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
                             SDL_WINDOWPOS_CENTERED, // 画面のy座標の中心
                             640,                    //  幅
                             480,                    //  高さ
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
}

void Game::UpdateGame() {

  //  フレームレート制限を設ける
  //  左の引数が、右の引数を超えていればtrueを返す
  //  1fが0.016ms(60FPS)より早かった場合、0.16を超えるまで待つ
  float frame = (1.0 / 30.0) * 1000;
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + frame))
    ;

  float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

  //  デルタタイムが大きくなりすぎることを防ぐ
  //  1fが0.05ms(20FPS)より遅かった場合、下限を20FPSにする
  if (deltaTime > 0.05f) {
    deltaTime = 0.05f;
  }

  mTicksCount = SDL_GetTicks();

  float fps = 1 / deltaTime;

  SDL_Log("deltaTime:%f", deltaTime);
  SDL_Log("fps:%f", fps);
}

void Game::GenerateOutput() {
  //  === バックバッファに描画 ===
  //  色の設定
  SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
  //  バックバッファを塗りつぶす
  SDL_RenderClear(mRenderer);

  //  壁(四角)を描画する
  //  色の設定
  SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

  //  長方形のRect構造体を生成
  SDL_Rect wall{
      0,        // 左上隅からのx
      0,        // 左上隅からのy
      1024,     // 幅
      thickness // 高さ
  };

  //  中を塗りつぶした長方形を描画する
  SDL_RenderFillRect(mRenderer, &wall);

  //  フロントバッファとバックバッファの入れ替え
  //  すべての描画処理が終わって最後に処理する
  SDL_RenderPresent(mRenderer);
}
