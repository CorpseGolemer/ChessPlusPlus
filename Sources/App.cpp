#include "../Headers/App.h"
#include "../Headers/Game/GameManager.h"
#include <algorithm>

/// Implementacja konstruktora
App::App(sf::RenderWindow& window) :
    renderer(gameManager), uiManager(context), context(window, gameManager, uiManager) {
    // Renderer jako observer gry
    gameManager.addObserver(&renderer);
}

/// Implementacja destruktora
App::~App() = default;

/// Implementacja getRenderer
BoardRenderer& App::getRenderer() {
    return renderer;
}

/// Implementacja getGameManager
GameManager& App::getGameManager() {
    return gameManager;
}

AppContext& App::getContext() {
    return context;
}

void App::handleEvent(const sf::Event &e, const sf::RenderWindow &window) {
    if (uiManager.handleEvent(e, window)) return;
    handleGameLogic(e, sf::Mouse::getPosition(window));
}

void App::handleGameLogic(const sf::Event &e, const sf::Vector2i& mousePos) {
    e.visit([mousePos, this]<typename T0>(T0&& ev) {
        using T = std::decay_t<T0>;

        if constexpr (std::is_same_v<T, sf::Event::MouseButtonPressed>) {
            this->handleClick(ev);
        }

        else if constexpr (std::is_same_v<T, sf::Event::MouseButtonReleased>) {
            this->handleRelease(ev);
        }

        else if constexpr (std::is_same_v<T, sf::Event::MouseMoved>) {
            this->handleMouseMovement(mousePos.x, mousePos.y);
        }
    });
}

/// Pobiera input od main i przekazuje go GameManagerowi
void App::handleClick(const sf::Event::MouseButtonPressed & mouseButton) {
    const sf::Vector2f mousePos = {static_cast<float>(mouseButton.position.x),static_cast<float>(mouseButton.position.y)};

    if (gameManager.getState() == GameState::Promoting) {
        if (mouseButton.button == sf::Mouse::Button::Left) {
            const PieceType choice = renderer.getPromotionRenderer().consumeClick(mousePos);
            gameManager.onPromotionChosen(choice);
        }
        else {
            gameManager.onPromotionChosen(PieceType::None);
        }
        return;
    }

    const auto [fst, snd] = BoardRenderer::convertPixelToTile(mouseButton.position.x, mouseButton.position.y);

    if (mouseButton.button == sf::Mouse::Button::Left) {
        gameManager.onLeftClicked({fst, snd});
    }

    if (mouseButton.button == sf::Mouse::Button::Right) {
        gameManager.onRightClicked({fst, snd});
    }
}

/// Pobiera sygnał release od main i podaje go GameManagerowi
void App::handleRelease(const sf::Event::MouseButtonReleased &mouseButton) {
    const auto [fst, snd] = BoardRenderer::convertPixelToTile(mouseButton.position.x,mouseButton.position.y);
    if (mouseButton.button == sf::Mouse::Button::Left) {
        gameManager.onLeftReleased({fst, snd});
    }
}

/// Implementacja handleMouseMovement
void App::handleMouseMovement(const int x, const int y) {
    renderer.onMouseMoved(sf::Vector2f(static_cast<float>(x),static_cast<float>(y)));
}

void App::drawApp(sf::RenderWindow &window) {
    renderer.draw(window);
    uiManager.drawUI(window);
}

void App::update(const float deltaTime) {
    renderer.updateAnimations(deltaTime);
    renderer.update();
    uiManager.update();
    gameManager.update(deltaTime);
}






