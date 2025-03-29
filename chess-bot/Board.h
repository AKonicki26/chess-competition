//
// Created by thecr on 3/18/2025.
//

#ifndef CHESS_COMPETITION_BOARD_H
#define CHESS_COMPETITION_BOARD_H

#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <ranges>
#include <string_view>

struct FenBoard {
    std::string pieceInfo;
    bool whiteTurn =  true;
    std::string castling;
    std::string enPassant;
    int halfMove = 0;
    int fullMove = 0;
};

FenBoard fenBoardFromString(std::string fen) {
    FenBoard fenSplit;
    std::vector<std::string> segments;
    // split the fen string on space
    for (const auto segment : std::views::split(fen, ' '))
        segments.emplace_back(std::string_view (segment));

    fenSplit.pieceInfo = segments[0];
    fenSplit.whiteTurn = segments[1] == "w";
    fenSplit.castling = segments[2];
    fenSplit.enPassant = segments[3];
    fenSplit.halfMove = std::stoi(segments[4]);
    fenSplit.fullMove = std::stoi(segments[5]);

    return fenSplit;
}

class Board {
public:
    Board(std::string fen) {
        // get an object representing the board as passed in from fen
        auto fenBoard = fenBoardFromString(std::move(fen));




    }
};


#endif //CHESS_COMPETITION_BOARD_H