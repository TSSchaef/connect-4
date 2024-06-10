#include "solver.h"

#define ALPHA -5
#define BETA 5
#define DEPTH 7

uint8_t bestMove;

int negamax(board_t *game, int alpha, int beta, int depth){
    if(game->moves == WIDTH * HEIGHT){
        return 0;
    }

    if(gameOver(game)){
        return -1*(WIDTH * HEIGHT - game->moves / 2);
    }
    
    int max = (WIDTH * HEIGHT) - (game->moves / 2);
    if(beta > max){
        beta = max;
        if(alpha >= beta) return beta;
    }

    if(depth == 0){ 
       return 0; 
    }

    int i;
    for(i = 0; i < WIDTH; i++){
        if(canAdd(game, i)){
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
    negamax(&game, ALPHA, BETA, DEPTH);
    return bestMove;
}
