#include "Game.h"

Game::Game() : mWindow(nullptr), mIsRunning(true) {}

bool Game::Initialize() {
  int sdlResult = SDL_Init(SDL_INIT_VIDEO);
  if (sdlResult != 0) {
    SDL_Log("SDLを初期化できませんでした:%s", SDL_GetError());
    return false;
  }

  mWindow = SDL_CreateWindow("Fist Window", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, 640, 480, 0);

  if (!mWindow) {
    SDL_Log("ウィンドウの初期化に失敗しました:%s", SDL_GetError());
    return false;
  }

  return true;
}

void Game::Shutdown() {
  SDL_DestroyWindow(mWindow);
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
}
void Game::UpdateGame() {}
void Game::GenerateOutput() {}
