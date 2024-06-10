#include "solver.h"

#define DEPTH 7

uint8_t bestMove;

int negamax(board_t *game, int depth){
    if(game->moves == WIDTH * HEIGHT){
        return 0;
    }

    if(gameOver(game)){
        return -1*(WIDTH * HEIGHT - game->moves / 2);
    }

    if(depth == 0){ 
       return 0; 
    }

    int bestScore = -WIDTH * HEIGHT; 

    int i;
    for(i = 0; i < WIDTH; i++){
        if(canAdd(game, i)){
            board_t game2;
            copy(&game2, game);

            addChip(&game2, i);
            int score = -negamax(&game2, depth - 1);
            if(score > bestScore){
                bestScore = score;
                if(depth == DEPTH){
                    bestMove = i;
                }
            }
        }
    }

    return bestScore;
}

int computerInput(board_t game){
    negamax(&game, DEPTH);
    return bestMove;
}
