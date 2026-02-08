#include "../../Headers/Pieces/Rook.h"

void Rook::movesInPosition(const Board & board, const int8 row, const int8 col, std::pmr::vector<Move> & allMoves) {
    constexpr int8 directions[4][2] = {
        {1,0},  // ruch w górę
        {-1, 0}, // ruch w dół
        {0,1}, // ruch w prawo
        {0,-1} // ruch w lewo
    };

    addSlidingMoves(board, row,  col, allMoves, directions, 4);
}



