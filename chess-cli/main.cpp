#include <iostream>

#include "chess-simulator.h"
#include <string>

#include "Board.h"

int main() {
    Board board;
    board.printBoard();
    std::cout << "\n";
    
    for (auto move : board.getValidMoves(board.getCurrentColor()))
        std::cout << move << "\n";

    board = Board("4k2r/6r1/8/8/8/8/3R4/R3K3 w Qk - 0 1");
    std::cout << "\n";
    board.printBoard();
    std::cout << "\n";
    
    for (auto move : board.getValidMoves(board.getCurrentColor()))
        std::cout << move << "\n";
}
