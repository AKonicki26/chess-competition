//
// Created by thecr on 3/18/2025.
//

#ifndef CHESS_COMPETITION_BOARD_H
#define CHESS_COMPETITION_BOARD_H

#include <string>
#include <vector>
#include <sstream>
#include <ranges>
#include <string_view>
#include <atomic>
#include <functional>
#include <unordered_map>

enum class PieceType: uint8_t {
    EMPTY = 0b000,
    PAWN = 0b001,
    KNIGHT = 0b010,
    BISHOP = 0b011,
    ROOK = 0b100,
    QUEEN = 0b101,
    KING = 0b110
};

enum class PieceColor: uint8_t {
    BLACK = 0b0,
    WHITE = 0b1
};

struct Piece {
    PieceType type: 3;
    PieceColor color: 1;

    Piece() : type(PieceType::EMPTY), color(PieceColor::WHITE) {
    }

    Piece(PieceType type, PieceColor color) : type(type), color(color) {
    }

    bool isEmpty() const { return type == PieceType::EMPTY; }

    char toChar() const {
        if (isEmpty()) return '.';

        char c;
        switch (type) {
            case PieceType::PAWN: c = 'p';
                break;
            case PieceType::KNIGHT: c = 'n';
                break;
            case PieceType::BISHOP: c = 'b';
                break;
            case PieceType::ROOK: c = 'r';
                break;
            case PieceType::QUEEN: c = 'q';
                break;
            case PieceType::KING: c = 'k';
                break;
            default: return '.';
        }

        return (color == PieceColor::WHITE) ? toupper(c) : c;
    }
};

struct FenBoard {
    std::string pieceInfo;
    bool whiteTurn = true;
    std::string castling;
    std::string enPassant;
    int halfMove = 0;
    int fullMove = 0;
};

