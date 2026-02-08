#pragma once

#include <array>

#include "PromotionRenderer.h"
#include "Resources.h"
#include "../Game/GameManager.h"
#include "../Game/IGameObserver.h"
#include "SFML/Graphics.hpp"

class GameManager;

/// Klasa odpowiadająca za rysowanie cache'owanie informacji z GameManager oraz wyrysowywanie wszystkiego w oknie aplikacji
class BoardRenderer final : public IGameObserver {

    // ===== ENUMY =====

    /// ENUMERATOR TRZYMAJĄCY INFORMACJĘ CO TRZEBA ZRESETOWAĆ W CACHE
    enum DirtyLevel : uint8_t{
        /// Cache jest aktualny, nie wymaga żadnych zmian
        CLEAN = 0,
        /// Zmiana hover
        HOVER_CHANGE = 1 << 0,
        /// Zmiana selekcji
        SELECTION_CHANGE = 1 << 1,
        /// Cały cache do zresetowania
        CACHE_RESET = 1 << 2,
        /// Koniec gry
        GAME_OVER = HOVER_CHANGE | SELECTION_CHANGE | CACHE_RESET
    };

    // ===== STAŁE POLA KLASY =====

    /// Stałe zmiennoprzecinkowe, które określają, gdzie zaczyna się rysowanie szachownicy i wymiary pola (podawane od App)
    constexpr static float TILE_SIZE = 100.f;
    constexpr static auto BOARD_ORIGIN = sf::Vector2f(400.f, 940.f), OFFSET = sf::Vector2f(TILE_SIZE / 2.f, -TILE_SIZE / 2.f);
    constexpr static std::string DESCRIPTION_FONT = "ARIALBD";
    /// Stałe zmiennoprzecinkowe, które określają, rozmiar kółek podpowiedzi
    const float radius = TILE_SIZE * 0.18f, superRadius = TILE_SIZE * 0.4f;
    /// Tablica pozycji (nie musimy ich ciągle przeliczać)
    const std::array<std::array<sf::Vector2f, 8>, 8> TILE_POSITIONS = [this]() {
        std::array<std::array<sf::Vector2f, 8>, 8> positions{};
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                positions[i][j] = convertTileToPixel({i,j});
            }
        }
        return positions;
    }();
    /// !!!NIE STAŁE!!!! ZMNIEJSZA MI PADDING TUTAJ PO PROSTU XD
    bool isGameOver = false;
    /// Referencja do GameManagera
    const GameManager & gameManager;

    // ===== POLA KLASY =====

    /// Bazowe kolory pól szachownicy
    std::array<std::array<sf::Color, 8>, 8> tileColors;
    /// Jeśli na szachownicy jest białe, to tutaj jest czarne (litery i hover)
    std::array<std::array<sf::Color, 8>, 8> swappedTileColors;
    /// Kwadrat, który reprezentuje podświetlenie pola (addQuad dodaje tylko zwykłe proste kwadraty, więc odpada w tym miejscu)
    sf::RectangleShape tileHover = sf::RectangleShape(sf::Vector2f(TILE_SIZE * 0.9f, TILE_SIZE * 0.9f));
    /// Rysuje przyciemnienie szachownicy
    sf::RectangleShape windowDim = sf::RectangleShape(sf::Vector2f(TILE_SIZE * 8.f, TILE_SIZE * 8.f));
    /// Trzyma draggingSprite
    sf::Sprite draggingSprite = sf::Sprite(Resources::get().getTextureByIndex(0));
    /// Obiekt klasy PromotionRenderer, która zajmuje się rysowaniem stanu podczas promocji
    PromotionRenderer promotionRenderer;

    // === PRYWATNE STRUCTY =====

    /// Struct trzyma wszystkie VertexArray
    struct RenderVertices {
        /// Za pierwszym razem dodajemy tu pola
        sf::VertexArray tiles = sf::VertexArray(sf::PrimitiveType::Triangles);
        /// Za pierwszym razem, opisy kolumn i wierszy
        sf::VertexArray tileDescriptions = sf::VertexArray(sf::PrimitiveType::Triangles);
        /// To samo dla efektów na polach
        sf::VertexArray tileEffects = sf::VertexArray(sf::PrimitiveType::Triangles);
        /// Mapa VertexArray dla figur
        std::unordered_map<int, sf::VertexArray> piecesMap;
        /// VertexArray dla podpowiedzi
        sf::VertexArray hints = sf::VertexArray(sf::PrimitiveType::Triangles);

        struct RenderStates {
            sf::RenderStates tileDescStates = sf::RenderStates::Default;
            std::array<sf::RenderStates, 12> piecesStates;

            void initializeStates();
        } states {};
    } vertices {};

    /// Struct przechowujący stale zmieniające się infromacje
    struct RenderCache {
        std::optional<GameEvent> event = GameEvent::BoardChanged;
        bool lastDraggingActive = false;
        std::vector<GameManager::PieceInfo> piecesOnBoard;
        struct Hover {
            bool drawHover = false;
            std::pair<int, int> hoverTile = {-1,-1};
            sf::Vector2f hoverPosition = {-1.f, -1.f};
        } hover;
        std::pair<int, int> lastHoveredTile = {-1,-1};
        std::vector<GameManager::MoveHint> possiblePieceMoves;
        bool promoting = false;
    } cache {};

