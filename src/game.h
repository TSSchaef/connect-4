#ifndef GAMEH
#define GAMEH

#include <stdint.h>
#include <stdbool.h>

#define WIDTH 7
#define HEIGHT 6

typedef struct board{
    uint8_t board[WIDTH][HEIGHT];
    uint8_t currPlayer;
} board_t;

//initialize as empty
void init_board(board_t *game);
board_t copy_board(board_t toCopy);
//switches current player - returns current player
int switchPlayer(board_t *game);
//does nothing if chip cannot be added
bool addChip(board_t *game, uint8_t column);
//returns player that won if a connect 4 was completed
//column is the last column where a chip was played
uint8_t gameOver(board_t game, uint8_t column);

#endif
