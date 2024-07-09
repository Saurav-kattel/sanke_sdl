#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <time.h>

#define FRAME_TIME 16.67
#define MOVING_SPEED 10

int SCORE = 0;
int current_direction = 1;

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

int getRandomNumber(int maxCap) {
    return rand() % maxCap;
}

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
        SCORE++;
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
    switch (direction) {
        case 1:
            if (current_direction == 2) {
                return;
            }
            if (sp->y > 0) {
                sp->y -= MOVING_SPEED;
            } else {
                sp->y = 800;
            }
            break;
        case 2:
            if (current_direction == 1) {
                return;
            }
            if (sp->y < 800) {
                sp->y += MOVING_SPEED;
            } else {
                sp->y = 0;
            }
            break;
        case 3:
            if (current_direction == 4) {
                return;
            }
            if (sp->x > 0) {
                sp->x -= MOVING_SPEED;
            } else {
                sp->x = 1400;
            }
            break;
        case 4:
            if (current_direction == 3) {
                return;
            }
            if (sp->x < 1400) {
                sp->x += MOVING_SPEED;
            } else {
                sp->x = 0;
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



int main(int argc, char* argv[]) {
    srand(time(NULL));

    snake sp = {1000, 100, 20, 20};
    food fp = {getRandomNumber(1301), getRandomNumber(701), 20, 20};

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized, Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("SNAKE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1400, 800, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        printf("Cannot initialize SDL window, Error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
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

    while (!quit) {
        startTime = SDL_GetTicks();

        updateSnakePosition(&sp, current_direction);
 
        if (checkCollision(&sp, &fp)) {
            updateFoodPost(&sp, &fp);
            SCORE++;
            updateSnakeSize(&sp);
        
    }

        while (SDL_PollEvent(&e) != 0) {
            handlePollEvent(&quit, &e, &sp, &fp);
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
        drawSnake(renderer, &sp);

        drawFood(renderer, &fp);

        SDL_RenderPresent(renderer);
        Uint32 frameTime = SDL_GetTicks() - startTime;
        if (frameTime < FRAME_TIME) {
            SDL_Delay(FRAME_TIME - frameTime);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
