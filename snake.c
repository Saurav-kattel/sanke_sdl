#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FRAME_TIME 16.67
#define SCREEN_WIDTH 1400
#define SCREEN_HEIGHT 800

int MOVING_SPEED = 3;
int SCORE = 0;
int current_direction = 1;
int CURRENT_LENGTH = 0;

typedef struct Snake {
  int x;
  int y;
  int width;
  int height;
} snake;

typedef struct Food {
  int x;
  int y;
  int width;
  int height;
} food;

int getRandomNumber(int maxCap) { return rand() % maxCap; }

void updateFoodPost(snake *sp, food *fp) {
  int x = getRandomNumber(901);
  int y = getRandomNumber(701);

  while ((x == sp->x && y == sp->y) || (x == fp->x && y == fp->y)) {
    x = getRandomNumber(901);
    y = getRandomNumber(701);
  }

  fp->y = y;
  fp->x = x;
}

int checkCollision(snake *sp, food *fp) {
  int snake_center_x = sp->x + sp->width / 2;
  int snake_center_y = sp->y + sp->height / 2;

  if (snake_center_x >= fp->x && snake_center_x < (fp->x + fp->width) &&
      snake_center_y >= fp->y && snake_center_y < (fp->y + fp->height)) {
    return 1;
  }
  return 0;
}

void drawSnake(SDL_Renderer *ren, snake *sp) {
  SDL_SetRenderDrawColor(ren, 0x00, 0xFF, 0xFF, 0xFF);
  SDL_Rect rect = {sp->x, sp->y, sp->width, sp->height};
  SDL_RenderFillRect(ren, &rect);
}

void drawFood(SDL_Renderer *ren, food *fp) {
  SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0x00, 0xFF);
  SDL_Rect rect = {fp->x, fp->y, fp->width, fp->height};
  SDL_RenderFillRect(ren, &rect);
}

void updateSnakePosition(snake *sp, int direction) {
  // Check direction before updating positions
  if ((direction == 1 && current_direction == 2) ||
      (direction == 2 && current_direction == 1) ||
      (direction == 3 && current_direction == 4) ||
      (direction == 4 && current_direction == 3)) {
    return;
  }

  // Move body segments
  for (int i = CURRENT_LENGTH - 1; i > 0; i--) {
    sp[i].x = sp[i - 1].x;
    sp[i].y = sp[i - 1].y;
  }

  // Move the head
  switch (direction) {
  case 1:
    if (sp[0].y > 0) {
      sp[0].y -= MOVING_SPEED;
    } else {
      sp[0].y = SCREEN_HEIGHT;
    }
    break;
  case 2:
    if (sp[0].y < SCREEN_HEIGHT) {
      sp[0].y += MOVING_SPEED;
    } else {
      sp[0].y = 0;
    }
    break;
  case 3:
    if (sp[0].x > 0) {
      sp[0].x -= MOVING_SPEED;
    } else {
      sp[0].x = SCREEN_WIDTH;
    }
    break;
  case 4:
    if (sp[0].x < SCREEN_WIDTH) {
      sp[0].x += MOVING_SPEED;
    } else {
      sp[0].x = 0;
    }
    break;
  default:
    break;
  }

  current_direction = direction;
}

void handleKeyPress(int *flag, SDL_Event *e, snake *sp, food *fp) {
  switch (e->key.keysym.sym) {
  case SDLK_ESCAPE:
    *flag = 1;
    break;
  case SDLK_UP:
    updateSnakePosition(sp, 1);
    break;
  case SDLK_DOWN:
    updateSnakePosition(sp, 2);
    break;
  case SDLK_LEFT:
    updateSnakePosition(sp, 3);
    break;
  case SDLK_RIGHT:
    updateSnakePosition(sp, 4);
    break;
  }
}

void handlePollEvent(int *quit, SDL_Event *e, snake *sp, food *fp) {
  if (e->type == SDL_QUIT) {
    *quit = 1;
  } else if (e->type == SDL_KEYDOWN) {
    handleKeyPress(quit, e, sp, fp);
  }
}

