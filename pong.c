/*

  An implementation of PONG in SDL2
  Last modified 16th of April 2020
  Copyright (c) 2020 Arthur Ferreira

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

*/


#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

void drawDigit(SDL_Renderer *renderer, int digit, int col, int row, int pixSize){
  const int digits[10][20] = {{1,1,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,1,1},{0,1,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,1,1,1},{1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1},{1,1,1,1,0,0,0,1,0,1,1,1,0,0,0,1,1,1,1,1},{1,0,0,1,1,0,0,1,1,1,1,1,0,0,0,1,0,0,0,1},{1,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,1,1},{1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1},{1,1,1,1,0,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0},{1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,1,1},{1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1}};
  SDL_Rect rect = {0 , 0, pixSize, pixSize};
  for (int y = 0; y < 5; y++)
    for (int x = 0; x < 4; x++)
      if (digits[digit][y * 4 + x] == 1){
        rect.x = col + x * pixSize;
        rect.y = row + y * pixSize;
        SDL_RenderFillRect(renderer, &rect);
      }
}

int main(int argc, char **argv){
  int WIDTH = 600, HEIGHT = 400, SPEED, PZ;

  // making the frame rate 60 FPS
  Uint32 frameDelay = 17, frameStart, frameTime;

  // main variables of the game
  SDL_Rect canvas, dot, ball, paddleL, paddleR;
  float ballSpeedX, ballSpeedY;
  int paddleSpeed, scoreL, scoreR;

  // these bools store the state of the key pressed
  SDL_bool P1_UP=SDL_FALSE, P1_DW=SDL_FALSE, P2_UP=SDL_FALSE, P2_DW=SDL_FALSE ;

  // booleans controling the curse of the game
  SDL_bool isRunning = SDL_TRUE, NEW_GAME = SDL_TRUE;

  //  to manage the events (mainly key pressed and key releases)
  SDL_Event event;

  // randomize the random number generator
  srand(time(NULL));

  // init SDL, create a window and a renderer with ALPHA support
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_Window *window   = SDL_CreateWindow("pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  // sets the blend mode to... blend so that we can create nice gfx effects
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  // adds an application icon
  SDL_Surface *icon = SDL_LoadBMP("pongIcon256.bmp");
  SDL_SetWindowIcon(window, icon);

  // load the two sounds of the game
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
  Mix_Chunk *inEffect  = Mix_LoadWAV("pongIN.wav");  // rebounds
  Mix_Chunk *outEffect = Mix_LoadWAV("pongOUT.wav");  // ball exits the screen

  while (isRunning){
    // time is stored at the beginning of the frame
    frameStart = SDL_GetTicks();

    // (re)initialisation of all variables, in function of HEIGHT and WIDTH
    if (NEW_GAME){
       PZ = ((HEIGHT + WIDTH) / 60);
       if (PZ < 1) PZ = 1;
       SPEED = PZ / 2;
       paddleSpeed  = SPEED * 1.5;
       paddleL.x = PZ;
       paddleL.y = HEIGHT / 2 - 2 * PZ;
       paddleL.w = PZ;
       paddleL.h = PZ * 4;
       paddleR.x = WIDTH - 2 * PZ;
       paddleR.y = HEIGHT / 2 - 2 * PZ;
       paddleR.w = PZ;
       paddleR.h = PZ * 4;
       canvas.x = 0;
       canvas.y = 0;
       canvas.w = WIDTH;
       canvas.h = HEIGHT;
       dot.x = (WIDTH - PZ) / 2;
       dot.w = PZ;
       dot.h = PZ;
       ball.x = (WIDTH  - ball.w) / 2;
       ball.y = (HEIGHT - ball.h) / 2;
       ball.w = PZ;
       ball.h = PZ;
       scoreR = 0;
       scoreL = 0;
       ballSpeedX = rand() % 2 ? SPEED : -SPEED;
       ballSpeedY = rand() % (SPEED * 2) - SPEED;
       NEW_GAME = SDL_FALSE;
    }

    // adds a transparent black layer to 'clear' the screen, giving a nice fade effect
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 92);
    SDL_RenderFillRect(renderer, &canvas);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);

    // check for events
    while (SDL_PollEvent(&event)){
      if (event.type == SDL_QUIT)   // exit game
        isRunning = SDL_FALSE;
      if (event.type ==  SDL_KEYDOWN)
        switch (event.key.keysym.sym){
          case SDLK_ESCAPE:   // exit game
            isRunning = SDL_FALSE;
            break;
          case SDLK_q:
            P1_UP = SDL_TRUE;
            break;
          case SDLK_a:
            P1_DW = SDL_TRUE;
            break;
          case SDLK_UP:
            P2_UP = SDL_TRUE;
            break;
          case SDLK_DOWN:
            P2_DW = SDL_TRUE;
            break;
        }
      if (event.type ==  SDL_KEYUP)
        switch (event.key.keysym.sym){
          case SDLK_q:
            P1_UP = SDL_FALSE;
            break;
          case SDLK_a:
            P1_DW = SDL_FALSE;
            break;
          case SDLK_UP:
            P2_UP = SDL_FALSE;
            break;
          case SDLK_DOWN:
            P2_DW = SDL_FALSE;
            break;
          case SDLK_SPACE:
            NEW_GAME = SDL_TRUE; // restart game
        }
      if ((event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)){
          SDL_GetWindowSize(window, &WIDTH, &HEIGHT);
          NEW_GAME = SDL_TRUE;  // restart game
        }
      if ((event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)){
        while(1){
          SDL_PollEvent(&event); // pause the game while the window loses the focus
          if ((event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED))
            break;
          SDL_Delay(200);
        }
      }
    }

    // move the paddles
    if (P1_UP) paddleL.y -= paddleSpeed;
    if (paddleL.y < -paddleL.h+1) paddleL.y = -paddleL.h+1;
    if (P1_DW) paddleL.y += paddleSpeed;
    if (paddleL.y > HEIGHT-1) paddleL.y = HEIGHT-1;
    if (P2_UP) paddleR.y -= paddleSpeed;
    if (paddleR.y < -paddleR.h+1) paddleR.y = -paddleR.h+1;
    if (P2_DW) paddleR.y += paddleSpeed;
    if (paddleR.y > HEIGHT-1) paddleR.y = HEIGHT-1;

    // move the ball
    ball.x += ballSpeedX;
    ball.y += ballSpeedY;

    // ball rebonds on top or bottom of screen
    if (ball.y < 0 || ball.y >= HEIGHT - ball.h){
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      Mix_PlayChannel(-1, inEffect, 0);
      ballSpeedY *= -1;
    }

    // ball exist screen left, right player scores one point, the ball is served from right
    if (ball.x < 0){
      SDL_SetRenderDrawColor(renderer, 200, 0, 0, SDL_ALPHA_OPAQUE);  // flash the screen in red
      Mix_PlayChannel(-1, outEffect, 0);
      if (scoreR < 15 && scoreL < 15) scoreR++;
      ball.x = 3 * WIDTH / 4 - ball.w / 2;
      ball.y = (HEIGHT - ball.h) / 2;
      ballSpeedY = rand() % (SPEED * 2) - SPEED;
    }

    // ball exist screen right, left player scores one point, the ball is served from left
    if (ball.x >= WIDTH  - ball.w){
      SDL_SetRenderDrawColor(renderer, 200, 0, 0, SDL_ALPHA_OPAQUE);  // flash the screen in red
      Mix_PlayChannel(-1, outEffect, 0);
      if (scoreR < 15 && scoreL < 15) scoreL++;
      ball.x = 1 * WIDTH / 4  - ball.w / 2;
      ball.y = (HEIGHT - ball.h) / 2;
      ballSpeedY = rand() % (SPEED * 2) - SPEED;
    }

    // ball hits RIGHT paddle
    if (ball.x + ball.w >= paddleR.x  && ball.x + ball.w <= paddleR.x + paddleR.w && ball.y + ball.h >= paddleR.y && ball.y <= paddleR.y + paddleR.h){
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // flash the screen
      Mix_PlayChannel(-1, inEffect, 0);
      ball.x = paddleR.x - ball.w;
      ballSpeedX *= -1;
      ballSpeedY = SPEED * (((float)((ball.y + ball.h / 2) - (paddleR.y + paddleR.h / 2)) / (float)(ball.h / 2 + paddleR.h / 2)));
    }

    // ball hits LEFT paddle
    if (ball.x <= paddleL.x + paddleL.w && ball.x >= paddleL.x && ball.y + ball.h >= paddleL.y && ball.y <= paddleL.y + paddleL.h){
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // flash the screen
      Mix_PlayChannel(-1, inEffect, 0);
      ball.x = paddleL.x + paddleL.w;
      ballSpeedX *= -1;
      ballSpeedY = SPEED * (((float)((ball.y + ball.h / 2) - (paddleL.y + paddleL.h / 2)) / (float)(ball.h / 2 + paddleL.h / 2)));
    }

    // draw vertical dotted line in the middle to divide the screen
    for (dot.y = PZ / 2; dot.y < HEIGHT; dot.y += PZ * 2)
      SDL_RenderFillRect(renderer, &dot);

    // draw the scores at top of screen - 4 digits in total
    drawDigit(renderer, scoreL / 10, WIDTH / 2 - 7 * PZ, PZ, PZ/2);
    drawDigit(renderer, scoreL % 10, WIDTH / 2 - 4 * PZ, PZ, PZ/2);
    drawDigit(renderer, scoreR / 10, WIDTH / 2 + 2 * PZ, PZ, PZ/2);
    drawDigit(renderer, scoreR % 10, WIDTH / 2 + 5 * PZ, PZ, PZ/2);

    // draw paddles and ball
    SDL_RenderFillRect(renderer, &paddleL);
    SDL_RenderFillRect(renderer, &paddleR);
    SDL_RenderFillRect(renderer, &ball);

    // wait if there's some time left before rendering the frame
    frameTime = SDL_GetTicks() - frameStart;
    if (frameDelay > frameTime)
      SDL_Delay(frameDelay - frameTime);

    // render the screen
    SDL_RenderPresent(renderer);
  }

  // free all ressources and exit
  Mix_FreeChunk(inEffect);
  Mix_FreeChunk(outEffect);
  Mix_CloseAudio();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  exit(EXIT_SUCCESS);
}
