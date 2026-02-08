#pragma once
#include "ButtonManager.h"
#include "GameStateUIRenderer.h"
#include "../../AppContext.h"
#include "SFML/Graphics/RenderWindow.hpp"

class UIManager {
    // ===== POLA KLASY =====

    /// Rysuje UI podczas gry
    GameStateUIRenderer gameStateUIRenderer;
    /// Manager guzików
    ButtonManager buttonManager;
    /// Referencja do kontekstu aplikacji
    AppContext& context;

public:
    explicit UIManager(AppContext& context);

    // ===== METODY ZARZĄDZANIA UI =====

    /// \brief Metoda, która odpowiada na event i jeżeli zostanie on skonsumowany zwraca true
    /// \param event Wychwycony typ eventu
    /// \param window Okno, w którym jest rysowany program
    [[nodiscard]] bool handleEvent(const sf::Event& event, const sf::RenderWindow& window);

    /// \brief Metoda rysująca całe UI w oknie
    void drawUI(sf::RenderWindow& window) const;

    /// \brief Aktualizuje stan UI
    void update();

    /// \brief Tworzy przyciski w stanie gry
    void createGameStateButtons();

};
