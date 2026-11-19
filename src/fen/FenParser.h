#pragma once

#include <string_view>
#include <array>

#include "../game/Castling.h"
#include "../game/Piece.h"
#include "../utils/BoardConfig.h"

namespace fen {
    constexpr std::size_t SegmentCount = 6;
    constexpr std::size_t MinimalSegmentCount = 4;

    struct Segments {
        std::array<std::string_view, SegmentCount> fields{};
        std::size_t count = 0;

        [[nodiscard]] constexpr std::string_view placement() const noexcept { return fields[0]; }
        [[nodiscard]] constexpr std::string_view sideToMove() const noexcept { return fields[1]; }
        [[nodiscard]] constexpr std::string_view castlingRights() const noexcept { return fields[2]; }
        [[nodiscard]] constexpr std::string_view enPassant() const noexcept { return fields[3]; }
        [[nodiscard]] constexpr std::string_view halfMove() const noexcept { return fields[4]; }
        [[nodiscard]] constexpr std::string_view fullMove() const noexcept { return fields[5]; }
    };

    /// \brief Splits FEN into whitespace-separated segments
    /// \param fenString Chess position written in Forsyth–Edwards Notation
    /// \param out Struct to place parsed fragments
    /// \note Views inside Segments are valid only as long as fenString is alive
    [[nodiscard]] bool parseSegments(std::string_view fenString, Segments& out);

    /// \brief Sets position on board from given fen segment
    /// \param placement Part of the fen containing info about piece placement
    /// \param out Board to set position on
    [[nodiscard]] bool parsePosition(std::string_view placement, BoardGrid& out);

    /// \brief Parses active color segment into a Color
    [[nodiscard]] bool parseSideToMove(std::string_view sideToMove, Color& out);

    /// \brief Parses castling rights segment into a CastlingRights
    [[nodiscard]] bool parseCastlingRights(std::string_view castlingRights, CastlingRights& out);

    /// \brief Parses enPassant segment into a square
    [[nodiscard]] bool parseEnPassant(std::string_view enPassant, u8& out);

    /// \brief Parses halfMove segment into a number
    [[nodiscard]] bool parseHalfMove(std::string_view halfMove, u64& out);

    /// \brief Parses fullMove segment into a number
    [[nodiscard]] bool parseFullMove(std::string_view fullMove, u64& out);
}
