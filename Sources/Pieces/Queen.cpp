//
// Created by corps on 07.12.2025.
//

#include "../../Headers/Pieces/Queen.h"

void Queen::movesInPosition(const Board &board, const int8 row, const int8 col, std::pmr::vector<Move> &allMoves) {
    constexpr int8 directions[8][2] = {
        {1, 0}, // w górę
        {-1, 0}, // w dół
        {0, 1}, // w prawo
        {0, -1}, // w lewo
        {1, -1}, // lewy górny
        {1, 1}, // prawy górny
        {-1,- 1}, // lewy dolny
        {-1, 1} // prawy dolny
    };

    addSlidingMoves(board, row, col, allMoves, directions, 8);
}
