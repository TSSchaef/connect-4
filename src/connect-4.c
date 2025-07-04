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

#define TILE_SIZE 100
#define SCRN_WIDTH (TILE_SIZE*WIDTH)
#define SCRN_HEIGHT (TILE_SIZE*(HEIGHT + 1))

typedef enum { STATE_PLAYING, STATE_GAMEOVER } GameState;
GameState gameState = STATE_PLAYING;

SDL_Window *window;
SDL_Renderer *renderer;

bool keysPressed[SDL_NUM_SCANCODES];

SDL_Texture *emptyTile, *redTile, *yellowTile;
SDL_Rect bckgrndRect = {0, 0, SCRN_WIDTH, SCRN_HEIGHT};

TTF_Font* Hyperspace; 

board_t game;
uint8_t currColumn = 3; 

int shouldQuit = 0;
uint32_t gameOverTicks = 0;
uint32_t tieGameTicks = 0;

int userInput() {
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT:
                shouldQuit = 1;
                return 0;
            case SDL_KEYDOWN:
                if(event.key.keysym.scancode == SDL_SCANCODE_Q){
                    shouldQuit = 1;
#ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
#endif
                    return 0;
                }
                if (gameState == STATE_GAMEOVER && event.key.keysym.scancode == SDL_SCANCODE_R) {
                    // Restart game
                    init_board(&game);
                    currColumn = 3;
                    gameOverTicks = 0;
                    tieGameTicks = 0;
                    gameState = STATE_PLAYING;
                    printf("Restarting game!\n");
                }
                keysPressed[event.key.keysym.scancode] = true;
            break;
            case SDL_KEYUP:
                keysPressed[event.key.keysym.scancode] = false;
            break;
        }
    }
    return 1;
}

void updateGame(){
    static bool justMoved = false;
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
            if(!game.gameOver){
                int input = computerInput(game);
                addChip(&game, input); 
            }
        }
        justMoved = true;
    } else {
        justMoved = false;
    }
}

void drawSelection(){
    SDL_Rect tile = {currColumn * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
    if(game.moves % 2 == 0){
        if (SDL_RenderCopy(renderer, redTile, NULL, &tile) != 0) {
            printf("SDL_RenderCopy (drawSelection, redTile) error: %s\n", SDL_GetError());
        }
    } else {
        if (SDL_RenderCopy(renderer, yellowTile, NULL, &tile) != 0) {
            printf("SDL_RenderCopy (drawSelection, yellowTile) error: %s\n", SDL_GetError());
        }
    }
}

void drawBoard(){
    int i, j;
    for(i = 0; i < WIDTH; i++){
        for( j = 0; j < HEIGHT; j++){
            SDL_Rect tile = {i * TILE_SIZE, SCRN_HEIGHT - (j * TILE_SIZE) - TILE_SIZE, TILE_SIZE, TILE_SIZE}; 
            if(game.position & (BOTTOM_LEFT << (j + (i * (HEIGHT + 1))))){
                if (SDL_RenderCopy(renderer, (game.moves % 2 == 0) ? redTile : yellowTile, NULL, &tile) != 0) {
                    printf("SDL_RenderCopy (drawBoard, pos) error: %s\n", SDL_GetError());
                }
            } else if(game.mask & (BOTTOM_LEFT << (j + (i * (HEIGHT + 1))))){
                if (SDL_RenderCopy(renderer, (game.moves % 2 == 1) ? redTile : yellowTile, NULL, &tile) != 0) {
                    printf("SDL_RenderCopy (drawBoard, mask) error: %s\n", SDL_GetError());
                }
            } else {
                if (SDL_RenderCopy(renderer, emptyTile, NULL, &tile) != 0) {
                    printf("SDL_RenderCopy (drawBoard, emptyTile) error: %s\n", SDL_GetError());
                }
            }
        }
    }
}

void render(){
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) != 0) {
        printf("SDL_SetRenderDrawColor error: %s\n", SDL_GetError());
    }
    if (SDL_RenderClear(renderer) != 0) {
        printf("SDL_RenderClear error: %s\n", SDL_GetError());
    }
    drawSelection();
    drawBoard();
    if (gameState == STATE_GAMEOVER) {
        if(Hyperspace){
            SDL_Color white = {255, 255, 255, 255};
            SDL_Surface *surface = TTF_RenderText_Solid(Hyperspace, "Press R to restart or Q to quit", white);
            SDL_Texture *msg = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect msgRect = {40, SCRN_HEIGHT/2, surface->w, surface->h};
            SDL_RenderCopy(renderer, msg, NULL, &msgRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(msg);
        } else {
            fprintf(stderr, "Failed to load font, press R to restart");
        }
    }
    SDL_RenderPresent(renderer);
}

// --- New: game loop frame function for Emscripten ---
void gameLoopFrame(){
    if (shouldQuit) {
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#endif
        return;
    }

    // Only process input if game is not over
    if (!game.gameOver && game.moves < WIDTH * HEIGHT) {
        userInput();
        updateGame();
        render();
    } else if(gameState == STATE_PLAYING){
        render();
        // Set game state to game over, so we can process restart/quit input
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
        // Allow input for restart or quit
        userInput();
        render();
    }
}

int startScreen(){
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return -1;
    }
    window = SDL_CreateWindow("Connect-4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCRN_WIDTH, SCRN_HEIGHT, 0);
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

    if (TTF_Init() == -1) {
        printf("TTF_Init error: %s\n", TTF_GetError());
    }
    Hyperspace = TTF_OpenFont("src/Hyperspace.ttf", 24);
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
    // desktop fallback: keep the old blocking loop for native
    while(!shouldQuit) {
        gameLoopFrame();
        SDL_Delay(1000/FPS);
    }
#endif

    endScreen();
    return 0;
}
