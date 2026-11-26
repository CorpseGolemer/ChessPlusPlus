#pragma once

#include <string_view>
#include <array>
#include <charconv>
#include <expected>

#include "../game/Castling.h"
#include "../game/Piece.h"
#include "../utils/BoardConfig.h"

namespace fen {
    enum class FenError : u8 {
        // common
        InvalidSegmentLength,
        CharacterUndefined,
        InvalidInput,

        // parseSegments
        TooManySegments,
        NotEnoughSegments,

        // parsePosition
        TooManyRanks,
        NotEnoughRanks,
        AdjacentNumbers,
        RankOverflowed,
        RankUnderflowed,

        // parseCastlingRights
        WrongCastlingRightsLayout,

        // parseEnPassant
        WrongEnPassantFile,
        WrongEnPassantRank,
    };

    inline constexpr std::size_t SegmentCount = 6;
    inline constexpr std::size_t MinimalSegmentCount = 4;

    inline constexpr std::string_view StartingPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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

    namespace detail {
        [[nodiscard]] constexpr bool isAsciiSpace(const char c) noexcept {
            return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
        }
    }

    /// \brief Splits FEN into whitespace-separated segments
    /// \param fenString Chess position written in Forsyth–Edwards Notation
    /// \note Views inside Segments are valid only as long as fenString is alive
    [[nodiscard]] constexpr std::expected<Segments, FenError> parseSegments(const std::string_view fenString) noexcept {
        Segments seg;
        std::size_t start = 0;

        while (start < fenString.size()) {
            while (start < fenString.size() && detail::isAsciiSpace(fenString[start])) {
                ++start;
            }

            if (start >= fenString.size()) {
                break;
            }

            std::size_t end = start;
            while (end < fenString.size() && !detail::isAsciiSpace(fenString[end])) {
                ++end;
            }

            if (seg.count >= SegmentCount) {
                return std::unexpected(FenError::TooManySegments);
            }

            seg.fields[seg.count++] = fenString.substr(start, end - start);
            start = end;
        }

        if (seg.count < MinimalSegmentCount) {
            return std::unexpected(FenError::NotEnoughSegments);
        }
        return seg;
    }

    /// \brief Sets position on board from given fen segment
    /// \param placement Part of the fen containing info about piece placement
    [[nodiscard]] constexpr std::expected<BoardGrid, FenError> parsePosition(const std::string_view placement) noexcept {
        BoardGrid grid{};
        std::size_t start = 0;
        u8 rank = 7;

        while (start < placement.size()) {
            u8 file = 0;
            bool wasDigit = false;

            while (start < placement.size() && placement[start] != '/') {
                const char chr = placement[start];

                if (file >= 8) {
                    return std::unexpected(FenError::RankOverflowed);
                }

                if (const Piece p = piece::fromChar(chr); piece::isEmpty(p)) {
                    if (chr >= '1' && chr <= '8') {
                        if (wasDigit) {
                            return std::unexpected(FenError::AdjacentNumbers);
                        }
                        file += static_cast<u8>(chr - '0');
                        wasDigit = true;
                    }
                    else {
                        return std::unexpected(FenError::CharacterUndefined);
                    }
                }
                else {
                    grid[board_config::squareIndex(rank, file)] = p;
                    file++;
                    wasDigit = false;
                }
                start++;
            }
            if (file > 8) {
                return std::unexpected(FenError::RankOverflowed);
            }
            if (file < 8) {
                return std::unexpected(FenError::RankUnderflowed);
            }

            if (start >= placement.size()) break;
            if (rank == 0) { // if outside this loop would handle this situation
                return std::unexpected(FenError::TooManyRanks);
            }
            rank--;
            start++;
        }

        if (rank != 0) {
            return std::unexpected(FenError::NotEnoughRanks);
        }

        return grid;
    }

    /// \brief Parses active color segment into a Color
    [[nodiscard]] constexpr std::expected<Color, FenError> parseSideToMove(const std::string_view sideToMove) noexcept {
        if (sideToMove.size() != 1) {
            return std::unexpected(FenError::InvalidSegmentLength);
        }
        switch (sideToMove[0]) {
            case 'b': {
                return Color::Black;
            }
            case 'w': {
                return Color::White;
            }
            default: {
                return std::unexpected(FenError::CharacterUndefined);
            }
        }
    }

    /// \brief Parses castling rights segment into a CastlingRights
    [[nodiscard]] constexpr std::expected<CastlingRights, FenError> parseCastlingRights(const std::string_view castlingRights) noexcept {
        auto tempCastlingRights = CastlingRights::None;
        unsigned char lastChar = 0;
        std::size_t start = 0;

        if (castlingRights.empty()) {
            return std::unexpected(FenError::InvalidSegmentLength);
        }

        while (start < castlingRights.size()) {
            const char chr = castlingRights[start];
            const CastlingRights rights = castling::fromChar(chr);

            if (rights == CastlingRights::None) {
                if (start == 0 && chr == '-' && castlingRights.size() == 1) {
                    return CastlingRights::None;
                }
                return std::unexpected(FenError::CharacterUndefined);
            }
            if (chr > lastChar) { // This condition ensures that the order is maintained and there are no duplicates
                lastChar = chr;
                tempCastlingRights = castling::with(tempCastlingRights, rights);
            }
            else {
                return std::unexpected(FenError::WrongCastlingRightsLayout);
            }
            start++;
        }

        return tempCastlingRights;
    }

    /// \brief Parses enPassant segment into a square
    [[nodiscard]] constexpr std::expected<u8, FenError> parseEnPassant(const std::string_view enPassant) noexcept {
        if (enPassant.empty()) {
            return std::unexpected(FenError::InvalidSegmentLength);
        }
        if (enPassant.size() == 1) {
            if (enPassant[0] == '-') {
                return board_config::NoSquare;
            }
            return std::unexpected(FenError::CharacterUndefined);
        }
        if (enPassant.size() == 2) {
            const char file = enPassant[0];
            const char rank = enPassant[1];
            if (file < 'a' || file > 'h') {  // Only small letters from a to h
                return std::unexpected(FenError::WrongEnPassantFile);
            }
            if (rank != '3' && rank != '6') { // Only 3rd and 6th rank are acceptable
                return std::unexpected(FenError::WrongEnPassantRank);
            }
            return board_config::squareIndex(static_cast<u8>(rank - '1'), static_cast<u8>(file - 'a'));
        }
        return std::unexpected(FenError::InvalidSegmentLength);
    }

    /// \brief Parses halfMove segment into a number
    [[nodiscard]] constexpr std::expected<u64, FenError> parseHalfMove(const std::string_view halfMove) noexcept {
        if (halfMove.empty()) {
            return 0;
        }

        u64 temp = 0;
        if (const auto [ptr, err]
            = std::from_chars(halfMove.data(), halfMove.data() + halfMove.size(), temp);
            err != std::errc() || ptr != halfMove.data() + halfMove.size()) {

            return std::unexpected(FenError::InvalidInput);
            }
        return temp;
    }

    /// \brief Parses fullMove segment into a number
    [[nodiscard]] constexpr std::expected<u64, FenError> parseFullMove(const std::string_view fullMove) noexcept {
        if (fullMove.empty()) {
            return 1;
        }

        u64 temp = 0;
        if (const auto [ptr, err]
            = std::from_chars(fullMove.data(), fullMove.data() + fullMove.size(), temp);
            err != std::errc() || ptr != fullMove.data() + fullMove.size()) {

            return std::unexpected(FenError::InvalidInput);
            }
        if (temp == 0) {
            return std::unexpected(FenError::InvalidInput);
        }
        return temp;
    }
}
