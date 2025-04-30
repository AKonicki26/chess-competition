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

    std::string findBestMove(int depth);
    std::string getBestMove() const { return mBestMove;}
private:
    
    int minimaxAlgorithm(Board& board, int depth, bool maximizing, PieceColor color);
    Board* mpBoard;
    std::string mBestMove;
};



#endif //MINIMAX_H
