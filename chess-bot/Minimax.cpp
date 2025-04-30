//
// Created by Anne Konicki on 4/30/2025.
//

#include "Minimax.h"

// look depth moves deep to find the best possible move for the current player
std::string Minimax::getBestMove(int depth)
{
    std::string bestMove;

    int bestValue = INT_MIN;

    auto startingColor = mpBoard->getCurrentColor();
    auto legalMoves = mpBoard->getValidMoves(startingColor);

    for (const auto& move : legalMoves)
    {
        auto nextBoard = mpBoard->makeMove(move);
        int boardScore = minimaxAlgorithm(nextBoard, depth - 1, false, oppositeColor(startingColor));
        if (boardScore > bestValue)
        {
            bestValue = boardScore;
            bestMove = move;
        }
    }

    return bestMove;
}

int Minimax::minimaxAlgorithm(const Board& board, int depth, bool maximizing, PieceColor color)
{
    if (depth == 0)
        return board.evaluate(color);

    auto legalMoves = board.getValidMoves(color);

    // getting to a position where we have no moves is baaaaaad. No stalemates :(
    if (legalMoves.empty())
        return INT_MIN;
    
    int bestValue = maximizing ? INT_MIN : INT_MAX;

    for (const auto& move : legalMoves)
    {
        Board nextBoard = board.makeMove(move);
        int boardScore = minimaxAlgorithm(nextBoard, depth - 1, !maximizing, oppositeColor(color));

        if (maximizing)
            bestValue = std::max(bestValue, boardScore);
        else
            bestValue = std::min(bestValue, boardScore);
    }

    return bestValue;
    
}
