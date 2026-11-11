#pragma once

#include "Types.h"

namespace board_config {
    constexpr u8 SquareCount = 64;
    constexpr u8 RankCount = 8;
    constexpr u8 FileCount = 8;
    constexpr u8 NoSquare = SquareCount;

    /// \brief Returns index of wanted square
    [[nodiscard]] constexpr u8 squareIndex(const u8 rank, const u8 file) noexcept{
        assert(rank < RankCount && file < FileCount && "Square index out of range!");
        return static_cast<u8>(rank * FileCount + file);
    }
}

using BoardGrid = std::array<Piece, board_config::SquareCount>;