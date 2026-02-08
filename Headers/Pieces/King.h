#ifndef KING_H
#define KING_H

#include "Piece.h"

class King final : public Piece {
public:
    explicit King(const bool isWhite) : Piece(isWhite, PieceType::King, 1000) {}

    void movesInPosition(const Board &board, int8 row, int8 col, std::pmr::vector<Move> &allMoves) override;

    [[nodiscard]] std::pair<bool,Move> addShortCastle(bool canShortCastle) const;
    [[nodiscard]] std::pair<bool, Move> addLongCastle(bool canLongCastle) const;
};

#endif //KING_H
