#pragma once

#include <string_view>
#include <string>

#include "Castling.h"
#include "Piece.h"
#include "../fen/FenParser.h"
#include "../utils/BoardConfig.h"

/// \brief Represents standard 8×8 chess board
class Board {
    /// \brief Current position
    BoardGrid board{};
    /// \brief Player on move
    Color sideToMove = Color::White;
    /// \brief Castling rights
    CastlingRights castlingRights = CastlingRights::None;
    /// \brief enPassant target
    u8 enPassantTarget = board_config::NoSquare;

public:
    Board() = default;

    /// \brief Function that sets position on board from fen position
    /// \param segments FEN position divided into segments without whitespaces
    [[nodiscard]] std::expected<void, fen::FenError> loadFromFen(const fen::Segments& segments);

    /// \brief Function that sets position on board from fen position
    /// \param fenString Full chess position in FEN notation
    [[nodiscard]] std::expected<void, fen::FenError> loadFromFen(std::string_view fenString);

    [[nodiscard]] constexpr Piece pieceAt(const u8 square) const noexcept {
        return board[square];
    }
    [[nodiscard]] constexpr Piece pieceAt(const u8 rank, const u8 file) const noexcept {
        return board[board_config::squareIndex(rank, file)];
    }

    [[nodiscard]] std::string toString() const;
};