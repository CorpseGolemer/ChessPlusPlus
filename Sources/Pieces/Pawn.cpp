#include "../../Headers/Pieces/Pawn.h"
#include "../../Headers/Game/Board.h"

void Pawn::movesInPosition(const Board &board, const int8 row, const int8 col, std::pmr::vector<Move> &allMoves) {
    const Piece* piece = board.getPiece(row, col);

    bool flag = false;
    for (constexpr int moves[4][2] = {{1, 0}, {2, 0}, {1, -1}, {1,1}}; const auto move : moves) {
        const bool color = piece->getColor();
        if (move[0] == 2) {
            if (!notMoved || flag) continue;
        }
        const int nx = row + (color ? move[0] : -move[0]);
        const int ny = col + move[1];

        if (!Board::isOnBoard({nx,ny})) continue;

        MoveFlags flags = None;
        if (const Piece* target = board.getPiece(static_cast<int8>(nx), static_cast<int8>(ny)); target != nullptr) {
            if (move[1] == 0) {flag = true; continue;}
            if (target->getColor() == color) continue;
            flags = static_cast<MoveFlags>(flags | Capture);
        }
        else {
            if (move[1] != 0) {
                if (std::pair(nx, ny) == board.getEnPassant()) {
                    flags = static_cast<MoveFlags>(flags | EnPassant);
                }
                else continue;
            }
        }


        if (color) {
            if (nx == 7) flags = static_cast<MoveFlags>(flags | Promotion);
        }
        else {
            if (nx == 0) flags = static_cast<MoveFlags>(flags | Promotion);
        }

        if (flags & Promotion) {
            for (PieceType pieces[4] = {PieceType::Queen, PieceType::Rook, PieceType::Knight, PieceType::Bishop}; const PieceType pieceSymbol : pieces) {
                const Move mov {{row,col}, {nx,ny}, flags, piece->getSymbol(), pieceSymbol};
                allMoves.push_back(mov);
            }
        }
        else {
            const Move mov {{row, col}, {nx, ny}, flags, piece->getSymbol()};
            allMoves.push_back(mov);
        }
    }
}
