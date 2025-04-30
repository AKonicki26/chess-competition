//
// Created by Anne Konicki on 4/30/2025.
//

#ifndef MINIMAX_H
#define MINIMAX_H
#include <string>

#include "Board.h"


class Minimax {
public:
    explicit Minimax(const std::string& boardFen) : mpBoard(new Board(boardFen)) {}

    std::string getBestMove(int depth);
private:
    
    int minimaxAlgorithm(const Board& board, int depth, bool maximizing, PieceColor color);
    Board* mpBoard;
};



#endif //MINIMAX_H
