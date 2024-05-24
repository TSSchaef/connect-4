#include "game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define EMPTY_TILE "src/images/empty-tile.png"
#define RED_TILE "src/images/red-tile.png"
#define YELLOW_TILE "src/images/yellow-tile.png"

#define SCRN_WIDTH 700
#define SCRN_HEIGHT 700
#define TILE_SIZE 100

SDL_Window *window;
SDL_Renderer *renderer;

bool keysPressed[SDL_NUM_SCANCODES];

SDL_Texture *emptyTile, *redTile, *yellowTile;
SDL_Rect bckgrndRect = {0, 0, SCRN_WIDTH, SCRN_HEIGHT};

TTF_Font* Hyperspace; 

int userInput(){
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT:
                return 0;
                break;

            case SDL_KEYDOWN:
                if(event.key.keysym.scancode == SDL_SCANCODE_Q){
                    return 0;
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
    if(keysPressed[SDL_SCANCODE_W] || keysPressed[SDL_SCANCODE_UP]){
   
    } else {
   
    }

    if(keysPressed[SDL_SCANCODE_A] || keysPressed[SDL_SCANCODE_LEFT]){
    }

    if(keysPressed[SDL_SCANCODE_D] || keysPressed[SDL_SCANCODE_RIGHT]){
    }

    if(keysPressed[SDL_SCANCODE_SPACE]){
    }
}

void drawBoard(board_t game){
    int i, j;
    for(i = 0; i < WIDTH; i++){
        for( j = 0; j < HEIGHT; j++){
            SDL_Rect tile = {i * TILE_SIZE, SCRN_HEIGHT - (j * TILE_SIZE) - TILE_SIZE, TILE_SIZE, TILE_SIZE}; 
            if(game.board[i][j] == 0){
                SDL_RenderCopy(renderer, emptyTile, NULL, &tile);
            } else if(game.board[i][j] == 1){
                SDL_RenderCopy(renderer, redTile, NULL, &tile);
            } else {
                SDL_RenderCopy(renderer, yellowTile, NULL, &tile);
            }
        }
    }
}

/*void drawBackground(){
    static uint64_t lastSwap = 0;
    static bool swap = true;

    if(SDL_GetTicks64() > lastSwap + BACKGROUND_FLICKER){
        lastSwap = SDL_GetTicks64();
        swap = !swap;
    }

    if(swap){
        SDL_RenderCopy(renderer, bckgrnd1, NULL, &bckgrndRect);
    } else {
        SDL_RenderCopy(renderer, bckgrnd2, NULL, &bckgrndRect);
    }
}*/

void render(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    //drawBackground();
    SDL_RenderPresent(renderer);
}

void gameLoop(){
	/*uint64_t frameStart;  
    uint64_t frameTime;
    uint64_t frameDelay = 1000 / FPS;

	while(1){
        frameStart = SDL_GetTicks64();
       
        userInput();
        updateGame();
		render();

        frameTime = SDL_GetTicks64() - frameStart;

        if(frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
	}	*/
}

int startScreen(){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
       return -1;
    }

    window = SDL_CreateWindow("Connect-4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCRN_WIDTH, SCRN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    emptyTile = IMG_LoadTexture(renderer, EMPTY_TILE);
    redTile = IMG_LoadTexture(renderer, RED_TILE);
    yellowTile = IMG_LoadTexture(renderer, YELLOW_TILE);

    TTF_Init();
//    Hyperspace = TTF_OpenFont("src/Hyperspace.ttf", 24);
    
    return 0;
}

void endScreen(){
//    TTF_CloseFont(Hyperspace);
    TTF_Quit();

    SDL_DestroyTexture(emptyTile);
    SDL_DestroyTexture(redTile);
    SDL_DestroyTexture(yellowTile);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[]){
    board_t game;
    init_board(&game);
    addChip(&game, 4);
    switchPlayer(&game);
    addChip(&game, 4);
    switchPlayer(&game);
    addChip(&game, 4);
    switchPlayer(&game);
    addChip(&game, 4);
    switchPlayer(&game);
    addChip(&game, 2);

    startScreen();
    drawBoard(game);

    SDL_RenderPresent(renderer);

    while(userInput()){}

    endScreen();

    return 0;
}