void addNewSnake(snake **s, int *snake_capacity) {
  if (CURRENT_LENGTH >= *snake_capacity) {
    *snake_capacity += 2;
    *s = realloc(*s, *snake_capacity * sizeof(snake));
  }

  if (CURRENT_LENGTH > 0) {
    (*s)[CURRENT_LENGTH].x =
        (*s)[CURRENT_LENGTH - 1].x + (*s)[CURRENT_LENGTH - 1].width;
    (*s)[CURRENT_LENGTH].y = (*s)[CURRENT_LENGTH - 1].y;
    (*s)[CURRENT_LENGTH].width = (*s)[CURRENT_LENGTH - 1].width;
    (*s)[CURRENT_LENGTH].height = (*s)[CURRENT_LENGTH - 1].height;
  } else {
    (*s)[CURRENT_LENGTH].x = SCREEN_WIDTH / 2;
    (*s)[CURRENT_LENGTH].y = SCREEN_HEIGHT / 2;
    (*s)[CURRENT_LENGTH].width = 20;
    (*s)[CURRENT_LENGTH].height = 20;
  }

  CURRENT_LENGTH++;
}

int checkGameOver(snake **s) {

  snake head = *(s)[0];

  int head_x = head.x + head.width / 2;
  int head_y = head.y + head.height / 2;

  for (int i = 1; i < CURRENT_LENGTH; i++) {

    int segment_center_x = (*s)[i].x + (*s)[i].width / 2;
    int segment_center_y = (*s)[i].y + (*s)[i].height / 2;
    if (head_x == segment_center_x && head_y == segment_center_y) {
      printf("Game over\n");
      return 1;
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {

  int current_snake_capacity = 2;
  srand(time(NULL));

  food fp = {getRandomNumber(1301), getRandomNumber(701), 20, 20};
  snake *snakes = (snake *)malloc(sizeof(snake) * current_snake_capacity);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not be initialized, Error: %s\n", SDL_GetError());
    return 1;
  }

  if (TTF_Init() == -1) {
    printf("TTF_Init Error: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Window *win =
      SDL_CreateWindow("SNAKE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       1400, 800, SDL_WINDOW_SHOWN);
  if (win == NULL) {
    printf("Cannot initialize SDL window, Error: %s\n", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("Cannot initialize SDL renderer, Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Event e;
  int quit = 0;
  Uint32 startTime = 0;

  addNewSnake(&snakes, &current_snake_capacity);

  while (!quit) {
    startTime = SDL_GetTicks();
    updateSnakePosition(snakes, current_direction);

    if (checkGameOver(&snakes)) {
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(win);
      SDL_Quit();
      free(snakes);
      exit(1);
    }
    if (checkCollision(&snakes[0], &fp)) {
      updateFoodPost(snakes, &fp);
      SCORE++;
      if ((SCORE > 0) && (SCORE % 5 == 0)) {
        addNewSnake(&snakes, &current_snake_capacity);
        MOVING_SPEED += 2;
      }
    }

    while (SDL_PollEvent(&e) != 0) {
      handlePollEvent(&quit, &e, snakes, &fp);
    }

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);

    for (int i = 0; i < CURRENT_LENGTH; i++) {
      drawSnake(renderer, &snakes[i]);
    }

    drawFood(renderer, &fp);

    SDL_Color textColor = {255, 255, 255, 255}; // White color

    char scoreText[20];
    sprintf(scoreText, "Score: %d", SCORE);

    TTF_Font *font =
        TTF_OpenFont("/usr/share/fonts/TTF/M+2NerdFont-Thin.ttf", 24);
    if (font == NULL) {
      printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
    } else {
      SDL_Surface *textSurface =
          TTF_RenderText_Solid(font, scoreText, textColor);
      if (textSurface == NULL) {
        // Handle error if necessary
      } else {
        SDL_Texture *textTexture =
            SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture == NULL) {
          printf("Failed to create texture from surface! SDL Error: %s\n",
                 SDL_GetError());
        } else {
          int textWidth = textSurface->w;
          int textHeight = textSurface->h;

          SDL_Rect textRect = {SCREEN_WIDTH - textWidth - 20, 20, textWidth,
                               textHeight};

          SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

          SDL_DestroyTexture(textTexture);
        }

        SDL_FreeSurface(textSurface);
      }

      TTF_CloseFont(font);
    }

    SDL_RenderPresent(renderer);
    Uint32 frameTime = SDL_GetTicks() - startTime;
    if (frameTime < FRAME_TIME) {
      SDL_Delay(FRAME_TIME - frameTime);
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();
  free(snakes);
  return 0;
}
