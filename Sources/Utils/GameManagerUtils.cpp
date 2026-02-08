#include "../../Headers/Utils/GameManagerUtils.h"
#include <iostream>
#include "../../Headers/Game/Board.h"

void GameManagerUtils::DraggingPiece::startDraggingPiece(const unsigned char symbol, const std::pair<int, int> &pos, const bool dontDeselect) {
    this->pos = {-1, -1};
    endPos = {-1, -1};

    active = true;
    this->dontDeselect = dontDeselect;
    this->pos = pos;
    this->symbol = symbol;
}

Move GameManagerUtils::DraggingPiece::onStopDragging(const std::pair<int, int> &pos) {
    active = false;
    endPos = pos;

    return Move{this->pos, endPos};
}

void GameManagerUtils::BotInstance::updateBotBoard(const Move &move) const {
    if (botState != BotState::None) {
        bot->onMoveApplied(move);
    }
}

void GameManagerUtils::BotInstance::destroyBot() {
    botState = BotState::None;
    bot = nullptr;
}

bool GameManagerUtils::BotInstance::initializeBot(std::unique_ptr<IBot> bot, std::unique_ptr<Board> currentPosition) {
    try {
        this->bot = std::move(bot);
        this->bot->syncFromBoard(std::move(currentPosition));
        botState = BotState::Initialized;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Bot initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void GameManagerUtils::BotInstance::startBotThinking() {
    botState = BotState::Thinking;
    botFuture = bot->chooseMoveAsync();
}

bool GameManagerUtils::BotInstance::tryGetBotMove() {
    if (!botFuture.valid()) {
        return false;
    }

    auto status = botFuture.wait_for(std::chrono::milliseconds(0));

    if (status != std::future_status::ready) {
        return false;
    }

    try {
        Move move = botFuture.get();
        pendingBotMove = std::move(move);
        botState = BotState::MoveReady;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Bot move failed: " << e.what() << std::endl;
        return false;
    }
}

void GameManagerUtils::BotInstance::onMoveMade() {
    pendingBotMove.reset();
    botState = BotState::Initialized;
}