#pragma once

#include "Types.h"
#include <cassert>

/// \brief Holds piece types
enum class PieceType : u8 {
    None = 0,
    Pawn = 1,
    Knight = 2,
    Bishop = 3,
    Rook = 4,
    Queen = 5,
    King = 6,
};

/// \brief Holds piece colors
enum class Color : u8 {
    White = 0,
    Black = 8
};

/// \brief Namespace containing piece centered methods and variables
namespace piece {
    constexpr u8 TypeMask = 7; // 0000 0111
    constexpr u8 ColorMask = 8; // 0000 1000

    /// \brief Returns index of piece with given parameters
    inline u8 create(const PieceType type, const Color color) {
        assert(type <= PieceType::King && "Attempted creating piece with invalid PieceType!");
        assert((color == Color::White || color == Color::Black) && "Attempted creating piece with invalid Color!");
        return static_cast<u8>(type) | static_cast<u8>(color);
    }

    inline PieceType getType(const u8 piece) {
        const u8 type = piece & TypeMask;
        assert(type <= static_cast<u8>(PieceType::King) && "Piece corrupted! Invalid PieceType!");
        return static_cast<PieceType>(type);
    } 

    inline bool isEmpty(const u8 piece) {
        return (piece & TypeMask) == static_cast<u8>(PieceType::None);
    }

    /// \brief Checks if given piece color matches the wanted
    /// \param piece piece to check the color
    /// \param color this color is compared with piece’s color
    inline bool isSameColor(const u8 piece, const Color color) {
        return !isEmpty(piece) && (piece & ColorMask) == static_cast<u8>(color);
    }

    /// \brief Checks if two given pieces match by color
    inline bool isSameColor(const u8 piece1, const u8 piece2) {
        return !isEmpty(piece1) && !isEmpty(piece2) && (piece1 & ColorMask) == (piece2 & ColorMask);
    }

    /// \brief Returns opposite color for a given color
    inline Color flipColor(const Color color) {
        return static_cast<Color>(static_cast<u8>(color) ^ ColorMask);
    }
}
