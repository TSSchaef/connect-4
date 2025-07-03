#include "solver.h"
#include <stdio.h>

#define ALPHA -1
#define BETA 1
#define DEPTH 20

uint8_t evalOrder[WIDTH];

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
            return ((WIDTH * HEIGHT) + 1 - (game->moves / 2));
        }
    }
    
    int max = (WIDTH * HEIGHT) - 1 - (game->moves / 2);
    if(beta > max){
        beta = max;
        if(alpha >= beta) return beta;
    }
    
    int bestScore = -(WIDTH * HEIGHT);
    for(i = 0; i < WIDTH; i++){
        if(canAdd(game, evalOrder[i])){
            //if(bestMove > WIDTH) bestMove = i;
            board_t game2;
            copy(&game2, game);

            addChip(&game2, evalOrder[i]);
            int8_t score;
            if(depth > 0){
                score = get(key(game2));
                if(score > WIDTH * HEIGHT){
                    score = -negamax(&game2, -beta, -alpha, depth - 1);
                    put(key(game2), score);
                }
            } else {
                score = 0;
            }
            if(score >= beta){
                if(depth == DEPTH){
                    bestMove = evalOrder[i];
                }
                return score;
            } 
            if(score > alpha) {
                if(depth == DEPTH){
                    bestMove = evalOrder[i];
                }
                alpha = score;
            } else if(score > bestScore && depth == DEPTH && bestMove > WIDTH){
                bestScore = score;
                bestMove = evalOrder[i];
            }
        }
    }

    return alpha;
}

int computerInput(board_t game){
    numEvaluated = 0;
    bestMove = WIDTH + 1;
    int score = negamax(&game, ALPHA, BETA, DEPTH);
    printf("Evaluated: %ld\nScore: %d\n", numEvaluated, score);
    return bestMove;
}

void init_opponent(){
    init_table();
    int i;
    for(i = 0; i < WIDTH; i++){
        evalOrder[i] = WIDTH / 2 + (1 - 2*(i % 2))*(i + 1) / 2;
    }
}
