#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "./constants.h"

int game_is_running = false;
int last_frame_time = 0;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

struct game_object {
  float x;
  float y;
  float width;
  float height;
  float vel_x;
  float vel_y;
} ball, paddle;

int initialize_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL!\n");
    return false;
  }

  /* @param title  {string}
   * @param xcord  {integer}
   * @param ycord  {integer}
   * @param width  {integer}
   * @param height {integer}
   * @param option {builtin}
  **/
  window = SDL_CreateWindow(
    NULL,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    SDL_WINDOW_BORDERLESS
  );
  if (!window) {
    fprintf(stderr, "Error creating SDL Window.\n");
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    fprintf(stderr, "Error creating SDL Renderer.\n");
    return false;
  }
  return true;
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
      case SDL_QUIT:
        game_is_running = false;
        break;
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
          game_is_running = false;
        if (event.key.keysym.sym == SDLK_LEFT)
          paddle.vel_x = -400;
        if (event.key.keysym.sym == SDLK_RIGHT)
          paddle.vel_x = 400;
        break;
      case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_LEFT)
          paddle.vel_x = 0;
        if (event.key.keysym.sym == SDLK_RIGHT)
          paddle.vel_x = 0;
        break;
    }
}

void setup(void) {
  ball.x = WINDOW_WIDTH / 2;
  ball.y = 20;
  ball.width = 15;
  ball.height = 15;
  ball.vel_x = -200;
  ball.vel_y = 150;

  paddle.x = WINDOW_WIDTH / 2 - (paddle.width / 2);
  paddle.y = WINDOW_HEIGHT - 40;
  paddle.width = 100;
  paddle.height = 20;
  paddle.vel_x = 0;
  paddle.vel_y = 0;
}

void update(void) {

  // Calculate game objects are updated at a fixed and consistent interval
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

  // Only enforce this rule if needed
  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  // Get the difference in seconds between the current and previous frames
  float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;

  last_frame_time = SDL_GetTicks();

  // Update the position as a function of dela time (makes it x frames / sec)
  ball.x += ball.vel_x * delta_time;
  ball.y += ball.vel_y * delta_time;

  // TODO: update paddle position based on its velocity
  paddle.x += paddle.vel_x * delta_time;

  // TODO: check for ball collision with the walls
  if (ball.x < 0 || ball.x + ball.width > WINDOW_WIDTH) {
    ball.vel_x = -ball.vel_x;
  }
  if (ball.y < 0) {
    ball.vel_y = -ball.vel_y;
  }

  // TODO: check for ball collision with the paddle
  if
  (
    ball.y + ball.height >= paddle.y &&
    ball.x + ball.width >= paddle.x &&
    ball.x <= paddle.x + paddle.width
  )
  {
    ball.vel_y = -ball.vel_y;
  }

  // TODO: prevent paddle from moving outside from boundaries of the window
  if (paddle.x <= 0)
      paddle.x = 0;
  if (paddle.x >= WINDOW_WIDTH - paddle.width)
      paddle.x = WINDOW_WIDTH - paddle.width;

  // Check for game over
  if (ball.y + ball.height > WINDOW_HEIGHT) {
      ball.x = WINDOW_WIDTH / 2;
      ball.y = 0;
  }
}

void render(void) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // Draw a rectangle
  SDL_Rect ball_rect = {
    (int)ball.x,
    (int)ball.y,
    (int)ball.width,
    (int)ball.height
  };
  // Draw right paddle
  SDL_Rect paddle_rect = {
    (int)paddle.x,
    (int)paddle.y,
    (int)paddle.width,
    (int)paddle.height
  };

  // Select fill color
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  // Render shapes into screen
  SDL_RenderFillRect(renderer, &ball_rect);
  SDL_RenderFillRect(renderer, &paddle_rect);

  // Double "front and back" buffer, prevent glitches
  SDL_RenderPresent(renderer);
}

void destroy_window(void) {
  // Clean up variables used by SDL, in reverse order they weree declared
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main(void) {
  game_is_running = initialize_window();

  setup();

  // Game Loop
  while(game_is_running) {
    process_input();
    update();
    render();
  }

  destroy_window();

  return true;
}
