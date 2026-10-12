#include <iostream>

#include "../Headers/App.h"
#include "../Headers/Game/Zobrist.h"

constexpr std::string_view projectName = "ChessPlusPlus";
constexpr std::string_view version = "0.1";

int main() {
    Zobrist::init();
    Resources::get().loadChessResources();
    sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode()), "Chess Game", sf::State::Fullscreen);
    window.setFramerateLimit(144);

    const auto app = std::make_unique<App>(window);

    sf::Sprite background(Resources::get().getTexture("background"));
    background.setScale(sf::Vector2f(0.75f, 0.675f));

    sf::Clock clock;

    while (window.isOpen()) {
        const float deltaTime = clock.restart().asSeconds();
        // Reagowanie na zdarzenia
        while (const std::optional event = window.pollEvent()) {
            // Sprawdź, czy zamknięcie
            if (event->is<sf::Event::Closed>()) {
                window.close();
                continue;
            }

            // Przekaż zdarzenie do App
            app->handleEvent(*event, window);
        }

        // Updating cache of renderer
        app->update(deltaTime);

        // Cykl rysowniczy
        window.clear();
        window.draw(background);
        app->drawApp(window);
        window.display();
    }
    return 0;
}
