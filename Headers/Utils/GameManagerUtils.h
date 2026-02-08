#pragma once

#include <memory>
#include "../Bots/IBot.h"
#include "../Game/Move.h"

class GameManagerUtils {
public:
    /// Struct odpowiadający za przechowywanie informacji o ciągniętej figurze
    struct DraggingPiece {
        bool active = false;
        bool dontDeselect = false;
        unsigned char symbol = 12;

        std::pair<int, int> pos = {-1, -1};
        std::pair<int, int> endPos = {-1, -1};

        void startDraggingPiece(unsigned char symbol, const std::pair<int, int> &pos, bool dontDeselect);
        Move onStopDragging(const std::pair<int, int> &pos);
    };

    struct BotInstance {
        std::unique_ptr<IBot> bot = nullptr;
        BotState botState = BotState::None;
        std::future<Move> botFuture;
        std::optional<Move> pendingBotMove = std::nullopt;

        void updateBotBoard(const Move& move) const;
        void destroyBot();
        bool initializeBot(std::unique_ptr<IBot> bot, std::unique_ptr<Board> currentPosition);
        void startBotThinking();
        bool tryGetBotMove();
        void onMoveMade();
    };
};
