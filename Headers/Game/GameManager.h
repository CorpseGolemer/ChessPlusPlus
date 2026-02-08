#pragma once

#include <utility>
#include "Game.h"
#include "IGameObserver.h"
#include "../Bots/IBot.h"
#include "../Utils/GameManagerUtils.h"

enum class GameState {
    Idle,
    PieceSelected,
    Dragging,
    Promoting,
    GameOver
};

class GameManager {

    // ===== POLA KLASY =====

    /// Wybrane pole szachownicy
    std::pair<int, int> selectedTile = {-1,-1};
    /// Czy dane pole jest podświetlone
    std::vector<std::pair<int, int>> highlightedTiles{};
    /// Ostatni ruch
    std::optional<Move> lastMove;
    /// Instancja gry
    Game game;
    /// Kto gra na tym komputerze (boty mają swoje)
    Game::Player localPlayer = Game::Player::None;
    /// Obiekt struktury DraggingPiece przechowuje infromacje o ciągniętej bierce
    GameManagerUtils::DraggingPiece draggingPiece;
    /// Stan
    GameState state = GameState::Idle;
    /// Wektor obserwatorów
    std::pmr::vector<IGameObserver*> observers;
    /// Struktura bota
    GameManagerUtils::BotInstance botInstance;

    // ===== PRYWATNE METODY =====

    /// Ustawia nowy stan
    void setState(GameState newState);
    /// Logika kliknięcia lewego przycisku, jeżeli selectedTile = {-1,-1}
    void handleIdleLeftClick(const std::pair<int, int> &tile);
    /// Logika kliknięcia lewego przycisku, jeżeli selectedTile != {-1,-1}
    void handleSelectionLeftClick(const std::pair<int, int> &tile);
    /// Logika prawego kliknięcia
    void handleRightClick(const std::pair<int, int> &tile);
    /// Powiadomienie obserwatorów o zmianach
    void notify(GameEvent event) const;
    /// Wykonuje potrzebne operacje po ruchu
    void onAfterMove();

public:
    static constexpr std::array<PieceType, 4> promotionWindowOrder = {PieceType::Queen, PieceType::Knight, PieceType::Rook, PieceType::Bishop};

    /// Konstruktor
    GameManager() = default;

    // ===== METODY ZAJMUJĄCE SIĘ OBSŁUGĄ KLIKNIĘĆ =====

    /// Metoda odpowiadająca na kliknięcie lewego przycisku myszy (ustawia pole jako zaznaczone, wysyła informację, że gracz próbuje wykonać ruch)
    void onLeftClicked(const std::pair<int, int> &tile);
    /// Metoda zajmuje się sytuacją, kiedy puścimy lewy przycisk myszy
    void onLeftReleased(const std::pair<int, int> &tile);
    /// Metoda odpowiadająca za kliknięcie prawego przycisku myszy (podświetla pola)
    void onRightClicked(const std::pair<int, int> &tile);
    /// Metoda mówi jaką figurę promocyjną gracz wybrał i czy w ogóle coś wybrał
    void onPromotionChosen(PieceType choice);
    /// Metoda ustawia localPlayer
    void setLocalPlayer(Game::Player localPlayer);
    /// Metoda ustawia bota
    void setBot(std::unique_ptr<IBot> bot);

    // ===== GETTERY =====

    /// Metoda zwracająca obecnie wybrane pole
    [[nodiscard]] const std::pair<int, int> &getSelectedTile() const;
    /// Metoda zwraca array podświetlonych pól
    [[nodiscard]] const std::vector<std::pair<int, int>> & getHighlightedTiles() const;
    /// Metoda sprawdza, czy obecnie wybrano jakieś pole
    [[nodiscard]] bool isTileSelected() const;
    /// Metoda zwraca referencję do instancji gry
    const Game & getGame() const;
    /// Zwraca referencje do przeciąganego obiektu
    const GameManagerUtils::DraggingPiece & getDraggingState() const;
    /// Zwraca UIState
    GameState getState() const;
    /// Zwraca pole promocyjne
    int getPromotionFile() const;// -1, jeśli nie ma
    /// Zwraca ostatni ruch
    std::optional<Move> getLastMove() const;
    /// Zwraca, którym kolorem gramy
    bool isLocalPlayersTurn() const;

    // ===== PUBLICZNE METODY OBSERWATORÓW =====

    /// Dodaje podany observer do wektora
    void addObserver(IGameObserver *observer) {observers.push_back(observer);}
    /// Usuwa podany observer z wektora
    void removeObserver(IGameObserver *observer) {std::erase(observers, observer);}

    // ===== POMOCNICZE =====

    struct MoveHint {
        bool capture = false;
        std::pair<int8, int8> pos = {-1, -1};
    };

    /// Liczy możliwe ruchy obecnie zaznaczonej bierki
    std::vector<MoveHint> selectedPieceMoves() const;

    struct PieceInfo {
        std::pair<int8, int8> pos = {-1, -1};
        unsigned char coloredSymbol = 12;
    };

    /// Daje wektor z pozycjami bierek
    std::vector<PieceInfo> piecesOnBoard() const;
    /// Info, jakie bierki potrzebują aktualizacji
    std::bitset<12> needsUpdate() const;

    // ===== INNE =====

    void update(float deltaTime);
};
