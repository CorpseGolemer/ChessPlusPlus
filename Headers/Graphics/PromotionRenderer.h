#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include <cmath>
#include "Resources.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"

enum class PieceType : uint8_t;

class PromotionRenderer {

    // ===== STAŁE POLA KLASY =====

    /// Informacje o granicach animacji
    const float maxSize = 100.f, minSize = maxSize / static_cast<float>(sqrt(2)) - 5.f,
    maxDeg = 90.f, minDeg = 45.f,
    maxOutline = 3.f, minOutline = 0.f,
    maxPieceScale = 0.88f, minPieceScale = 0.55f;
    /// Informacja o rozmiarze jednego pola
    const float tileSize, diamondSize = tileSize / static_cast<float>(sqrt(2));

    // ===== PRYWATNE STRUCTY =====

    /// Przechowuje obecny stan animacji diamentu promocji
    struct PromotionAnim {
        bool active = false;
        bool needsAnimation = false;
        float currentAngle = 45.f;
        float currentSize = 100.f / static_cast<float>(sqrt(2)) - 5.f;
        float currentOutline = 3.f;
        float currentPieceScale = 0.55f;
    };

    // ===== POLA KLASY =====

    /// Array diamentów
    std::array<sf::RectangleShape, 4> diamonds = {
        sf::RectangleShape({minSize, minSize}),
        sf::RectangleShape({minSize, minSize}),
        sf::RectangleShape({minSize, minSize}),
        sf::RectangleShape({minSize, minSize}),
    };
    /// Sprite'y figur w diamentach
    std::array<sf::Sprite, 8> diamondSprites = {
        sf::Sprite(Resources::get().getTexture("bQ")),
        sf::Sprite(Resources::get().getTexture("wQ")),
        sf::Sprite(Resources::get().getTexture("bN")),
        sf::Sprite(Resources::get().getTexture("wN")),
        sf::Sprite(Resources::get().getTexture("bR")),
        sf::Sprite(Resources::get().getTexture("wR")),
        sf::Sprite(Resources::get().getTexture("bB")),
        sf::Sprite(Resources::get().getTexture("wB")),
    };
    std::array<sf::Vector2f, 4> diamondPositions;
    /// Informacja o tym kto promuje
    bool whitePromotes = true;
    /// Przechowuje stan animacji każdego z diamentów
    std::array<PromotionAnim, 4> diamondAnim;

    // ===== PRYWATNE METODY KLASY =====

    /// Metoda sprawdza, czy myszka znajduje się nad którymś z diamentów i jeśli tak to zwraca jego numer
    int isMouseOverDiamond(sf::Vector2f mouseCoords) const;
    /// Metoda centruje origin diamentu
    void centerDiamond(int index);
    /// Metoda centruje origin diamentu
    static void centerDiamond(sf::RectangleShape& diamond);

public:
    /// Konstruktor
    explicit PromotionRenderer(float tileSize);

    // ===== PUBLICZNE METODY KLASY

    /// Metoda obsługuje kliknięcie (jednorazowo, jeżeli aktywna promocja)
    PieceType consumeClick(sf::Vector2f mousePos) const;

    /// Metoda reaguje na hover, jeżeli jest aktywne okno promocji
    void handleHover(sf::Vector2f position);

    /// Metoda aktualizuje pozycje rysowania diamentów
    void updatePositions(sf::Vector2f firstDiamondPosition, bool whitesPromotes);

    /// Metoda zajmuje się animowaniem diamentów
    void doDiamondAnimation(float deltaTime);

    /// Metoda rysuje diamenty w oknie
    void drawAllDiamond(sf::RenderWindow& window) const;

    /// Aktywuje animacje pierwszego diamentu
    void activate(const sf::Vector2f& mousePos);
    /// Odwrót activate
    void deactivate(const sf::Vector2f& mousePos);

    /// Metoda podaje pierwszy diament
    PromotionAnim & getFirstDiamond();
};
