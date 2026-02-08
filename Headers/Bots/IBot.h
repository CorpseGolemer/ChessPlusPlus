#pragma once
#include <future>

class Move;
class Board;

enum class BotState {
    None,
    Initialized,
    Thinking,
    MoveReady
};

class IBot {
public:
    virtual ~IBot() = default;

    virtual void syncFromBoard(std::unique_ptr<Board> board) = 0;
    virtual void onMoveApplied(const Move& move) = 0;
    virtual std::future<Move> chooseMoveAsync() = 0;
    virtual void cancelCalculation() = 0;
    virtual bool isCalculating() = 0;
};