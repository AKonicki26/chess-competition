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
#include <atomic>

struct FenBoard {
    std::string pieceInfo;
    bool whiteTurn =  true;
    std::string castling;
    std::string enPassant;
    int halfMove = 0;
    int fullMove = 0;
};

enum class PieceType: uint8_t {
    EMPTY  = 0b000,
    PAWN   = 0b001,
    KNIGHT = 0b010,
    BISHOP = 0b011,
    ROOK   = 0b100,
    QUEEN  = 0b101,
    KING   = 0b110
};

enum class PieceColor: uint8_t {
    BLACK = 0b0,
    WHITE = 0b1
};

struct Piece
{
    PieceType type:3;
    PieceColor color:1;
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
        mWhiteTurn = fenBoard.whiteTurn;
        mFullMove = fenBoard.fullMove;
        mHalfMove = fenBoard.halfMove;

        // TODO: Turn piece info into the mPieces array

    }
private:
    bool mWhiteTurn = true;
    uint8_t mHalfMove;
    uint8_t mFullMove;
    Piece mPieces[64];

    // TODO: Add castling and en passant.
    // Currently the AI will not know these are things in the game at all,
    // will likely lost to other AIs capable of doing these as it will not
    // predict the moves
};


#endif //CHESS_COMPETITION_BOARD_H