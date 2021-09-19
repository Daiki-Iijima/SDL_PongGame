#include "Game.h"
#include <sdl2/2.0.16/include/SDL2/SDL_render.h>

Game::Game() : mWindow(nullptr), mIsRunning(true), mRenderer(nullptr) {}

bool Game::Initialize() {

  //  SDL初期化
  int sdlResult = SDL_Init(SDL_INIT_VIDEO);
  if (sdlResult != 0) {
    SDL_Log("SDLを初期化できませんでした:%s", SDL_GetError());
    return false;
  }

  //  Window作成
  mWindow = SDL_CreateWindow("Fist Window", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, 640, 480, 0);

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

void Game::UpdateGame() {}
void Game::GenerateOutput() {}
