
#ifndef BISHOP_H
#define BISHOP_H
#include "Piece.h"
#include "../App.h"

class Bishop final : public Piece {
public:
    explicit Bishop(const bool isWhite) : Piece(isWhite, PieceType::Bishop, 3) {}

    void movesInPosition(const Board &board, int8 row, int8 col, std::pmr::vector<Move> &allMoves) override;
};

#endif //BISHOP_H
