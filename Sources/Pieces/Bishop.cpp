//
// Created by corps on 07.12.2025.
//

#include "../../Headers/Pieces/Bishop.h"

void Bishop::movesInPosition(const Board &board, const int8 row, const int8 col, std::pmr::vector<Move> &allMoves) {
    constexpr int8 directions[4][2] = {
        {1, -1}, // lewy górny
        {1, 1}, // prawy górny
        {-1, -1}, // lewy dolny
        {-1, 1} // prawy dolny
    };

    addSlidingMoves(board, row, col, allMoves, directions, 4);
}

