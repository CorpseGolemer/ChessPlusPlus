#pragma once

#include <array>
#include <cstdint>
#include <cassert>

using u8 = std::uint8_t;
using u64 = std::uint64_t;

using Piece = u8;
using BoardGrid = std::array<Piece, 64>;

namespace board_config {
    constexpr u8 SquareCount = 64;
    constexpr u8 RankCount = 8;
    constexpr u8 FileCount = 8;
    constexpr u8 NoSquare = 64;

    /// \brief Returns index of wanted square
    [[nodiscard]] constexpr u8 squareIndex(const u8 rank, const u8 file) noexcept{
        assert(rank < RankCount && file < FileCount && "Square index out of range!");
        return rank * FileCount + file;
    }
}
