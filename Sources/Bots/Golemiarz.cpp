#include "../../Headers/Bots/Golemiarz.h"

#include <iostream>

#include "../../Headers/Game/Board.h"
#include "../../Headers/Game/Move.h"

void Golemiarz::syncFromBoard(std::unique_ptr<Board> board) {
    std::lock_guard lock(boardMutex);
    this->board = std::move(board);
    shouldCancel = false;
}

void Golemiarz::onMoveApplied(const Move &move) {
    std::lock_guard lock(boardMutex);
    Move::UndoState undoState;
    this->board->applyMove(move, undoState);
    board->setTurn(!board->isWhitesTurn());
}

std::future<Move> Golemiarz::chooseMoveAsync() {
    calculating = true;
    shouldCancel = false;
    myColor = board->isWhitesTurn();

    return std::async(std::launch::async, [this]() {
        return calculateMove();
    });
}

void Golemiarz::cancelCalculation() {
    shouldCancel = true;
}

bool Golemiarz::isCalculating() {
    return calculating;
}

Move Golemiarz::calculateMove() {
    const auto start = std::chrono::high_resolution_clock::now();
    board->generateMoves();
    Move bestMove;
    const auto moves = board->getAllMoves();
    int bestValue = std::numeric_limits<int>::min();

    for (const auto& move : moves) {
        Move::UndoState undoState;
        board->applyMove(move, undoState);
        board->setTurn(!board->isWhitesTurn());

        if (const int boardValue = minimax(3, false); boardValue > bestValue) {
            bestMove = move;
            bestValue = boardValue;
        }

        board->undoMove(move, undoState);
        board->setTurn(!board->isWhitesTurn());
    }
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << duration.count() / 1000 << " milliseconds" << std::endl;
    return bestMove;
}

int Golemiarz::minimax(const int depth, const bool isMaximizing) {
    if (depth == 0) {
        return evaluateCurrentBoard();
    }

    board->generateMoves();
    const auto moves = board->getAllMoves();
    if (moves.empty()) {
        if (board->inCheck()) return isMaximizing ? -50000 : 50000;
        return 0;
    }

    if (isMaximizing) {
        int bestScore = std::numeric_limits<int>::min();
        for (const auto& move : moves) {
            Move::UndoState undoState;
            board->applyMove(move, undoState);
            board->setTurn(!board->isWhitesTurn());

            bestScore = std::max(bestScore, minimax(depth - 1, false));

            board->undoMove(move, undoState);
            board->setTurn(!board->isWhitesTurn());
        }
        return bestScore;
    }
    int bestScore = std::numeric_limits<int>::max();
    for (const auto& move : moves) {
        Move::UndoState undoState;
        board->applyMove(move, undoState);
        board->setTurn(!board->isWhitesTurn());

        bestScore = std::min(bestScore, minimax(depth - 1, true));

        board->undoMove(move, undoState);
        board->setTurn(!board->isWhitesTurn());
    }
    return bestScore;
}

int Golemiarz::evaluateCurrentBoard() const {
    int score = 0;
    for (int8 i = 0; i < 8; i++) {
        for (int8 j = 0; j < 8; j++) {
            if (const auto p = board->getPiece(i,j); p != nullptr) {
                int val = p->getValue() * 100;
                if (i >= 3 && i <= 4 && j >= 3 && j <= 4) val += 20;
                if (i >= 2 && i <= 5 && j >= 2 && j <= 5) val += 10;

                if (p->getColor() == myColor) score += val;
                else score -= val;
            }
        }
    }
    return score;
}






