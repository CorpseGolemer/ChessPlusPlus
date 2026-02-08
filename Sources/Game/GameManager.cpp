#include "../../Headers/Game/GameManager.h"
#include <algorithm>
#include <iostream>
#include "../../Headers/Graphics/BoardRenderer.h"

/// setState
void GameManager::setState(const GameState newState) {
    if (state == newState) {return;}
    state = newState;
}

/// Implementacja handleLeftClick
void GameManager::handleIdleLeftClick(const std::pair<int, int> &tile) {
    if (!Board::isOnBoard(tile)) return;
    notify(GameEvent::SelectionChanged);
    highlightedTiles.clear();

    if (const Piece * p = game.getBoard().getPiece(static_cast<int8>(tile.first), static_cast<int8>(tile.second)); p != nullptr) {
        selectedTile = tile;
        draggingPiece.startDraggingPiece(p->coloredSymbol(), tile, true);
        setState(GameState::Dragging);
    }
}

/// Implementacja handleSelectionLeftClick
void GameManager::handleSelectionLeftClick(const std::pair<int, int> &tile) {
    notify(GameEvent::SelectionChanged);
    const Piece* p = nullptr;
    if (Board::isOnBoard(tile)) {
        p = game.getBoard().getPiece(static_cast<int8>(tile.first), static_cast<int8>(tile.second));
    }

    if (selectedTile.first == tile.first && selectedTile.second == tile.second) {
        assert(p != nullptr && "Selected file should have a piece!");
        draggingPiece.startDraggingPiece(p->coloredSymbol(), tile, false);
        setState(GameState::Dragging);
        return;
    }
    if (localPlayer == game.getCurrentPlayer()) {
        if (game.tryMakeMove(tile.first, tile.second, selectedTile)) {
            onAfterMove();
            return;
        }
    }

    if (p != nullptr) {
        selectedTile = tile;
        draggingPiece.startDraggingPiece(p->coloredSymbol(), tile, true);
        setState(GameState::Dragging);
        return;
    }

    selectedTile = {-1,-1};
    setState(GameState::Idle);
}

/// Implementacja handleRightClick
void GameManager::handleRightClick(const std::pair<int, int> &tile) {
    if (state != GameState::GameOver) {
        setState(GameState::Idle);
    }
    notify(GameEvent::SelectionChanged);
    if (!Board::isOnBoard(tile)) return;
    auto it = std::ranges::find(highlightedTiles.begin(), highlightedTiles.end(), tile);
    if (it != highlightedTiles.end()) {
        highlightedTiles.erase(it);
        return;
    }

    highlightedTiles.push_back(tile);
}

/// Implementacja notifySelectionChanged
void GameManager::notify(const GameEvent event) const {
    for (auto* observer : this->observers) {
        observer->onGameEvent(event);
    }
}

/// Implementacja onAfterMove
void GameManager::onAfterMove() {
    selectedTile = {-1,-1};
    lastMove = game.getLastMove();
    notify(GameEvent::BoardChanged);
    draggingPiece.active = false;

    if (game.getBoard().isPromoting() != -1) {
        setState(GameState::Promoting);
    }
    else {
        setState(GameState::Idle);
        botInstance.updateBotBoard(game.getLastMove().value());
    }

    if (game.isGameOver() != Game::EndReason::None) {
        setState(GameState::GameOver);
        notify(GameEvent::GameOver);
        setLocalPlayer(Game::Player::None);
        botInstance.destroyBot();
    }
}

/// Implementacja metody ustawiającej wybrane pole
void GameManager::onLeftClicked(const std::pair<int, int> & tile) {
    switch (state) {
        case GameState::Idle: {
            handleIdleLeftClick(tile);
            break;
        }
        case GameState::PieceSelected: {
            handleSelectionLeftClick(tile);
            break;
        }
        case GameState::GameOver: {
            highlightedTiles.clear();
            notify(GameEvent::SelectionChanged);
        }
        default: break;
    }
}

/// Implementacja onLeftReleased
void GameManager::onLeftReleased(const std::pair<int, int> & tile) {
    if (state != GameState::Dragging) return;

    Move moveToMake = draggingPiece.onStopDragging(tile);

    notify(GameEvent::SelectionChanged);
    if (moveToMake.getPosition() == moveToMake.getNextPosition()) {
        if (!draggingPiece.dontDeselect) {selectedTile = {-1,-1}; setState(GameState::Idle);}
        else setState(GameState::PieceSelected);
        return;
    }
    if (localPlayer == game.getCurrentPlayer()) {
        if (game.tryMakeMove(moveToMake)) {
            onAfterMove();
            return;
        }
    }
    setState(GameState::PieceSelected);
}

