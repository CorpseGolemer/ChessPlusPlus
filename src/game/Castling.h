#pragma once

#include <concepts>
#include <optional>

#include "../utils/Types.h"

enum class CastlingRights : u8 {
    None = 0b0000'0000, // -
    WhiteKing = 0b0000'0001, // K
    WhiteQueen = 0b0000'0010, // Q
    BlackKing = 0b0000'0100, // k
    BlackQueen = 0b0000'1000, // q
    All = 0b0000'1111 // KQkq
};

constexpr CastlingRights operator|(CastlingRights a, CastlingRights b) noexcept {
    return static_cast<CastlingRights>(static_cast<u8>(a) | static_cast<u8>(b));
}

constexpr CastlingRights& operator|=(CastlingRights& a, CastlingRights b) noexcept {
    return a = static_cast<CastlingRights>(static_cast<u8>(a) | static_cast<u8>(b));
}

constexpr CastlingRights operator&(CastlingRights a, CastlingRights b) noexcept {
    return static_cast<CastlingRights>(static_cast<u8>(a) & static_cast<u8>(b));
}

constexpr CastlingRights operator~(CastlingRights a) noexcept {
    return static_cast<CastlingRights>(~static_cast<u8>(a) & static_cast<u8>(CastlingRights::All));
}

constexpr CastlingRights& operator&=(CastlingRights& a, CastlingRights b) noexcept {
    return a = static_cast<CastlingRights>(static_cast<u8>(a) & static_cast<u8>(b));
}

constexpr CastlingRights operator^(CastlingRights a, CastlingRights b) {
    return static_cast<CastlingRights>(static_cast<u8>(a) ^ static_cast<u8>(b));
}

namespace castling {
    /// \brief Combines all passed castling rights together.
    template <std::same_as<CastlingRights>... Args>
    [[nodiscard]] constexpr CastlingRights with(const Args... rights) noexcept {
        return (CastlingRights::None | ... | rights);
    }

    /// \brief Removes specified rights from base state
    [[nodiscard]] constexpr CastlingRights without(const CastlingRights base, const CastlingRights toRemove) noexcept {
        return base & ~toRemove;
    }

    /// \brief Checks if base state has all specified rights
    template <std::same_as<CastlingRights>... Args>
    [[nodiscard]] constexpr bool has(const CastlingRights base, const Args... rights) noexcept {
        const auto target = (CastlingRights::None | ... | rights);
        return (base & target) == target;
    }

    /// \brief Checks if base state has any of specified rights
    template <std::same_as<CastlingRights>... Args>
    [[nodiscard]] constexpr bool hasAny(const CastlingRights base, const Args... rights) noexcept {
        return (base & (... | rights)) != CastlingRights::None;
    }

    constexpr std::optional<CastlingRights> fromChar(const char chr) noexcept {
        switch (chr) {
            case '-': return CastlingRights::None;
            case 'K': return CastlingRights::WhiteKing;
            case 'Q': return CastlingRights::WhiteQueen;
            case 'k': return CastlingRights::BlackKing;
            case 'q': return CastlingRights::BlackQueen;
            default: return std::nullopt;
        }
    }
}