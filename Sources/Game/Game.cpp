#include "../../Headers/Game/Game.h"
#include <cassert>

Game::Game() {
    board.generateMoves();
    history.pushHash(board.getZobristHash());
}

// Zwraca kolor figur, które teraz się ruszają
Game::Player Game::getCurrentPlayer() const {
    return this->currentPlayer;
}

void Game::changeTurn() {
    if (currentPlayer == Player::White) {
        currentPlayer = Player::Black;
    }
    else if (currentPlayer == Player::Black) {
        currentPlayer = Player::White;
    }
}

bool Game::tryMakeMove(Move &moveToMake) {
    if (const Piece* piece = board.getPiece(moveToMake.getPosition().first, moveToMake.getPosition().second); !piece) { return false; }

    if (board.tryMakeMove(moveToMake)) {
        makeMove(moveToMake);
        return true;
    }
    return false;
}

// Implementacja nextTurn
bool Game::tryMakeMove(const int clickedRow, const int clickedCol, std::pair<int, int> & selectedTile) {
    Move moveToMake {{selectedTile.first, selectedTile.second}, {clickedRow, clickedCol}};
    return tryMakeMove(moveToMake);
}

// Przeładowanie metody tryChangeTurn z myślą o graczu, który zawsze wykonuje poprawny ruch
void Game::makeMove(const Move &move) {
    // Stwórz bazowy obraz cofania ruchu
    Move::UndoState undoState;
    // Wykonaj ruch (uzupełni obraz)
    board.applyMove(move, undoState);
    // Zmień turę
    changeTurn();
    // Daj informacje do Board
    board.setTurn(!board.isWhitesTurn());
    // Zwiększ licznik pół ruchów, w przypadku spełnienia warunków
    if (!(move.hasMoveFlag(Capture) || move.getPieceSymbol() == PieceType::Pawn)) halfMoveCount++;
    else halfMoveCount = 0;

    board.computeFullHash();

    if (board.isPromoting() == -1) {
        board.generateMoves();
        if (board.getAllMoves().empty()) {
            if (board.inCheck()) {
                if (board.isWhitesTurn()) endReason = EndReason::Mate_Black;
                else endReason = EndReason::Mate_White;
            }
            else {
                endReason = EndReason::Stalemate;
            }
        }
        // Szukaj innych remisów
        postMoveUpdate();
    }
    // Zaktualizuj historię
    history.pushHistoryEntry(move,undoState);
    history.pushHash(board.getZobristHash());
    lastUndo.reset();
}

// Implementacja undoMove
void Game::undoMove() {
    // Zmień turę
    changeTurn();
    // Informacja do Board
    board.setTurn(!board.isWhitesTurn());

    board.undoMove(getLastMove().value(), getLastUndoState().value());
    if (board.isPromoting() == -1) {
        board.generateMoves();
    }
    this->halfMoveCount = history.getLastHistoryEntry().prevHalfMoveCount;

    lastUndo = {getLastMove().value(), getLastUndoState().value()};
    history.popHistoryEntry();
    history.popHash(board.getZobristHash());
}

std::bitset<12> Game::getOnBoardChanges() const {
    if (history.isEmpty()) return std::bitset<12>("111111111111");
    auto result = std::bitset<12>("000000000000");
    Move m;
    Move::UndoState u;
    if (lastUndo.has_value()) {
        m = lastUndo.value().first;
        u = lastUndo.value().second;
        // RUCH
        assert(board.getPiece(m.getPosition()) != nullptr && "getOnBoardChanges: getPiece(m.getPosition()) == nullptr");
        result.set(board.getPiece(m.getPosition())->coloredSymbol());
        // Promocja
        const int changed = static_cast<int>(m.promotedPiece) * 2 + board.isWhitesTurn();
        if (changed < 12) result.set(changed);
        // Bicie
        if (const Piece* p = u.capturedPiece; p != nullptr) result.set(p->coloredSymbol());
        // ROSZADY
        if (m.hasMoveFlag(CastleShort) || m.hasMoveFlag(CastleLong)) {
            result.set(0 + board.isWhitesTurn());
        }
    }
    else {
        m = getLastMove().value();
        u = getLastUndoState().value();
        // Ruch
        const int moved = static_cast<int>(m.getPieceSymbol()) * 2 + !board.isWhitesTurn();
        if (moved < 12) result.set(moved);
        // Promocja
        const int changed = static_cast<int>(m.promotedPiece) * 2 + !board.isWhitesTurn();
        if (changed < 12) result.set(changed);
        // Bicie
        if (const Piece* p = u.capturedPiece; p != nullptr) result.set(p->coloredSymbol());
        // ROSZADY
        if (m.hasMoveFlag(CastleShort) || m.hasMoveFlag(CastleLong)) {
            result.set(0 + !board.isWhitesTurn());
        }
    }

    return result;
}

// Implementacja metody getBoard
const Board & Game::getBoard() const {
    return board;
}

// Implementacja metody getLastMove
std::optional<Move> Game::getLastMove() const {
    if (history.isEmpty()) return std::nullopt;
    return history.getLastHistoryEntry().move;
}

std::optional<Move::UndoState> Game::getLastUndoState() const {
    if (history.isEmpty()) return std::nullopt;
    return history.getLastHistoryEntry().undoState;
}

// Implementacja metody handlePromotion
void Game::handlePromotion(const bool closePromotionWindow, const PieceType choice) {
    undoMove();
    if (closePromotionWindow) {
        board.resetPromotion();
        return;
    }
    const Move finalPromotionMove = board.applyPromotion(choice);
    makeMove(finalPromotionMove);
}

Game::EndReason Game::isGameOver() const {
    return endReason;
}

void Game::postMoveUpdate() {
    // Insufficient Material Check
    if (board.isInsufficientMaterial()) {
        this->endReason = EndReason::Insufficient_Material;
        return;
    }
    // Threefold Repetition Check
    if (history.isThreefoldRepetition(board.getZobristHash())) {
        this->endReason = EndReason::Threefold_Repetition;
        return;
    }
    // 50-Move-Rule-Check
    if (halfMoveCount >= 100) {
        this->endReason = EndReason::Fifty_Move_Rule;
    }
}

void Game::setCurrentPlayer() {
    if (board.isWhitesTurn()) currentPlayer = Player::White;
    else currentPlayer = Player::Black;
}

std::unique_ptr<Board> Game::copyBoard() const {
    auto copy = std::make_unique<Board>();
    board.copy(copy);
    Move::UndoState undoState;
    if (history.gameHistory.empty()) return copy;
    for (auto& move : history.gameHistory) {
        copy->applyMove(move.move, undoState);
    }
    return copy;
}