/// Implementacja onRightClicked
void GameManager::onRightClicked(const std::pair<int, int> & tile) {
    switch (state) {
        case GameState::PieceSelected: {
            selectedTile = {-1,-1};
            handleRightClick(tile);
            break;
        }
        case GameState::Idle:
            handleRightClick(tile);
            break;
        case GameState::Dragging: {
            draggingPiece.active = false;
            selectedTile = {-1,-1};
            setState(GameState::Idle);
            notify(GameEvent::SelectionChanged);
        }
        case GameState::GameOver: {
            handleRightClick(tile);
            break;
        }
        default: break;
    }
}

/// Implementacja onPromotionChosen
void GameManager::onPromotionChosen(const PieceType choice) {
    if (choice != PieceType::None) {
        game.handlePromotion(false, choice);
    }
    else {
        game.handlePromotion(true);
    }

    onAfterMove();
}

/// Implementacja setLocalPlayer
void GameManager::setLocalPlayer(const Game::Player localPlayer) {
    this->localPlayer = localPlayer;
    game.setCurrentPlayer();
}

void GameManager::setBot(std::unique_ptr<IBot> bot) {
    auto boardCopy = game.copyBoard();
    botInstance.initializeBot(std::move(bot), std::move(boardCopy));
}

/// Implementacja metody zwracającej obecnie wybrane pole
const std::pair<int,int> & GameManager::getSelectedTile() const {
    return selectedTile;
}

/// Implementacja getHighlightedTiles
const std::vector<std::pair<int, int>>& GameManager::getHighlightedTiles() const {
    return highlightedTiles;
}

/// Zwraca true, jeśli pole jest wybrane i false w przeciwnym wypadku
bool GameManager::isTileSelected() const {
    return selectedTile.first != -1 && selectedTile.second != -1;
}

/// Zwraca referencję do instancji gry
const Game& GameManager::getGame() const {
    return this->game;
}

/// Zwraca referencję do przeciąganego obiektu
const GameManagerUtils::DraggingPiece & GameManager::getDraggingState() const {
    return this->draggingPiece;
}

/// Implementacja getState
GameState GameManager::getState() const {
    return state;
}

int GameManager::getPromotionFile() const {
    if (game.getBoard().isPromoting() == -1) return -1;
    const bool flag = game.getBoard().isWhitesTurn();
    const int promotionFile = (flag ? 0 : 7) * 8 + game.getBoard().isPromoting();
    return promotionFile;
}

std::optional<Move> GameManager::getLastMove() const {
    return lastMove;
}

bool GameManager::isLocalPlayersTurn() const {
    return localPlayer == game.getCurrentPlayer();
}

/// Implementacja selectedPieceMoves
std::vector<GameManager::MoveHint> GameManager::selectedPieceMoves() const {
    if (localPlayer != game.getCurrentPlayer()) return {};
    std::vector<MoveHint> moves;
    std::pair lastPromotionDestination = {-1,-1};

    if (const auto [fst, snd] = this->selectedTile; fst != -1) {
        if (!game.getBoard().getMoves().has_value()) return moves;
        for (const auto & allMoves = game.getBoard().getMoves().value(); const auto & move : allMoves) {
            if (move.getPosition() == std::pair{fst, snd}) {
                if (move.getNextPosition() == lastPromotionDestination) continue;
                moves.emplace_back(move.hasMoveFlag(Capture), move.getNextPosition());
                if (move.hasMoveFlag(Promotion)) {
                    lastPromotionDestination = move.getNextPosition();
                }
            }
        }
    }
    return moves;
}

std::vector<GameManager::PieceInfo> GameManager::piecesOnBoard() const {
    std::vector<PieceInfo> pieces;
    const Board & board = game.getBoard();
    for (int8 i = 0; i < Board::BOARD_SIZE_X; i++) {
        for (int8 j = 0; j < Board::BOARD_SIZE_Y; j++) {
            if (const Piece* piece = board.getPiece(i,j); piece != nullptr) {
                pieces.emplace_back(std::pair{i,j}, piece->coloredSymbol());
            }
        }
    }
    return pieces;
}

std::bitset<12> GameManager::needsUpdate() const {
    return game.getOnBoardChanges();
}

void GameManager::update(float) {
    if (botInstance.botState == BotState::Initialized && game.getCurrentPlayer() != localPlayer) {
        if (game.getBoard().isPromoting() == -1) {
            botInstance.startBotThinking();
        }
    }
    if (botInstance.botState == BotState::Thinking && botInstance.botFuture.valid()) {
        botInstance.tryGetBotMove();
    }

    if (botInstance.botState == BotState::MoveReady && botInstance.pendingBotMove.has_value()) {
        game.makeMove(botInstance.pendingBotMove.value());
        onAfterMove();
        if (getState() != GameState::GameOver) {
            botInstance.onMoveMade();
        }
    }
}







