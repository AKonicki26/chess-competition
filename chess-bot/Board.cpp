//
// Created by anne.konicki on 3/29/2025.
//

#include "Board.h"

#include <iostream>

std::unordered_map<PieceType, std::function<std::vector<std::string>(Board*, PieceColor, uint8_t, uint8_t)> >
Board::moveFunctions = {
    {PieceType::PAWN, Board::pawnMove},
    {PieceType::KNIGHT, Board::knightMove},
    {PieceType::BISHOP, Board::bishopMove},
    {PieceType::ROOK, Board::rookMove},
    {PieceType::KING, Board::kingMove},
    {PieceType::QUEEN, Board::queenMove}
};

Board::Board() {
    setStartingBoard();
}

Board::Board(const std::string &fen) {
    resetBoard();

    // get an object representing the board as passed in from fen
    auto fenBoard = fenBoardFromString(fen);
    mColorTurn = fenBoard.whiteTurn ? PieceColor::WHITE : PieceColor::BLACK;
    mFullMove = fenBoard.fullMove;
    mHalfMove = fenBoard.halfMove;

    uint8_t rank = 7;
    uint8_t file = 0;

    for (const char c: fenBoard.pieceInfo) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (c >= '1' && c <= '8') {
            file += (c - '0');
        } else {
            Piece piece;
            if (c == 'P') piece = Piece(PieceType::PAWN, PieceColor::WHITE);
            else if (c == 'N') piece = Piece(PieceType::KNIGHT, PieceColor::WHITE);
            else if (c == 'B') piece = Piece(PieceType::BISHOP, PieceColor::WHITE);
            else if (c == 'R') piece = Piece(PieceType::ROOK, PieceColor::WHITE);
            else if (c == 'Q') piece = Piece(PieceType::QUEEN, PieceColor::WHITE);
            else if (c == 'K') piece = Piece(PieceType::KING, PieceColor::WHITE);
            else if (c == 'p') piece = Piece(PieceType::PAWN, PieceColor::BLACK);
            else if (c == 'n') piece = Piece(PieceType::KNIGHT, PieceColor::BLACK);
            else if (c == 'b') piece = Piece(PieceType::BISHOP, PieceColor::BLACK);
            else if (c == 'r') piece = Piece(PieceType::ROOK, PieceColor::BLACK);
            else if (c == 'q') piece = Piece(PieceType::QUEEN, PieceColor::BLACK);
            else if (c == 'k') piece = Piece(PieceType::KING, PieceColor::BLACK);

            setPiece(piece, rank, file);
            file++;
        }
    }
}

Piece Board::getPiece(const uint8_t rank, const uint8_t file) const {
    if (rank <= 8 && file <= 8)
        return mBoard[rank][file];
    return Piece();
}

void Board::setPiece(const Piece piece, uint8_t rank, uint8_t file) {
    if (rank <= 8 && file <= 8)
        mBoard[rank][file] = piece;
}


std::vector< std::string> Board::getValidMoves(PieceColor color) {
    std::vector<std::string> validMoves;

    auto getMovesForPiece = [&](const Piece piece, uint8_t rank, uint8_t file) -> std::vector<std::string> {
        if (piece.isEmpty())
            return {};
        return moveFunctions[piece.type](this, piece.color, rank, file);
    };

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            Piece currentPiece = getPiece(rank, file);
            if (currentPiece.color != color)
                continue;

            auto moves = getMovesForPiece(currentPiece, rank, file);
            // append the moves for the current piece onto the list of all moves
            validMoves.insert(validMoves.end(), moves.begin(), moves.end());
        }
    }

    return validMoves;
}

void Board::printBoard() const {
    std::cout << "  a b c d e f g h" << std::endl;
    
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << (rank + 1) << " ";
        
        for (int file = 0; file < 8; file++) {
            std::cout << mBoard[rank][file].toChar() << " ";
        }
        
        std::cout << (rank + 1) << std::endl;
    }
    
    std::cout << "  a b c d e f g h" << std::endl;
    
    // Print additional state information
    std::cout << (mColorTurn == PieceColor::WHITE ? "White" : "Black") << " to move" << std::endl;
    std::cout << "Half moves: " << static_cast<int>(mHalfMove) << std::endl;
    std::cout << "Full moves: " << static_cast<int>(mFullMove) << std::endl;
}

void Board::resetBoard() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            mBoard[rank][file] = Piece();
        }
    }

    // Reset game state
    mColorTurn = PieceColor::WHITE;
    mHalfMove = 0;
    mFullMove = 1;
}

void Board::setStartingBoard() {
    resetBoard();

    for (int file = 0; file < 8; file++) {
        setPiece(Piece(PieceType::PAWN, PieceColor::WHITE), 1, file);
        setPiece(Piece(PieceType::PAWN, PieceColor::BLACK), 6, file);
    }

    setPiece(Piece(PieceType::ROOK, PieceColor::WHITE), 0, 0);
    setPiece(Piece(PieceType::KNIGHT, PieceColor::WHITE), 0, 1);
    setPiece(Piece(PieceType::BISHOP, PieceColor::WHITE), 0, 2);
    setPiece(Piece(PieceType::QUEEN, PieceColor::WHITE), 0, 3);
    setPiece(Piece(PieceType::KING, PieceColor::WHITE), 0, 4);
    setPiece(Piece(PieceType::BISHOP, PieceColor::WHITE), 0, 5);
    setPiece(Piece(PieceType::KNIGHT, PieceColor::WHITE), 0, 6);
    setPiece(Piece(PieceType::ROOK, PieceColor::WHITE), 0, 7);

    setPiece(Piece(PieceType::ROOK, PieceColor::BLACK), 7, 0);
    setPiece(Piece(PieceType::KNIGHT, PieceColor::BLACK), 7, 1);
    setPiece(Piece(PieceType::BISHOP, PieceColor::BLACK), 7, 2);
    setPiece(Piece(PieceType::QUEEN, PieceColor::BLACK), 7, 3);
    setPiece(Piece(PieceType::KING, PieceColor::BLACK), 7, 4);
    setPiece(Piece(PieceType::BISHOP, PieceColor::BLACK), 7, 5);
    setPiece(Piece(PieceType::KNIGHT, PieceColor::BLACK), 7, 6);
    setPiece(Piece(PieceType::ROOK, PieceColor::BLACK), 7, 7);
}
