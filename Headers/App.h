#ifndef APP_H
#define APP_H

#include "Game/GameManager.h"
#include "Graphics/BoardRenderer.h"
#include "Graphics/UI/UIManager.h"
#include "AppContext.h"

class GameManager;
class Piece;

/// Hypervisor programu
class App {

    // ===== POLA KLASY =====

    /// Odpowiada za rysowanie planszy
    BoardRenderer renderer;
    /// Główny supervisor logiczny
    GameManager gameManager;
    /// Odpowiada za rysowanie UI oraz za jego działanie
    UIManager uiManager;
    /// Odpowiada za callbacki
    AppContext context;

public:
    /// Konstruktor
    explicit App(sf::RenderWindow& window);
    /// Destruktor
    ~App();

    // ===== GETTERY =====

    /// Podaje renderer
    BoardRenderer& getRenderer();
    /// Podaje gameManager
    GameManager& getGameManager();
    /// Podaje kontekst aplikacji
    AppContext& getContext();


    // ===== FUNKCJE ZAJMUJĄCE SIĘ OBSŁUGĄ KLIKNIĘĆ =====

    void handleEvent(const sf::Event& e, const sf::RenderWindow& window);
    /// Jeśli UI nie wychwyci kliknięcia, to zostaje ono oddane GameManagerowi (i może aktualizować coś w BoardRenderer)
    void handleGameLogic(const sf::Event& e, const sf::Vector2i& mousePos);
    /// Metoda, która zajmuje się pobraniem input myszy i podaniem go dalej
    void handleClick(const sf::Event::MouseButtonPressed & mouseButton);
    /// Metoda, która zajmuje się puszczeniem klawisza myszy
    void handleRelease(const sf::Event::MouseButtonReleased & mouseButton);
    /// Metoda, która odpowiada na ruch myszą
    void handleMouseMovement(int x, int y);

    /// Funkcja rysująca wszystko, co należy narysować
    void drawApp(sf::RenderWindow& window);
    /// Funckja aktualizuje stany
    void update(float deltaTime);

};

#endif //APP_H
