//
// Created by corps on 07.12.2025.
//

#include "../../Headers/Pieces/Knight.h"

void Knight::movesInPosition(const Board &board, const int8 row, const int8 col, std::pmr::vector<Move> &allMoves) {
    constexpr int8 moves[8][2] = {
        {2, 1}, // 2 do góry 1 w prawo
        {2, -1}, // 2 do góry 1 w lewo
        {-2, 1}, // 2 do dołu 1 w prawo
        {-2, -1}, // 2 do dołu 1 w lewo
        {1, 2}, // 2 w prawo 1 w górę
        {-1, 2}, // 2 w prawo 1 w dół
        {1, -2}, // 2 w lewo 1 w górę
        {-1, -2} // 2 w lewo 1 w dół
    };

    addSingleMoves(board, row, col, allMoves, moves, 8);
}

