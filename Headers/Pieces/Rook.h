#ifndef ROOK_H
#define ROOK_H

#include "Piece.h"

// Klasa pochodna dziedziczy po Piece, final oznacza, że po tej klasie już nie można dziedziczyć
class Rook final : public Piece{
public:
    // Konstruktor klasy jest explicit, ponieważ przyjmuje tylko jeden argument
    explicit Rook(const bool isWhite)
    : Piece(isWhite, PieceType::Rook, 5) {}

    // Implementacja wirtualnej metody z klasy bazowej Piece
    void movesInPosition(const Board & board, int8 row, int8 col, std::pmr::vector<Move> & allMoves) override;
};



#endif //ROOK_H
