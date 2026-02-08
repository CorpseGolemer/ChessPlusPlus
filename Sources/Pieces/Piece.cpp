#include "../../Headers/Pieces/Piece.h"
#include "../../Headers/Game/Board.h"
#include "../../Headers/App.h"

// Konstruktor bierki, ustawia konkretne pola klasy
Piece::Piece(const bool isWhite, const PieceType symbol, const int value) {
    this->isWhite = isWhite;
    this->symbol = symbol;
    this->value = value;
}

// Zwraca pole kolor
bool Piece::getColor() const {
    return this->isWhite;
}

// Zwraca pole symbol
PieceType Piece::getSymbol() const {
    return this->symbol;
}

unsigned char Piece::coloredSymbol() const {
    return static_cast<unsigned char>(this->symbol) * 2 + this->isWhite;
}

// Zwraca pole wartość
int Piece::getValue() const {
    return this->value;
}

// Wylicza ruchy dla gońców, hetmanów i wież (antyduplikacja)
void Piece::addSlidingMoves(const Board & board, int8 row, int8 col, std::pmr::vector<Move> &allMoves, const int8 directions[][2], const int8 dirCount) {
    const Piece * piece = board.getPiece(row, col);
    for (int8 i = 0; i < dirCount; i++) {
        const int8 dx = directions[i][0];
        const int8 dy = directions[i][1];

        int nx = row + dx;
        int ny = col + dy;

        while (Board::isOnBoard({nx,ny})) {
            if (const Piece * target = board.getPiece(static_cast<int8>(nx), static_cast<int8>(ny)); target != nullptr) {
                if (target->getColor() == piece->getColor()) {
                    break;
                }
                Move move {
                    {row, col}, {nx, ny},
                    Capture,
                    piece->getSymbol()
                };
                allMoves.push_back(move);
                break;
            }

            Move move {
                {row, col}, {nx, ny},
                None,
                piece->getSymbol()
            };
            allMoves.push_back(move);
            nx += dx;
            ny += dy;
        }
    }
}

// Wylicza ruchy króla oraz skoczków
void Piece::addSingleMoves(const Board &board, const int8 row, const int8 col, std::pmr::vector<Move> &allMoves, const int8 moves[][2], const int8 movesCount) {
    const Piece * piece = board.getPiece(row, col);
    for (int8 i=0; i<movesCount; i++) {
        const int8 dx = moves[i][0];
        const int8 dy = moves[i][1];

        int nx = row + dx;
        int ny = col + dy;

        if (Board::isOnBoard({nx, ny})) {
            if (const Piece * target = board.getPiece(static_cast<int8>(nx), static_cast<int8>(ny)); target != nullptr) {
                if (target->getColor() == piece->getColor()) {
                    continue;
                }
                Move move {
                    {row, col}, {nx, ny},
                    Capture,
                    piece->getSymbol()
                };
                allMoves.push_back(move);
                continue;
            }
            Move move {
                {row, col}, {nx, ny},
                None,
                piece->getSymbol()

            };
            allMoves.push_back(move);
        }
    }
}
















