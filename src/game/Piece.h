#pragma once

#include "../utils/Types.h"

/// \brief Holds piece types
enum class PieceType : u8 {
    None = 0b0000'0000,
    Pawn = 0b0000'0001,
    Knight = 0b0000'0010,
    Bishop = 0b0000'0011,
    Rook = 0b0000'0100,
    Queen = 0b0000'0101,
    King = 0b0000'0110
};

/// \brief Holds piece colors
enum class Color : u8 {
    White = 0b0000'0000,
    Black = 0b0000'1000
};

/// \brief Namespace containing piece centered methods and variables
namespace piece {
    constexpr u8 TypeMask = 0b0000'0111;
    constexpr u8 ColorMask = 0b0000'1000;

    static_assert(static_cast<u8>(Color::Black) == ColorMask, "Color::Black value MUST match piece::ColorMask!");
    static_assert(static_cast<u8>(PieceType::King) < TypeMask, "PieceType::King value MUST be less than piece::TypeMask!");

    /// \brief Returns value of piece with given parameters
    [[nodiscard]] constexpr Piece create(const PieceType type, const Color color) noexcept {
        assert(type <= PieceType::King && "Attempted creating piece with invalid PieceType!");
        assert((color == Color::White || color == Color::Black) && "Attempted creating piece with invalid Color!");
        return static_cast<u8>(type) | static_cast<u8>(color);
    }

    [[nodiscard]] constexpr bool isEmpty(const Piece pc) noexcept {
        return (pc & TypeMask) == static_cast<u8>(PieceType::None);
    }

    [[nodiscard]] constexpr PieceType getType(const Piece pc) noexcept {
        const u8 type = pc & TypeMask;
        assert(type <= static_cast<u8>(PieceType::King) && "Piece corrupted! Invalid PieceType!");
        return static_cast<PieceType>(type);
    }

    /// \warning Debug: assert. Release: undefined result for an empty square
    [[nodiscard]] constexpr Color getColor(const Piece pc) noexcept {
        assert(!isEmpty(pc) && "Empty square has no color!");
        return static_cast<Color>(pc & ColorMask);
    }

    /// \brief Checks if given piece color matches the other
    /// \param pc piece to check the color
    /// \param color this color is compared with piece’s color
    [[nodiscard]] constexpr bool isAlly(const Piece pc, const Color color) noexcept {
        return !isEmpty(pc) && (pc & ColorMask) == static_cast<u8>(color);
    }

    /// \brief Checks if given piece color does not match the other
    [[nodiscard]] constexpr bool isEnemy(const Piece pc, const Color color) noexcept{
        return !isEmpty(pc) && (pc & ColorMask) != static_cast<u8>(color);
    }

    /// \brief Returns opposite color for a given color
    [[nodiscard]] constexpr Color flipColor(const Color color) noexcept{
        return static_cast<Color>(static_cast<u8>(color) ^ ColorMask);
    }

    [[nodiscard]] constexpr Piece fromChar(const char chr) noexcept {
        switch (chr) {
            case 'p': return create(PieceType::Pawn, Color::Black);
            case 'P': return create(PieceType::Pawn, Color::White);
            case 'n': return create(PieceType::Knight, Color::Black);
            case 'N': return create(PieceType::Knight, Color::White);
            case 'b': return create(PieceType::Bishop, Color::Black);
            case 'B': return create(PieceType::Bishop, Color::White);
            case 'r': return create(PieceType::Rook, Color::Black);
            case 'R': return create(PieceType::Rook, Color::White);
            case 'q': return create(PieceType::Queen, Color::Black);
            case 'Q': return create(PieceType::Queen, Color::White);
            case 'k': return create(PieceType::King, Color::Black);
            case 'K': return create(PieceType::King, Color::White);
            default: return static_cast<Piece>(PieceType::None);
        }
    }
}
