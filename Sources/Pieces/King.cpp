#include "../../Headers/Pieces/King.h"
#include "../../Headers/Game/Board.h"

void King::movesInPosition(const Board &board, const int8 row, const int8 col, std::pmr::vector<Move> &allMoves) {
    constexpr int8 moves[8][2] {
        {1, 1}, // prawy górny
        {0, 1}, // w prawo
        {-1, 1}, // prawy dolny
        {-1,0}, // w dół
        {-1, -1}, // lewy dolny
        {0, -1}, // w lewo
        {1, -1}, // lewy górny
        {1, 0} // w górę
    };

    addSingleMoves(board, row, col, allMoves, moves, 8);
    if (auto [fst, snd] = addShortCastle(board.canCastleShort(this->isWhite)); fst) {
        allMoves.push_back(snd);
    }
    if (auto [fst, snd] = addLongCastle(board.canCastleLong(this->isWhite)); fst) {
        allMoves.push_back(snd);
    }

}

std::pair<bool, Move> King::addShortCastle(const bool canShortCastle) const {
    const int row = this->isWhite ? 0 : 7;
    if (canShortCastle) {
        Move move {{row,4}, {row,6}, CastleShort, PieceType::King};
        return {true, move};
    }
    Move move {};
    return {false, move};
}

std::pair<bool, Move> King::addLongCastle(const bool canLongCastle) const {
    const int row = this->isWhite ? 0 : 7;
    if (canLongCastle) {
        Move move {{row,4}, {row,2}, CastleLong, PieceType::King};
        return {true, move};
    }
    Move move {};
    return {false, move};
}

