#pragma once
#include "SFML/Graphics/RenderWindow.hpp"

class GameManager;
class UIManager;

/// Klasa, trzymająca kontekst apki
class AppContext {
public:
    sf::RenderWindow& window;
    GameManager& gameManager;
    UIManager& uiManager;

    AppContext(sf::RenderWindow& window, GameManager& gameManager, UIManager& uiManager) : window(window), gameManager(gameManager), uiManager(uiManager) {}
};