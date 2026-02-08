//
// Created by corps on 07.12.2025.
//

#ifndef QUEEN_H
#define QUEEN_H
#include "Piece.h"

class Queen final : public Piece {
public:
    explicit Queen(const bool isWhite) : Piece(isWhite, PieceType::Queen, 9) {}

    void movesInPosition(const Board &board, int8 row, int8 col, std::pmr::vector<Move> &allMoves) override;
};

#endif //QUEEN_H
