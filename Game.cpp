#include "Game.h"
#include <cstdio>
#include <sdl2/2.0.16/include/SDL2/SDL_log.h>
#include <sdl2/2.0.16/include/SDL2/SDL_render.h>
#include <sdl2/2.0.16/include/SDL2/SDL_scancode.h>
#include <sdl2/2.0.16/include/SDL2/SDL_timer.h>

const int thickness = 15;
const float paddleHeight = 100.0f;

Game::Game()
    : mWindow(nullptr), mRenderer(nullptr), mTicksCount(0), mIsRunning(true),
      mPaddleDir1(0), mPaddleDir2(0), mUserCount(1) {}

bool Game::Initialize(int userCount) {

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

  //  ユーザー数のストア
  mUserCount = userCount;

  //  パドルとボールの座標の初期化
  mPaddlePos1.x = 0.0f;
  mPaddlePos1.y = 768.0f / 2.0f;
  mPaddlePos2.x = 1024.0f - thickness;
  mPaddlePos2.y = 768.0f / 2.0f;

  mBallPos.x = 1024.0f / 2.0f;
  mBallPos.y = 768.0f / 2.0f;
  mBallVel.x = -200.0f;
  mBallVel.y = -235.0f;

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

  mPaddleDir1 = 0;
  //  キー入力
  if (state[SDL_SCANCODE_S]) {
    mPaddleDir1 += 1;
  }
  if (state[SDL_SCANCODE_W]) {
    mPaddleDir1 -= 1;
  }

  //  ユーザー2
  if (mUserCount == 2) {
    mPaddleDir2 = 0;
    //  キー入力
    if (state[SDL_SCANCODE_K]) {
      mPaddleDir2 += 1;
    }
    if (state[SDL_SCANCODE_I]) {
      mPaddleDir2 -= 1;
    }
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

  //  === パドルの移動 ===
  //  パドル1の移動
  mPaddlePos1.y += mPaddleDir1 * 300.0f * deltaTime;

  //  パドルの移動制限
  //  パドルとボールは原点が中心座標
  //  thicknessを含めているのは、壁を加味するため
  if (mPaddleDir1 == -1 && mPaddlePos1.y < paddleHeight / 2 + thickness) {
    mPaddlePos1.y = paddleHeight / 2 + thickness;
  }
  if (mPaddleDir1 == 1 &&
      mPaddlePos1.y > 768 - (paddleHeight / 2) - thickness) {
    mPaddlePos1.y = 768 - (paddleHeight / 2) - thickness;
  }

  //  パドル2の移動
  if (mUserCount == 2) {
    mPaddlePos2.y += mPaddleDir2 * 300.0f * deltaTime;

    //  パドルの移動制限
    //  パドルとボールは原点が中心座標
    //  thicknessを含めているのは、壁を加味するため
    if (mPaddleDir2 == -1 && mPaddlePos2.y < paddleHeight / 2 + thickness) {
      mPaddlePos2.y = paddleHeight / 2 + thickness;
    }
    if (mPaddleDir2 == 1 &&
        mPaddlePos2.y > 768 - (paddleHeight / 2) - thickness) {
      mPaddlePos2.y = 768 - (paddleHeight / 2) - thickness;
    }
  }

  //  ボールの移動
  mBallPos.x += mBallVel.x * deltaTime;
  mBallPos.y += mBallVel.y * deltaTime;

  //  ボールのあたり判定
  //  上の壁
  if (mBallPos.y <= thickness && mBallVel.y < 0.0f) {
    mBallVel.y *= -1;
  }
  //  下の壁
  if (mBallPos.y >= 768 - thickness && mBallVel.y > 0.0f) {
    mBallVel.y *= -1;
  }

  if (mUserCount == 1) {
    //  右の壁
    if (mBallPos.x > 1024.0 - thickness && mBallVel.x > 0.0f) {
      mBallVel.x *= -1;
    }
  }

  if (mUserCount == 2) {
    //  右の壁
    if (mBallPos.x > 1024.0 - thickness && mBallVel.x > 0.0f) {
      SDL_Log("GameOver");
      mBallPos.x = 1024.0f / 2.0f;
      mBallPos.y = 768.0f / 2.0f;
    }
  }

  //  左の端から出た場合
  if (mBallPos.x < 0 && mBallVel.x < 0.0f) {
    SDL_Log("GameOver");
    mBallPos.x = 1024.0f / 2.0f;
    mBallPos.y = 768.0f / 2.0f;
  }

  //  パドルとのあたり判定
  //  ユーザー1
  //  中心の距離を求める
  float diffY = mPaddlePos1.y - mBallPos.y;
  //  絶対値を求める
  diffY = diffY > 0.0f ? diffY : -diffY;

  if (diffY <= paddleHeight / 2 && // パドルを半分にした長さよりも短い範囲
      mBallPos.x <= mPaddlePos1.x + thickness && //  X座標がパドル以下
      mBallVel.x <= 0.0f)                        //  向きがマイナス
  {
    mBallVel.x *= -1;
  }

  //  ユーザー2
  if (mUserCount == 2) {
    //  中心の距離を求める
    float diffY = mPaddlePos2.y - mBallPos.y;
    //  絶対値を求める
    diffY = diffY > 0.0f ? diffY : -diffY;

    if (diffY <= paddleHeight / 2 && // パドルを半分にした長さよりも短い範囲
        mBallPos.x >= mPaddlePos2.x - thickness / 2 && //  X座標がパドル以下
        mBallVel.x >= 0.0f)                            //  向きがマイナス
    {
      mBallVel.x *= -1;
    }
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

  //  ユーザーが1人だった場合
  if (mUserCount == 1) {
    //  右の壁を生成
    wall.x = 1024 - thickness;
    wall.y = 0;
    wall.w = thickness;
    wall.h = 768;
    SDL_RenderFillRect(mRenderer, &wall);
  } else if (mUserCount == 2) { //  ユーザーが2人だった場合
    //  右側にパドルを生成
    SDL_Rect paddle{
        static_cast<int>(mPaddlePos2.x),                    // x
        static_cast<int>(mPaddlePos2.y - paddleHeight / 2), // y
        thickness,                                          // w
        static_cast<int>(paddleHeight)                      // h
    };

    SDL_RenderFillRect(mRenderer, &paddle);
  }

  //  パドルの生成
  SDL_Rect paddle{
      static_cast<int>(mPaddlePos1.x),                    // x
      static_cast<int>(mPaddlePos1.y - paddleHeight / 2), // y
      thickness,                                          // w
      static_cast<int>(paddleHeight)                      // h
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
