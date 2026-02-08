//
// Created by corps on 09.12.2025.
//

#ifndef PAWN_H
#define PAWN_H
#include "Piece.h"

class Pawn final : public Piece {
public:
    // Pomocniczy bool, który informuje czy pion wykonał już ruch
    bool notMoved = true;

    explicit Pawn(const bool isWhite) : Piece(isWhite, PieceType::Pawn, 1) {}


    void movesInPosition(const Board &board, int8 row, int8 col, std::pmr::vector<Move> &allMoves) override;
    void setNotMoved(const bool notMoved) override {this->notMoved = notMoved;}
    [[nodiscard]] bool getNotMoved() const override {return notMoved;}
};

#endif //PAWN_H
