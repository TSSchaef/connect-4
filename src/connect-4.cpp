#include <cstdint>

#define WIDTH 7
#define HEIGHT 6

class board{
public:
    uint8_t board[WIDTH][HEIGHT];
    uint8_t currPlayer;

    board(){
        //initialize as empty
        int i, j;
        for(i = 0; i < WIDTH; i++){
            for(j = 0; j < HEIGHT; j++){
                board[i][j] = 0;
            }
        }
        currPlayer = 1;
    }

    board(board &toCopy){
        int i, j;
        for(i = 0; i < WIDTH; i++){
            for(j = 0; j < HEIGHT; j++){
                this->board[i][j] = toCopy.board[i][j];
            }
        }
        this->currPlayer = toCopy.currPlayer;
    }

    ~board(){}
    
    //switches current player - returns current player
    int switchPlayer(){
       currPlayer = (currPlayer == 1) ? 2 : 1;
       return currPlayer;
    }

    //does nothing if chip cannot be added
    bool addChip(uint8_t column){
        uint8t i;
        for(i = 0; i < HEIGHT; i++){
            if(board[column][i] == 0){
                board[column][i] = currPlayer;
                return true;
            }
        }
        return false;
    }

    //returns if a connect 4 was completed
    //column is the last column where a chip was played
    uint8_t gameOver(uint8_t column){
        return 0;
    }
};


int main(int argc, char *argv[]){

    return 0;
}
