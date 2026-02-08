#pragma once

enum class GameEvent {
    HoverChanged,
    SelectionChanged,
    BoardChanged,
    GameOver
};

class IGameObserver {
public:
    virtual ~IGameObserver() = default;

    virtual void onGameEvent(GameEvent event) = 0;
};