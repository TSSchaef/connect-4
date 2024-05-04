#include <cstdint>

#define WIDTH 7
#define HEIGHT 6

class board{
private:
    uint8_t baord[WIDTH][HEIGHT];
    uint8_t currPlayer;

public:
    board(){
        //initialize as empty
    }
    ~board(){}

    bool addChip(uint8_t column){
        return true;
    }

    uint8_t gameOver(uint8_t column){
        return 0;
    }
};


int main(int argc, char *argv[]){

    return 0;
}