public:
    /// Konstruktor boardRenderer
    explicit BoardRenderer(const GameManager & gameManager);

    // ===== OVERRIDE =====

    void onGameEvent(GameEvent event) override;

    // ===== GETTERY I SETTERY =====

    /// Funckja podaje promotionRenderer
    PromotionRenderer & getPromotionRenderer();

    // ===== FUNKCJE ZAJMUJĄCA SIĘ OBSŁUGĄ MYSZY =====

    /// Metoda reaguje na ruch myszy
    void onMouseMoved(sf::Vector2f mousePos);

    // ===== FUNKCJE KONWERSJI =====

    /// Zmienia pozycje, w pikselach, klikniętą przez mysz na pozycje na szachownicy
    [[nodiscard]] static std::pair<int, int> convertPixelToTile(int x, int y) ;
    /// Zmienia pozycję na szachownicy, na pozycję w pikselach
    [[nodiscard]] static sf::Vector2f convertTileToPixel(const std::pair<int, int>& tile) ;
    /// Dodaje offset centrujący pozycję dla pozycji tile
    static sf::Vector2f tileCenter(const sf::Vector2f & tilePos);

    // ===== FUNKCJE ZARZĄDZANIA CACHE =====

    /// Aktualizuje cache
    void update();
    /// Modyfikuje cache po przesunięciu myszy (zmiana hovera)
    void handleHoverChange();
    /// Modyfikuje cache po kliknięciu myszą (inaczej zmiana selekcji)
    void handleSelectionChange();
    /// Modyfikuje cache po dużych zmianach na planszy (np. ruch)
    void handleBoardChanged();
    /// Zmienia stan czystości cache
    void notifyChange(GameEvent event);

    // ===== FUNKCJE RYSOWNICZE =====

    /// Funkcja rysuje cały obecny stan planszy
    void draw(sf::RenderWindow& window);
    /// Metoda rysuje pustą szachownicę
    void updateBoard();
    /// Metoda rysuje efekty na polach (selekcja, ostatni ruch, podświetlenie)
    void updateTileEffects();
    /// Metoda rysuje figury na odpowiednich polach
    void updatePieces();
    /// Metoda rysuje podpowiedzi do ruchów na odpowiednich polach
    void updateHints();
    /// Metoda rysuje ciągniętą figurę
    void updateDraggedPiece();
    /// Metoda odpowiada za rysowanie okna promocji
    void updatePromotionWindow();
    /// Metoda rysuje animacje
    void updateAnimations(float deltaTime);

    // ===== POMOCNICZE FUNKCJE RYSOWNICZE =====

    /// Funckja dodaje pole do VertexArray (rysowanie pól, selekcji, highlight, lastMove)
    static void addQuad(sf::VertexArray & vertices, const sf::Vector2f& position, const sf::Color& color, const sf::Vector2f& size = {TILE_SIZE, TILE_SIZE});
    /// Funckja dodaje figurę do jej VertexArray
    static void addPiece(sf::VertexArray& vertices, const sf::Vector2f& pos, const sf::Texture& tex, bool isDraggedPiece = false,
        const sf::Vector2f& size = {105.f * 0.88f, 105.f * 0.88f}, const sf::Vector2f& tileSize = {TILE_SIZE, TILE_SIZE});
    /// Funckja dodaje kółko do VertexArray
    static void addCircle(sf::VertexArray& vertices, const sf::Vector2f& pos, sf::Color color,
        float radius, float outlineThickness = 0.f, sf::Color outlineColor = sf::Color::Transparent, int pointCount = 30);
};