inline FenBoard fenBoardFromString(std::string fen) {
    FenBoard fenSplit;
    std::vector<std::string> segments;
    // split the fen string on space
    for (const auto segment: std::views::split(fen, ' '))
        segments.emplace_back(std::string_view(segment));

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
    Board();

    Board(const std::string &fen);

    Piece getPiece(const uint8_t rank, const uint8_t file) const;

    Piece getPiece(const std::string &square) const;

    void setPiece(const Piece piece, uint8_t rank, uint8_t file);

    void setPiece(const Piece piece, const std::string &square);

    std::vector<std::string> getValidMoves(PieceColor color);

    PieceColor getCurrentColor() const { return mColorTurn; };

private:
    PieceColor mColorTurn = PieceColor::WHITE;
    uint8_t mHalfMove;
    uint8_t mFullMove;
    std::array<std::array<Piece, 8>, 8> mBoard;

    // TODO: Add castling and en passant.
    // Currently the AI will not know these are things in the game at all,
    // will likely lost to other AIs capable of doing these as it will not
    // predict the moves

    // Clear the board (set all squares to empty)
    void resetBoard();

    void setStartingBoard();

    static bool algebraicToCoords(const std::string &algebraic, int &rank, int &file) {
        if (algebraic.length() != 2) return false;

        file = algebraic[0] - 'a';
        rank = algebraic[1] - '1';

        return (rank >= 0 && rank < 8 && file >= 0 && file < 8);
    }

    // Convert rank/file to algebraic notation
    static std::string coordsToAlgebraic(int rank, int file) {
        if (rank < 0 || rank >= 8 || file < 0 || file >= 8) {
            return "??";
        }

        return std::string(1, 'a' + file) + std::string(1, '1' + rank);
    }


    // Pawn move function
static std::vector<std::string> pawnMove(Board* board, PieceColor color, uint8_t rank, uint8_t file) {
    std::vector<std::string> possibleMoves;

    int rankMovementDirection = (color == PieceColor::WHITE) ? 1 : -1;
    
    // Check for boundary conditions first
    if ((color == PieceColor::WHITE && rank == 7) || (color == PieceColor::BLACK && rank == 0)) {
        // Pawn is already at the end of the board, can't move further
        return possibleMoves;
    }
    
    // Single square forward move
    int newRank = rank + rankMovementDirection;
    if (newRank >= 0 && newRank < 8) {  // Ensure new rank is on board
        if (board->getPiece(newRank, file).isEmpty()) {
            // Check if it's a promotion move
            if (newRank == 7 || newRank == 0) {
                // Add promotion moves (queen, rook, bishop, knight)
                possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, file) + "q");
                possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, file) + "r");
                possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, file) + "b");
                possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, file) + "n");
            } else {
                // Regular move
                possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, file));
                
                // Two square move from starting position
                if ((color == PieceColor::WHITE && rank == 1) || (color == PieceColor::BLACK && rank == 6)) {
                    int twoSquareRank = rank + (2 * rankMovementDirection);
                    if (board->getPiece(twoSquareRank, file).isEmpty()) {
                        possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(twoSquareRank, file));
                    }
                }
            }
        }
    }
    
    // Captures to the left and right
    for (int fileOffset : {-1, 1}) {
        int newFile = file + fileOffset;
        if (newFile >= 0 && newFile < 8) {  // Ensure new file is on board
            Piece targetPiece = board->getPiece(newRank, newFile);
            if (!targetPiece.isEmpty() && targetPiece.color != color) {
                // It's an enemy piece, can capture
                if (newRank == 7 || newRank == 0) {
                    // Capture with promotion
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile) + "q");
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile) + "r");
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile) + "b");
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile) + "n");
                } else {
                    // Regular capture
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile));
                }
            }
        }
    }
    
    // TODO: En passant
    
    return possibleMoves;
}

    // Knight move function
    static std::vector<std::string> knightMove(Board* board, PieceColor color, uint8_t rank, uint8_t file) {
        std::vector<std::string> possibleMoves;

        // All possible knight moves (L-shapes)
        const int knightOffsets[8][2] = {
            {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
            {1, -2}, {1, 2}, {2, -1}, {2, 1}
        };

        for (const auto &offset: knightOffsets) {
            int newRank = rank + offset[0];
            int newFile = file + offset[1];

            // Check if the new position is on the board
            if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                // Check if the square is empty or has an enemy piece
                Piece targetPiece = board->getPiece(newRank, newFile);
                if (targetPiece.isEmpty() || targetPiece.color != color) {
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile));
                }
            }
        }

        return possibleMoves;
    }

    // Bishop move function
    static std::vector<std::string> bishopMove(Board* board, PieceColor color, uint8_t rank, uint8_t file) {
        std::vector<std::string> possibleMoves;

        // All diagonal directions
        const int directions[4][2] = {
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };

        for (const auto &dir: directions) {
            int rankDir = dir[0];
            int fileDir = dir[1];

            for (int i = 1; i < 8; i++) {
                int newRank = rank + i * rankDir;
                int newFile = file + i * fileDir;

                // Check if the new position is on the board
                if (newRank < 0 || newRank >= 8 || newFile < 0 || newFile >= 8) {
                    break; // Out of bounds
                }

                Piece targetPiece = board->getPiece(newRank, newFile);
                if (targetPiece.isEmpty()) {
                    // Empty square, can move here
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile));
                } else if (targetPiece.color != color) {
                    // Enemy piece, can capture and then stop
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile));
                    break;
                } else {
                    // Own piece, can't move here or beyond
                    break;
                }
            }
        }

        return possibleMoves;
    }

    // Rook move function
    static std::vector<std::string> rookMove(Board* board, PieceColor color, uint8_t rank, uint8_t file) {
        std::vector<std::string> possibleMoves;

        // All orthogonal directions
        const int directions[4][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}
        };

        for (const auto &dir: directions) {
            int rankDir = dir[0];
            int fileDir = dir[1];

            for (int i = 1; i < 8; i++) {
                int newRank = rank + i * rankDir;
                int newFile = file + i * fileDir;

                // Check if the new position is on the board
                if (newRank < 0 || newRank >= 8 || newFile < 0 || newFile >= 8) {
                    break; // Out of bounds
                }

                Piece targetPiece = board->getPiece(newRank, newFile);
                if (targetPiece.isEmpty()) {
                    // Empty square, can move here
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile));
                } else if (targetPiece.color != color) {
                    // Enemy piece, can capture and then stop
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile));
                    break;
                } else {
                    // Own piece, can't move here or beyond
                    break;
                }
            }
        }

        return possibleMoves;
    }

    // Queen move function (combines bishop and rook)
    static std::vector<std::string> queenMove(Board* board, PieceColor color, uint8_t rank, uint8_t file) {
        std::vector<std::string> possibleMoves;

        // All 8 directions (combinations of orthogonal and diagonal)
        const int directions[8][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}, // Rook-like moves
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1} // Bishop-like moves
        };

        for (const auto &dir: directions) {
            int rankDir = dir[0];
            int fileDir = dir[1];

            for (int i = 1; i < 8; i++) {
                int newRank = rank + i * rankDir;
                int newFile = file + i * fileDir;

                // Check if the new position is on the board
                if (newRank < 0 || newRank >= 8 || newFile < 0 || newFile >= 8) {
                    break; // Out of bounds
                }

                Piece targetPiece = board->getPiece(newRank, newFile);
                if (targetPiece.isEmpty()) {
                    // Empty square, can move here
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile));
                } else if (targetPiece.color != color) {
                    // Enemy piece, can capture and then stop
                    possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile));
                    break;
                } else {
                    // Own piece, can't move here or beyond
                    break;
                }
            }
        }

        return possibleMoves;
    }

    // King move function
    static std::vector<std::string> kingMove(Board* board, PieceColor color, uint8_t rank, uint8_t file) {
        std::vector<std::string> possibleMoves;

        // All 8 directions for one square
        for (int rankOffset = -1; rankOffset <= 1; rankOffset++) {
            for (int fileOffset = -1; fileOffset <= 1; fileOffset++) {
                // Skip the current position
                if (rankOffset == 0 && fileOffset == 0) continue;

                int newRank = rank + rankOffset;
                int newFile = file + fileOffset;

                // Check if the new position is on the board
                if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                    // Check if the square is empty or has an enemy piece
                    Piece targetPiece = board->getPiece(newRank, newFile);
                    if (targetPiece.isEmpty() || targetPiece.color != color) {
                        possibleMoves.emplace_back(coordsToAlgebraic(rank, file) + coordsToAlgebraic(newRank, newFile));
                    }
                }
            }
        }

        return possibleMoves;
    }

    // Map that represents a function for each type of piece
    static std::unordered_map<
        PieceType,
        std::function<
            std::vector<std::string>
            (Board* board, PieceColor, uint8_t, uint8_t)
        >
    > moveFunctions;
};


#endif //CHESS_COMPETITION_BOARD_H
