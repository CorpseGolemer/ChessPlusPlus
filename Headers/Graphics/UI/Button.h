#pragma once
#include <functional>
#include "../Resources.h"
#include "../../AppContext.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"

class AppContext;
/// Klasa przycisku
class Button {

public:
    /// Tekst tylko do obliczeń
    static sf::Text measurementText;

    // ===== STRUCTY =====

    /// Enum stanów przycisku
    enum class State: uint8_t {
        Normal,
        Hovered,
        Pressed,
        Disabled
    };

    /// Styl przycisku
    struct ButtonStyle {
        std::string id;
        unsigned int textSize = 24;
        sf::Color textColor = sf::Color::White;

        sf::Color normalColor = sf::Color::Transparent;
        sf::Color hoveredColor = sf::Color::Transparent;
        sf::Color pressedColor = sf::Color::Transparent;
        sf::Color disabledColor = sf::Color::Transparent;
        float letterSpacing = 1.f;
        float lineSpacing = 1.f;
        std::string fontName = "ARIALBD";

        static const ButtonStyle Default;

        bool operator==(const ButtonStyle & o) const {
            return this->id == o.id;
        }
    };

    /// Informacje potrzebne do stworzenia przycisku
    struct ButtonData {
        sf::Vector2f size = {100.f,100.f};
        sf::Vector2f position = {0.f,0.f};
        sf::String label = "Button";
        std::function<void(AppContext&)> action;
        State state = State::Normal;
        bool visible = true;
    };

    /// HandleEvent zwraca
    struct HandleFlags {
        bool consumed = false;
        bool needsUpdate = false;
        bool clicked = false;
    };

private:

    // ===== POLA KLASY =====

    /// Dane przycisku
    ButtonData buttonData;
    /// Wskaźnik do stylu przycisku
    ButtonStyle* buttonStyle;

    /// Informacja czy mysz jest nad przyciskiem
    [[nodiscard]] bool contains(const sf::Vector2f& mouseCoords) const;

public:
    /// Konstruktor
    explicit Button(const ButtonData& buttonData, ButtonStyle& buttonStyle);

    // ===== SETTERY =====

    /// Zmienia pozycję przycisku
    void setPosition(const sf::Vector2f& position);
    /// Ustawia tekst
    void setText(const sf::String& text);
    /// Ustawia kolor tekstu (domyślnie biały)
    void setTextColor(const sf::Color& color = sf::Color::White) const;
    /// Ustawia pole enabled
    bool setState(State newState);
    /// Ustawia callback do funkcji (tutaj w przypadku kliknięcia)
    void setCallback(const std::function<void(AppContext&)>& callback);
    /// Ustawia rozmiar przycisku
    void setSize(const sf::Vector2f& size);
    /// Ustawia aktywność przycisku
    void setVisible(bool visible);
    /// Ustawia paletę kolorów przycisku
    void setColors(const sf::Color& normal, const sf::Color& hovered,
        const sf::Color& pressed, const sf::Color& disabled) const;
    /// Ustawia rozmiar tekstu
    void setTextSize(unsigned char size) const;

    // ===== GETTERY =====

    /// Pobiera obecny kolor
    [[nodiscard]] sf::Color getCurrentColor() const;
    /// Pobiera referencję do daty
    [[nodiscard]] const ButtonData& getButtonData() const;
    /// Pobiera referencję do stylu
    [[nodiscard]] const ButtonStyle& getButtonStyle() const;

    // ===== FUNKCJE DZIAŁANIA PRZYCISKU =====

    /// Obsługa zdarzeń
    HandleFlags handleEvent(const sf::Event& event, const sf::Vector2f& mouseCords);

    // ===== POMOCNICZE =====
    /// Oblicza pozycję tekstu na podstawie pozycji przycisku
    [[nodiscard]] sf::Vector2f calculateTextPosition() const;

};
