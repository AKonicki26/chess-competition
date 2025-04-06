#include <iostream>

#include "chess-simulator.h"
#include <string>

#include "Board.h"

int main() {
    Board board;
    auto moves = board.getValidMoves(board.getCurrentColor());

    board.printBoard();

    std::cout << "\n";

    for (auto move : moves) {
        std::cout << move << "\n";
    }
}
