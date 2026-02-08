#include "../../Headers/Bots/ChciwyJakub.h"

#include <iostream>

#include "../../Headers/Game/Zobrist.h"

void ChciwyJakub::syncFromBoard(std::unique_ptr<Board> board) {
    std::lock_guard lock(boardMutex);
    this->board = std::move(board);
    shouldCancel = false;
    lastMoveInGame = std::nullopt;
}

void ChciwyJakub::onMoveApplied(const Move &move) {
    std::lock_guard lock(boardMutex);
    Move::UndoState undoState;
    this->board->applyMove(move, undoState);
    board->setTurn(!board->isWhitesTurn());
    lastMoveInGame = move;
    ++madeMoves;
}

std::future<Move> ChciwyJakub::chooseMoveAsync() {
    calculating = true;
    shouldCancel = false;

    return std::async(std::launch::async, [this]() {
        return calculateMove();
    });
}

void ChciwyJakub::cancelCalculation() {
    shouldCancel = true;
}

bool ChciwyJakub::isCalculating() {
    return calculating;
}

Move ChciwyJakub::calculateMove() {
    const auto startTime = std::chrono::high_resolution_clock::now();
    const auto thinkDuration = std::chrono::milliseconds(std::clamp(static_cast<int>(Zobrist::MT19937::getGauss(2000, 1000)), 500, 5000));

    while (!shouldCancel) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = currentTime - startTime;

        if (elapsedTime >= thinkDuration) { break;}

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (shouldCancel) {
        calculating = false;
        throw std::runtime_error("ChciwyJakub::isCalculating() failed");
    }

    std::lock_guard lock(boardMutex);
    Move bestMove = findBestMove();
    calculating = false;
    return bestMove;
}

Move ChciwyJakub::findBestMove() const {
    board->generateMoves();
    const auto allMoves = board->getAllMoves();

    std::vector<MoveEvaluations> evaluations;
    evaluations.reserve(allMoves.size());

    for (const auto &move : allMoves) {
        evaluations.push_back(evaluateMove(move));
    }

    return selectedJakubMove(evaluations);
}

ChciwyJakub::MoveEvaluations ChciwyJakub::evaluateMove(const Move& move) const {
    int moveScore = 0;
    constexpr int pieceValMult = 100;

    // Wykonujemy ten ruch i Jakub będzie liczył
    Move::UndoState undoState;
    this->board->applyMove(move, undoState);
    board->setTurn(!board->isWhitesTurn());
    board->generateMoves();

    if (board->getAllMoves().empty()) {
        if (board->inCheck()) {
            if (Zobrist::MT19937::getDouble(0.f, 1.f) < 0.8f) {
                return {move, 40000}; // PRIORYTET ABSOLUTNY
            }
        }
    }

    // AKTUALIZACJA 1 -- JAKUB SIĘ TERAZ MŚCI ZA ZBICIE
    if (lastMoveInGame.has_value() && lastMoveInGame.value().getNextPosition() == move.getNextPosition()) {
        moveScore += 120;
    }

    // AKTUALIZACJA 2 -- JAKUB LUBI ZACZYNAĆ OD HETMANA
    if (move.getPieceSymbol() == PieceType::Queen && madeMoves < 15) {
        moveScore += 60;
    }

    //  3 ------ JAKUB JEST CHCIWY: Jak widzi bicie, to dostaje pierdolca (nie no żart XD)
    if (undoState.capturedPiece != nullptr) {
        const int victimVal = undoState.capturedPiece->getValue();
        // Bonus za bicie + zachęta
        moveScore += victimVal * pieceValMult + 100;
    }
    if (undoState.promotedPiece != nullptr) {
        const auto&[fst, snd] = move.getNextPosition();
        Piece* piece = board->getPiece(fst,snd);
        if (piece != nullptr) {
            moveScore += piece->getValue() * pieceValMult;
        }
    }

    // 4 --------- Jakub nie lubi być w szachu
    if (board->inCheck()) moveScore -= 150;

    /// 5 -------- ANALIZA POZYCJI JAKUBA (beznadziejna ale nie oczekujmy od niego dużo)

    /// JAKUB KOCHA WIĘCEJ MATERIAŁU
    if (move.hasMoveFlag(Promotion)) moveScore += 800;
    /// Jakub usłyszał od pewnego pana z białego wana, że warto roszować
    if (move.hasMoveFlag(CastleLong) || move.hasMoveFlag(CastleShort)) moveScore += 150;
    /// Jakub troszkę lubi szachować
    if (move.hasMoveFlag(Check)) moveScore += 80;
    auto [row, col] = move.getNextPosition();
    /// Instynk Jakuba podpowiada mu, że warto kontrolować centrum (niestety niezbyt mocno XD)
    if (row >= 3 && row <= 4 && col >= 3 && col <= 4) moveScore += 60;
    else if (row >= 2 && row <= 2 && col >= 5 && col <= 5) moveScore += 20;

    int maxLoss = 0;
    for (const auto &m : board->getAllMoves()) {
        if (const Piece* p = board->getPiece(m.getNextPosition().first, m.getNextPosition().second); p != nullptr) {
            int lostVal = p->getValue();
            maxLoss = std::max(maxLoss, lostVal);
        }
    }

    /// Jakub widzi stratę bierki, ale bardziej preferuje CHCIWOŚĆ
    moveScore -= maxLoss * pieceValMult / 2;


    /// Przywracamy starą pozycję
    this->board->undoMove(move, undoState);
    board->setTurn(!board->isWhitesTurn());

    // Czym byłby Jakub bez szumu
    moveScore += Zobrist::MT19937::getInt(-80, 80);

    return {move, moveScore};
}

Move ChciwyJakub::selectedJakubMove(const std::vector<MoveEvaluations> &evaluations) {
    int totalScore = 0;
    for (const auto&[move, score] : evaluations) {
        totalScore += std::max(1, score);
    }

    if (Zobrist::MT19937::getDouble(0.f, 1.f) < 0.1f) {
        const auto bestIt = std::max_element(evaluations.begin(), evaluations.end(), [](const auto &a, const auto &b) {
            return a.score < b.score;
        });
        return bestIt->move;
    }

    const int randomPoint = Zobrist::MT19937::getInt(0, totalScore - 1);

    int cumulativeScore = 0;
    for (const auto &[move, score] : evaluations) {
        cumulativeScore += std::max(1,score);
        if (cumulativeScore > randomPoint) {
            return move;
        }
    }

    return evaluations.back().move;
}





