#include "game.h"
#include "solver.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define FPS 60

#define EMPTY_TILE "src/images/empty-tile.png"
#define RED_TILE "src/images/red-tile.png"
#define YELLOW_TILE "src/images/yellow-tile.png"
#define PREVIEW_RED "src/images/red.png"
#define PREVIEW_YELLOW "src/images/yellow.png"

#define BASE_TILE_SIZE 100
#define BASE_MENU_HEIGHT 56
#define BASE_STATUS_HEIGHT 52
#define WIDTH 7
#define HEIGHT 6

typedef enum { STATE_PLAYING, STATE_GAMEOVER } GameState;
GameState gameState = STATE_PLAYING;

SDL_Window *window;
SDL_Renderer *renderer;

bool keysPressed[SDL_NUM_SCANCODES];

SDL_Texture *emptyTile, *redTile, *yellowTile;
SDL_Texture *previewRed, *previewYellow;
TTF_Font* Hyperspace;

board_t game;
uint8_t currColumn = 3;

int shouldQuit = 0;
uint32_t gameOverTicks = 0;
uint32_t tieGameTicks = 0;

int window_width = BASE_TILE_SIZE * WIDTH;
int window_height = BASE_TILE_SIZE * HEIGHT + BASE_MENU_HEIGHT + BASE_STATUS_HEIGHT + BASE_TILE_SIZE;

enum InputSource { INPUT_NONE, INPUT_MOUSE, INPUT_KEYBOARD };
enum InputSource lastInputSource = INPUT_NONE;
uint32_t lastInputTimestamp = 0;

int mouseColumn = -1;

void get_layout(int* tile_size, int* menu_height, int* status_height, int* preview_y, int* status_y, int* board_y) {
    *menu_height = BASE_MENU_HEIGHT * window_width / (BASE_TILE_SIZE * WIDTH);
    *status_height = BASE_STATUS_HEIGHT * window_width / (BASE_TILE_SIZE * WIDTH);

    int remaining_height = window_height - *menu_height - *status_height;
    int tile_size_x = window_width / WIDTH;
    int tile_size_y = remaining_height / (HEIGHT + 1);
    *tile_size = (tile_size_x < tile_size_y) ? tile_size_x : tile_size_y;

    int used_height = *tile_size * (HEIGHT + 1);
    int top_offset = *menu_height + ((remaining_height - used_height) > 0 ? (remaining_height - used_height)/2 : 0);

    *preview_y = top_offset;
    *status_y = *menu_height;
    *board_y = top_offset + *tile_size;
}

int userInput() {
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT:
#ifndef __EMSCRIPTEN__
                shouldQuit = 1;
#endif
                return 0;
            case SDL_KEYDOWN:
#ifdef __EMSCRIPTEN__
                if(event.key.keysym.scancode == SDL_SCANCODE_R) {
#else
                if(event.key.keysym.scancode == SDL_SCANCODE_R || event.key.keysym.scancode == SDL_SCANCODE_Q) {
#endif
#ifndef __EMSCRIPTEN__
                    if(event.key.keysym.scancode == SDL_SCANCODE_Q){
                        shouldQuit = 1;
                        return 0;
                    }
#endif
                    if(event.key.keysym.scancode == SDL_SCANCODE_R) {
                        init_board(&game);
                        currColumn = 3;
                        gameOverTicks = 0;
                        tieGameTicks = 0;
                        mouseColumn = -1;
                        gameState = STATE_PLAYING;
                        lastInputSource = INPUT_NONE;
                        for (int i = 0; i < SDL_NUM_SCANCODES; ++i) keysPressed[i] = false;
                        break;
                    }
                }
                keysPressed[event.key.keysym.scancode] = true;
                lastInputSource = INPUT_KEYBOARD;
                lastInputTimestamp = SDL_GetTicks();
                break;
            case SDL_KEYUP:
                keysPressed[event.key.keysym.scancode] = false;
                break;
            case SDL_MOUSEMOTION: {
                int tile_size, menu_height, status_height, preview_y, status_y, board_y;
                get_layout(&tile_size, &menu_height, &status_height, &preview_y, &status_y, &board_y);
                int x = event.motion.x;
                int col = (x >= 0) ? (x / tile_size) : 0;
                if (col < 0) col = 0;
                if (col >= WIDTH) col = WIDTH - 1;
                mouseColumn = col;
                lastInputSource = INPUT_MOUSE;
                lastInputTimestamp = SDL_GetTicks();
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT && gameState == STATE_PLAYING) {
                    int col = (lastInputSource == INPUT_MOUSE && mouseColumn >= 0) ? mouseColumn : currColumn;
                    if (col >= 0 && canAdd(&game, col)) {
                        addChip(&game, col);
                        if (!game.gameOver && game.moves < WIDTH * HEIGHT) {
                            int input = computerInput(game);
                            addChip(&game, input);
                        }
                    }
                }
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                }
                break;
        }
    }
    return 1;
}

