#include "solver.h"
#include <stdio.h>

#define ALPHA -1
#define BETA 1
#define DEPTH 14

uint8_t bestMove;
uint64_t numEvaluated;

int negamax(board_t *game, int alpha, int beta, int depth){
    numEvaluated++;
    if(game->moves == WIDTH * HEIGHT){
        return 0;
    }

    int i;
    for(i = 0; i < WIDTH; i++){
        if(canAdd(game, i) && isWinningMove(game, i)){
            if(depth == DEPTH){
                bestMove = i;
            }
            return (WIDTH * HEIGHT) - (game->moves / 2);
        }
    }
    
    int max = (WIDTH * HEIGHT) - (game->moves / 2);
    if(beta > max){
        beta = max;
        if(alpha >= beta) return beta;
    }

    if(depth == 0){ 
       return 0; 
    }

    for(i = 0; i < WIDTH; i++){
        if(canAdd(game, i)){
            if(bestMove > WIDTH) bestMove = i;
            board_t game2;
            copy(&game2, game);

            addChip(&game2, i);
            int score = -negamax(&game2, -beta, -alpha, depth - 1);
            if(score >= beta){
                if(depth == DEPTH){
                    bestMove = i;
                }
                return score;
            }
            if(score > alpha) {
                if(depth == DEPTH){
                    bestMove = i;
                }
                alpha = score;
            }
        }
    }

    return alpha;
}

int computerInput(board_t game){
    numEvaluated = 0;
    bestMove = WIDTH + 1;
    negamax(&game, ALPHA, BETA, DEPTH);
    printf("Evaluated: %ld\n", numEvaluated);
    return bestMove;
}
