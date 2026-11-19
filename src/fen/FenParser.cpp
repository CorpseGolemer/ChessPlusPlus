#include "FenParser.h"

#include <format>
#include <charconv>
#include <cctype>

#include "../utils/Logger.h"
#include "../game/Piece.h"

namespace fen {
    namespace {
        constexpr std::string_view nums = "12345678";
    }

    bool parseSegments(const std::string_view fenString, Segments& out) {
        Segments seg;
        std::size_t start = 0;

        while (start < fenString.size()) {
            while (start < fenString.size() && std::isspace(static_cast<unsigned char>(fenString[start]))) {
                ++start;
            }

            if (start >= fenString.size()) {
                break;
            }

            std::size_t end = start;
            while (end < fenString.size() && !std::isspace(static_cast<unsigned char>(fenString[end]))) {
                ++end;
            }

            if (seg.count >= SegmentCount) {
                logger::warning(std::format("Passed FEN has more than {} segments", SegmentCount));
                return false;
            }

            seg.fields[seg.count++] = fenString.substr(start, end - start);
            start = end;
        }

        if (seg.count < MinimalSegmentCount) {
            logger::warning(std::format("FEN has {} segments, need at least {}.", seg.count, MinimalSegmentCount));
            return false;
        }
        out = seg;
        return true;
    }

    bool parsePosition(const std::string_view placement, BoardGrid &out) {
        BoardGrid tempBoardGrid{};
        std::size_t start = 0;
        u8 rank = 7;

        while (start < placement.size()) {
            if (rank > 7) {
                logger::warning("Invalid FEN [Pieces]: Too many ranks!");
                return false;
            }

            u8 file = 0;
            bool wasDigit = false;

            while (start < placement.size() && placement[start] != '/') {
                const char chr = placement[start];

                if (file >= 8) {
                    logger::warning("Invalid FEN [Pieces]: Too many files!");
                    return false;
                }
                if (const Piece p = piece::fromChar(chr); piece::isEmpty(p)) {
                    if (nums.find(chr) != std::string_view::npos) {
                        if (wasDigit) {
                            logger::warning("Invalid FEN [Pieces]: Two digits cannot follow each other!");
                            return false;
                        }
                        file += chr - '0';
                        wasDigit = true;
                    }
                    else {
                        logger::warning(std::format("Invalid FEN [Pieces]: Prohibited character '{}'!", chr));
                        return false;
                    }
                }
                else {
                    tempBoardGrid[board_config::squareIndex(rank, file)] = p;
                    file++;
                    wasDigit = false;
                }
                start++;
            }
            if (file != 8) {
                logger::warning("Invalid FEN [Pieces]: Wrong file count!");
                return false;
            }
            if (start >= placement.size()) break;
            if (rank == 0) { // if outside this loop would handle this situation
                logger::warning("Invalid FEN [Pieces]: Trailing slash after 1st rank!");
                return false;
            }
            rank--;
            start++;
        }

        if (rank != 0) {
            logger::warning("Invalid FEN [Pieces]!");
            return false;
        }

        out = tempBoardGrid;
        return true;
    }

    bool parseSideToMove(const std::string_view sideToMove, Color& out) {
        if (sideToMove.size() != 1) {
            logger::warning("Invalid FEN [Color]: Segment must be a single character!");
            return false;
        }
        switch (sideToMove[0]) {
            case 'b': {
                out =  Color::Black;
                return true;
            }
            case 'w': {
                out = Color::White;
                return true;
            }
            default: {
                logger::warning("Invalid FEN [Color]: Unknown color character!");
                return false;
            }
        }
    }

    bool parseCastlingRights(const std::string_view castlingRights, CastlingRights& out) {
        auto tempCastlingRights = CastlingRights::None;
        unsigned char lastChar = 0;
        std::size_t start = 0;

        if (castlingRights.empty()) {
            logger::warning("Invalid FEN [CastlingRights]: Empty castling rights!");
            return false;
        }

        while (start < castlingRights.size()) {
            const char chr = castlingRights[start];
            const CastlingRights rights = castling::fromChar(chr);

            if (rights == CastlingRights::None) {
                if (start == 0 && chr == '-' && castlingRights.size() == 1) {
                    out =  CastlingRights::None;
                    return true;
                }
                logger::warning("Invalid FEN [CastlingRights]: Unknown character");
                return false;
            }
            if (chr > lastChar) { // This condition ensures that the order is maintained and there are no duplicates
                lastChar = chr;
                tempCastlingRights = castling::with(tempCastlingRights, rights);
            }
            else {
                logger::warning("Invalid FEN [CastlingRights]: Wrong character layout!");
                return false;
            }
            start++;
        }

        out = tempCastlingRights;
        return true;
    }

    bool parseEnPassant(const std::string_view enPassant, u8 &out) {
        if (enPassant.empty()) {
            logger::warning("Invalid FEN [EnPassant]: Empty enPassant segment!");
            return false;
        }
        if (enPassant.size() == 1) {
            if (enPassant[0] == '-') {
                out = board_config::NoSquare;
                return true;
            }
            logger::warning("Invalid FEN [EnPassant]!");
            return false;
        }
        if (enPassant.size() == 2) {
            const char file = enPassant[0];
            const char rank = enPassant[1];
            if (file < 'a' || file > 'h') {  // Only small letters from a to h
                logger::warning("Invalid FEN [EnPassant]!");
                return false;
            }
            if (rank != '3' && rank != '6') { // Only 3rd and 6th rank are acceptable
                logger::warning("Invalid FEN [EnPassant]!");
                return false;
            }
            out = board_config::squareIndex(rank - '1', file - 'a');
            return true;
        }
        logger::warning("Invalid FEN [EnPassant]: Too many characters!");
        return false;
    }

    bool parseHalfMove(const std::string_view halfMove, u64 &out) {
        if (halfMove.empty()) {
            logger::info("FEN [HalfMove]: Empty half move counter. It will be set to 0.");
            out = 0;
            return true;
        }

        u64 temp = 0;
        if (const auto [ptr, err]
            = std::from_chars(halfMove.data(), halfMove.data() + halfMove.size(), temp);
            err != std::errc() || ptr != halfMove.data() + halfMove.size()) {

            logger::warning("Invalid FEN [HalfMove]!");
            return false;
        }
        out = temp;
        return true;
    }

    bool parseFullMove(const std::string_view fullMove, u64 &out) {
        if (fullMove.empty()) {
            logger::info("FEN [FullMove]: Empty full move counter. It will be set to 1.");
            out = 1;
            return true;
        }

        u64 temp = 0;
        if (const auto [ptr, err]
            = std::from_chars(fullMove.data(), fullMove.data() + fullMove.size(), temp);
            err != std::errc() || ptr != fullMove.data() + fullMove.size()) {

            logger::warning("Invalid FEN [FullMove]!");
            return false;
        }
        if (temp == 0) {
            logger::warning("Invalid FEN [FullMove]: Must be >=1!");
            return false;
        }
        out = temp;
        return true;
    }


}
