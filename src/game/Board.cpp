#include "Board.h"

std::expected<void, fen::FenError> Board::loadFromFen(const fen::Segments &segments) {
    const auto grid = fen::parsePosition(segments.placement());
    if (!grid) return std::unexpected(grid.error());

    const auto side = fen::parseSideToMove(segments.sideToMove());
    if (!side) return std::unexpected(side.error());

    const auto rights = fen::parseCastlingRights(segments.castlingRights());
    if (!rights) return std::unexpected(rights.error());

    const auto enPassant = fen::parseEnPassant(segments.enPassant());
    if (!enPassant) return std::unexpected(enPassant.error());

    // TODO: Validation if given position is legal

    this->board = *grid;
    this->sideToMove = *side;
    this->castlingRights = *rights;
    this->enPassantTarget = *enPassant;
    return {};
}

std::expected<void, fen::FenError> Board::loadFromFen(const std::string_view fenString) {
    return fen::parseSegments(fenString).and_then([this](const fen::Segments &seg) noexcept {
        return this->loadFromFen(seg);
    });
}

std::string Board::toString() const {
    std::string result;
    result.reserve(650);

    constexpr std::string_view rankBreak = "  +---+---+---+---+---+---+---+---+\n";

    for (int r = 7; r >= 0; r--) {
        const auto rank = static_cast<u8>(r);
        result += rankBreak;
        result.push_back(static_cast<char>(rank + '1'));
        for (u8 file = 0; file < 8; file++) {
            result += " | ";
            result.push_back(piece::toChar(pieceAt(rank, file)));
        }
        result += " |\n";
    }
    result += rankBreak;
    result += "    a   b   c   d   e   f   g   h\n\n";

    result += "Side: ";
    result += this->sideToMove == Color::White ? "White" : "Black";

    result += " | CastlingRights: ";
    if (this->castlingRights == CastlingRights::None) result.push_back('-');
    else {
        if (castling::has(this->castlingRights, CastlingRights::WhiteKing)) result.push_back('K');
        if (castling::has(this->castlingRights, CastlingRights::WhiteQueen)) result.push_back('Q');
        if (castling::has(this->castlingRights, CastlingRights::BlackKing)) result.push_back('k');
        if (castling::has(this->castlingRights, CastlingRights::BlackQueen)) result.push_back('q');
    }

    result += " | EnPassantTarget: ";
    if (this->enPassantTarget == board_config::NoSquare) result.push_back('-');
    else {
        result.push_back(static_cast<char>(this->enPassantTarget % 8 + 'a'));
        result.push_back(static_cast<char>(this->enPassantTarget / 8 + '1'));
    }
    return result;
}