void updateGame(){
    static bool justMoved = false;
    if (lastInputSource == INPUT_KEYBOARD) {
        if(keysPressed[SDL_SCANCODE_A] || keysPressed[SDL_SCANCODE_LEFT]){
            if(!justMoved && currColumn > 0) currColumn--;
            justMoved = true;
        } else if(keysPressed[SDL_SCANCODE_D] || keysPressed[SDL_SCANCODE_RIGHT]){
            if(!justMoved && currColumn < WIDTH - 1) currColumn++;
            justMoved = true;
        } else if(keysPressed[SDL_SCANCODE_SPACE] || keysPressed[SDL_SCANCODE_S]
                  || keysPressed[SDL_SCANCODE_DOWN]){
            if(!justMoved && canAdd(&game, currColumn)){
                addChip(&game, currColumn);
                if(!game.gameOver && game.moves < WIDTH * HEIGHT){
                    int input = computerInput(game);
                    addChip(&game, input);
                }
            }
            justMoved = true;
        } else {
            justMoved = false;
        }
    }
    if (lastInputSource == INPUT_MOUSE && mouseColumn >= 0) {
        currColumn = mouseColumn;
    }
}

// --- Draw the menu at the top ---
void drawMenu() {
    int tile_size, menu_height, status_height, preview_y, status_y, board_y;
    get_layout(&tile_size, &menu_height, &status_height, &preview_y, &status_y, &board_y);

    if (!Hyperspace) return;

    SDL_Color yellow = {255, 255, 100, 255};
    char menuText[256];
#ifdef __EMSCRIPTEN__
    if (gameState == STATE_GAMEOVER) {
        snprintf(menuText, sizeof(menuText), "R to restart");
    } else {
        snprintf(menuText, sizeof(menuText),
            "Left/Right, A/D, or Mouse to select   |   Space/Down/Click to drop   |   R to restart");
    }
#else
    if (gameState == STATE_GAMEOVER) {
        snprintf(menuText, sizeof(menuText), "R to restart   |   Q to quit");
    } else {
        snprintf(menuText, sizeof(menuText),
             "Left/Right, A/D, or Mouse to select   |   Space/Down/Click to drop   |   R to restart | Q to quit");
    }
#endif

    SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(Hyperspace, menuText, yellow, window_width - 12);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;
    rect.x = 6;
    rect.y = 0;
    rect.w = (surface->w > window_width-12) ? window_width-12 : surface->w;
    rect.h = surface->h;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// --- Draw the preview chip above the board using new PNGs ---
void drawSelection(){
    int tile_size, menu_height, status_height, preview_y, status_y, board_y;
    get_layout(&tile_size, &menu_height, &status_height, &preview_y, &status_y, &board_y);
    SDL_Rect tile = {currColumn * tile_size, preview_y, tile_size, tile_size};
    SDL_Texture* preview = (game.moves % 2 == 0) ? previewRed : previewYellow;
    SDL_RenderCopy(renderer, preview, NULL, &tile);
}

// --- Draw the board using the original PNGs ---
void drawBoard(){
    int tile_size, menu_height, status_height, preview_y, status_y, board_y;
    get_layout(&tile_size, &menu_height, &status_height, &preview_y, &status_y, &board_y);
    for(int i = 0; i < WIDTH; i++){
        for(int j = 0; j < HEIGHT; j++){
            SDL_Rect tile = {
                i * tile_size,
                board_y + (HEIGHT - 1 - j) * tile_size,
                tile_size, tile_size
            };
            if(game.position & (BOTTOM_LEFT << (j + (i * (HEIGHT + 1))))){
                SDL_RenderCopy(renderer, (game.moves % 2 == 0) ? redTile : yellowTile, NULL, &tile);
            } else if(game.mask & (BOTTOM_LEFT << (j + (i * (HEIGHT + 1))))){
                SDL_RenderCopy(renderer, (game.moves % 2 == 1) ? redTile : yellowTile, NULL, &tile);
            } else {
                SDL_RenderCopy(renderer, emptyTile, NULL, &tile);
            }
        }
    }
}

// --- Draw game over status message under the menu bar (if game ended), centered ---
void drawStatusBar() {
    if (gameState != STATE_GAMEOVER) return;
    int tile_size, menu_height, status_height, preview_y, status_y, board_y;
    get_layout(&tile_size, &menu_height, &status_height, &preview_y, &status_y, &board_y);

    const char* msg = NULL;
    SDL_Color color;
    if (game.mask == ((1ULL << ((WIDTH)*(HEIGHT+1)))-1)) {
        msg = "Tie Game!";
        color.r = 200; color.g = 200; color.b = 255; color.a = 255;
    } else if ((game.moves % 2) == 0) {
        msg = "Yellow Won!";
        color.r = 255; color.g = 255; color.b = 50; color.a = 255;
    } else {
        msg = "Red Won!";
        color.r = 255; color.g = 60; color.b = 60; color.a = 255;
    }
    if (Hyperspace && msg) {
        SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(Hyperspace, msg, color, window_width - 12);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect;
        rect.x = (window_width - surface->w) / 2;
        if (rect.x < 0) rect.x = 0;
        rect.y = status_y;
        rect.w = surface->w;
        rect.h = surface->h;
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void render(){
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 40, 255) != 0) {
        printf("SDL_SetRenderDrawColor error: %s\n", SDL_GetError());
    }
    if (SDL_RenderClear(renderer) != 0) {
        printf("SDL_RenderClear error: %s\n", SDL_GetError());
    }
    drawMenu();
    drawStatusBar();
    drawSelection();
    drawBoard();
    SDL_RenderPresent(renderer);
}

void gameLoopFrame(){
    if (shouldQuit) {
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#endif
        return;
    }

    if (!game.gameOver && game.moves < WIDTH * HEIGHT) {
        userInput();
        updateGame();
        render();
    } else if(gameState == STATE_PLAYING){
        render();
        gameState = STATE_GAMEOVER;
        uint32_t now = SDL_GetTicks();
        if(game.gameOver && !gameOverTicks) {
            gameOverTicks = now;
            printf("%s won!\n", (game.moves % 2 == 0) ? "Yellow" : "Red");
        }
        if(game.moves == WIDTH * HEIGHT && !tieGameTicks) {
            tieGameTicks = now;
            printf("Tie game!\n");
        }
    } else if(gameState == STATE_GAMEOVER) {
        userInput();
        render();
    }
}

int startScreen(){
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return -1;
    }
    window = SDL_CreateWindow(
        "Connect-4",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height,
        SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        return -1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer error: %s\n", SDL_GetError());
        return -1;
    }

    emptyTile = IMG_LoadTexture(renderer, EMPTY_TILE);
    if (!emptyTile) {
        printf("IMG_LoadTexture (emptyTile) error: %s\n", IMG_GetError());
    }
    redTile = IMG_LoadTexture(renderer, RED_TILE);
    if (!redTile) {
        printf("IMG_LoadTexture (redTile) error: %s\n", IMG_GetError());
    }
    yellowTile = IMG_LoadTexture(renderer, YELLOW_TILE);
    if (!yellowTile) {
        printf("IMG_LoadTexture (yellowTile) error: %s\n", IMG_GetError());
    }

    previewRed = IMG_LoadTexture(renderer, PREVIEW_RED);
    if (!previewRed) {
        printf("IMG_LoadTexture (previewRed) error: %s\n", IMG_GetError());
    }
    previewYellow = IMG_LoadTexture(renderer, PREVIEW_YELLOW);
    if (!previewYellow) {
        printf("IMG_LoadTexture (previewYellow) error: %s\n", IMG_GetError());
    }

    if (TTF_Init() == -1) {
        printf("TTF_Init error: %s\n", TTF_GetError());
    }
    Hyperspace = TTF_OpenFont("src/Hyperspace.ttf", 24 * window_width / (BASE_TILE_SIZE * WIDTH));
    if (!Hyperspace) {
        printf("TTF_OpenFont error: %s\n", TTF_GetError());
    }
    return 0;
}

void endScreen(){
    if (Hyperspace) TTF_CloseFont(Hyperspace);
    TTF_Quit();

    if (emptyTile) SDL_DestroyTexture(emptyTile);
    if (redTile) SDL_DestroyTexture(redTile);
    if (yellowTile) SDL_DestroyTexture(yellowTile);
    if (previewRed) SDL_DestroyTexture(previewRed);
    if (previewYellow) SDL_DestroyTexture(previewYellow);

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[]){
    if (startScreen() != 0) {
        printf("Failed to initialize, exiting\n");
        return 1;
    }
    init_board(&game);
    init_opponent();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(gameLoopFrame, 0, 1);
#else
    while(!shouldQuit) {
        gameLoopFrame();
        SDL_Delay(1000/FPS);
    }
#endif

    endScreen();
    return 0;
}
