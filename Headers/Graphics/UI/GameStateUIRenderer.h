#pragma once
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

class GameStateUIRenderer {

    /// \brief Lewy panel wyświetlany podczas gry
    sf::RectangleShape leftSideBar = sf::RectangleShape({140.f, 1080.f});

public:
    /// Konstruktor
    GameStateUIRenderer();

    /// \brief Funckja rysująca UI w stanie gry
    /// \param window Render Target
    void drawGameUI(sf::RenderWindow &window) const;
};
