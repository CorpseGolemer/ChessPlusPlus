#pragma once

#include "SFML/Graphics/Color.hpp"

/// Klasa trzyma informacje o używanych kolorach
class Colors {
    public:

    // ===== POLA KLASY =====

    /// Podstawowe kolory pól
    static constexpr auto lightSquare = sf::Color(240,240,225);
    static constexpr auto darkSquare = sf::Color(118,150,86);

    /// Efekty na polach
    static constexpr auto selected = sf::Color(255,255,80,160);
    static constexpr auto lastMove = sf::Color(185, 155, 85, 120);
    static constexpr auto highlighted = sf::Color(255, 200, 90, 200);
    static constexpr auto hint = sf::Color(0, 0, 0, 60);

    /// Stan promocji
    static constexpr auto windowDim = sf::Color(0, 0, 0, 120);
    static constexpr auto diamondColor = sf::Color(210,215,205, 255);
    static constexpr auto diamondOutlineColor = sf::Color(0, 0, 0, 160);
    static constexpr auto diamondHoveredColor = sf::Color(220, 240, 100);

    /// Przyciski
    static constexpr auto normalButtonColor = sf::Color(70,70,70);
    static constexpr auto hoveredButtonColor = sf::Color(100,100,100);
    static constexpr auto pressedButtonColor = sf::Color(50,50,50);
    static constexpr auto disabledButtonColor = sf::Color(40,40,40);


    // ===== METODY KLASY =====

    /// Funkcja podaje bazowy kolor pola po podaniu jego współrzędnych
    static sf::Color getBaseTileColor(int i, int j, bool swap = false);

};
