#include "../../Headers/Game/GameHistory.h"

#include <algorithm>
#include <cassert>
#include <ranges>

void GameHistory::pushHistoryEntry(const Move &move, const Move::UndoState &undoState, const int prevHalfMoveCount) {
    gameHistory.push_back({move, undoState, prevHalfMoveCount});
}

void GameHistory::popHistoryEntry() {
    gameHistory.pop_back();
}

bool GameHistory::isEmpty() const {
    return gameHistory.empty();
}

const GameHistory::HistoryEntry &GameHistory::getLastHistoryEntry() const {
    return gameHistory.back();
}

void GameHistory::setLastHistoryEntry(const HistoryEntry &historyEntry) {
    gameHistory.pop_back();
    gameHistory.push_back(historyEntry);
}

void GameHistory::pushHash(const u64 hash) {
    repetitionCount[hash]++;
}

void GameHistory::popHash(const u64 hash) {
    assert(!gameHistory.empty() && "There are no entries in history");

    const auto it = repetitionCount.find(hash);
    assert(it != repetitionCount.end());

    if (--it->second == 0) {
        repetitionCount.erase(it);
    }
}

bool GameHistory::isThreefoldRepetition() const {
    return std::ranges::any_of(repetitionCount, [](auto& entry) {
        return entry.second >= 3;
    });
}

bool GameHistory::isThreefoldRepetition(const u64 hash) const {
    if (gameHistory.size() < 9) return false;

    const auto it = repetitionCount.find(hash);

    return it != repetitionCount.end() && it->second >= 3;
}






