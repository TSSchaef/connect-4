#include "game.h"

void init_board(board_t *game){
    int i, j;
    for(i = 0; i < WIDTH; i++){
        for(j = 0; j < HEIGHT; j++){
            game->board[i][j] = 0;
        }
    }
    game->currPlayer = 1;
}

board_t copy_board(board_t toCopy){
    int i, j;
    board_t this;
    for(i = 0; i < WIDTH; i++){
        for(j = 0; j < HEIGHT; j++){
            this.board[i][j] = toCopy.board[i][j];
        }
    }
    this.currPlayer = toCopy.currPlayer;
    return this;
}

int switchPlayer(board_t *game){
   game->currPlayer = (game->currPlayer == 1) ? 2 : 1;
   return game->currPlayer;
}

bool addChip(board_t *game, uint8_t column){
    uint8_t i;
    for(i = 0; i < HEIGHT; i++){
        if(game->board[column][i] == 0){
            game->board[column][i] = game->currPlayer;
            return true;
        }
    }
    return false;
}

uint8_t gameOver(board_t game, uint8_t column){
    return 0;
}
