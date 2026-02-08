//
// Created by corps on 07.12.2025.
//

#ifndef KNIGHT_H
#define KNIGHT_H
#include "Piece.h"

class Knight final : public Piece {
public:
    explicit Knight(const bool isWhite) : Piece(isWhite, PieceType::Knight, 3) {}

    void movesInPosition(const Board &board, int8 row, int8 col, std::pmr::vector<Move> &allMoves) override;
};

#endif //KNIGHT_H
