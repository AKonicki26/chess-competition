//
// Created by Anne Konicki on 4/30/2025.
//

#include "Minimax.h"

#include <thread>
#include <mutex>

// look depth moves deep to find the best possible move for the current player
std::string Minimax::findBestMove(int depth)
{
    std::string bestMove;

    int bestValue = INT_MIN;

    auto startingColor = mpBoard->getCurrentColor();
    auto legalMoves = mpBoard->getValidMoves(startingColor);

    std::mutex mutex;
    std::vector<std::thread> threads;

    for (const auto& move : legalMoves)
    {
        threads.emplace_back([&, move]()
        {
            Board localBoard = *mpBoard;
            auto undoInfo = localBoard.makeMove(move);
            int boardScore = minimaxAlgorithm(localBoard, depth - 1, false, oppositeColor(startingColor));

            {
                std::lock_guard<std::mutex> lock(mutex);
                if (boardScore > bestValue)
                {
                    bestValue = boardScore;
                    bestMove = move;
                }
            }
        });
    }

    // wait for all of our threads to finish
    for (auto& thread : threads)
        thread.join();

    mBestMove = bestMove;
    return bestMove;
}

int Minimax::minimaxAlgorithm(Board& board, int depth, bool maximizing, PieceColor color)
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
        
        auto undoInfo = board.makeMove(move);
        
        int boardScore = minimaxAlgorithm(board, depth - 1, !maximizing, oppositeColor(color));

        board.undoMove(undoInfo);
        
        if (maximizing)
            bestValue = std::max(bestValue, boardScore);
        else
            bestValue = std::min(bestValue, boardScore);
        
    }

    
    return bestValue;
    
}
