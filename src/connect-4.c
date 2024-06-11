#include "game.h"
#include "solver.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define FPS 60

#define EMPTY_TILE "src/images/empty-tile.png"
#define RED_TILE "src/images/red-tile.png"
#define YELLOW_TILE "src/images/yellow-tile.png"

#define TILE_SIZE 100
#define SCRN_WIDTH (TILE_SIZE*WIDTH)
#define SCRN_HEIGHT (TILE_SIZE*(HEIGHT + 1))

SDL_Window *window;
SDL_Renderer *renderer;

bool keysPressed[SDL_NUM_SCANCODES];

SDL_Texture *emptyTile, *redTile, *yellowTile;
SDL_Rect bckgrndRect = {0, 0, SCRN_WIDTH, SCRN_HEIGHT};

TTF_Font* Hyperspace; 

board_t game;
//current column selected by player
uint8_t currColumn = 3; 

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
                printf("%d\n", input);
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
        SDL_RenderCopy(renderer, redTile, NULL, &tile);
    } else {
        SDL_RenderCopy(renderer, yellowTile, NULL, &tile);
    }
}

void drawBoard(){
    int i, j;
    for(i = 0; i < WIDTH; i++){
        for( j = 0; j < HEIGHT; j++){
            SDL_Rect tile = {i * TILE_SIZE, SCRN_HEIGHT - (j * TILE_SIZE) - TILE_SIZE, TILE_SIZE, TILE_SIZE}; 
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
    drawSelection();
    drawBoard();

    SDL_RenderPresent(renderer);
}

void gameLoop(){
	uint64_t frameStart;  
    uint64_t frameTime;
    uint64_t frameDelay = 1000 / FPS;

	while(userInput()){
        frameStart = SDL_GetTicks64();
       
        updateGame();
		render();

        if(game.gameOver){
            if(game.moves % 2 == 0){
                printf("Yellow");
            } else {
                printf("Red");
            }
            printf(" won!\n");
            SDL_Delay(2500);
            break;
        }

        if(game.moves == WIDTH * HEIGHT){
            printf("Tie game!\n");
            SDL_Delay(2500);
            break;
        }

        frameTime = SDL_GetTicks64() - frameStart;

        if(frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
	}
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
    startScreen();

    init_board(&game);
    gameLoop();

    endScreen();

    return 0;
}
