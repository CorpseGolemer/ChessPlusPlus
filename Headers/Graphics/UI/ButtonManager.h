#pragma once
#include "Button.h"
#include "../Resources.h"
#include "SFML/Graphics/RenderWindow.hpp"

class ButtonManager {

    struct VertexArrayIdentifier {
        std::string id;
        unsigned int textSize;

        bool operator==(const VertexArrayIdentifier & other) const {
            return id == other.id && textSize == other.textSize;
        }
    };
    struct VertexArrayIdentifierHash {
        std::size_t operator()(const VertexArrayIdentifier & k) const {
            return std::hash<std::string>()(k.id) ^
                (std::hash<unsigned int>()(k.textSize) << 1);
        }
    };
    struct VertexArrayObject {
        sf::VertexArray vertexArray;
        bool needsUpdate = true;
        bool updating = false;
    };

    // ===== POLA KLASY =====

    /// \brief Kontekst aplikacji
    AppContext& context;
    /// \brief Mapa trzymająca przyciski i ich identyfikatory
    std::unordered_map<std::string, std::unique_ptr<Button>> buttons;
    /// \brief Vertex Array, który rysuje background przycisków
    sf::VertexArray backgroundArray;
    /// \brief Wektor tekstów (jestem zbyt głupi (czytać student), żeby zrobić vertex array dla tekstu)
    std::unordered_map<VertexArrayIdentifier, VertexArrayObject, VertexArrayIdentifierHash> textMap;
    /// \brief Mapa trzymająca style przycisków
    std::vector<Button::ButtonStyle> buttonStyles;
    /// \brief Flaga, która mówi, czy trzeba zaktualizować array przycisków
    bool needsVertexUpdate = true;

    // ===== METODY PRYWATNE =====

    /// Aktualizuje vertex array przycisków
    void updateBackgroundsVertices();

    /// Aktualizuje vertex array teksów
    void updateTextVertices();

public:
    /// Konstruktor
    explicit ButtonManager(AppContext& context);
    /// Destruktor
    ~ButtonManager();

    // ===== METODY PUBLICZNE =====
    /// Metoda zajmuje się sprawdzeniem, czy któryś z przycisków w labels nie zjadł kliknięcia myszy
    bool handleButtonEvent(const sf::Event& event, const sf::Vector2f& mouseCoords);
    /// Metoda tworzy nowy styl przycisków
    void addButtonStyle(const Button::ButtonStyle& style);
    /// Metoda tworzy nowy przycisk
    void addButton(const std::string& name, const Button::ButtonData& data, Button::ButtonStyle& style);
    /// Metoda usuwa przycisk
    void removeButton(const std::string &name);
    /// Metoda czyści dane obecnych przycisków (razem ze stylami)
    void clearAllButtons();
    /// Metoda rysuje przyciski
    void drawButtons(sf::RenderTarget& window) const;
    /// Aktualizuje przyciski
    void update();
    /// Podaje ButtonStyle o konkretnym id
    Button::ButtonStyle* getButtonStyle(const std::string &name);
    /// Dodaje tekst danego przycisku do VertexArray
    static void handleButtonText(sf::VertexArray& vertices, const Button& button);
    /// Aktualizuje stan przycisku z zewnątrz
    void updateButtonState(const std::string& id, Button::State newState);
    /// Aktualizuje widoczność przycisku
    void updateButtonVisibility(const std::string& id, bool visible);
    /// Dodaje GlyphQuad
    static void addGlyphQuad(sf::VertexArray& vertices, sf::Vector2f position, sf::Color color, const sf::Glyph& glyph, float italicShear);

};
