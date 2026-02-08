#include "../../../Headers/Graphics/UI/UIManager.h"

#include "../../../Headers/Bots/ChciwyJakub.h"
#include "../../../Headers/Bots/Golemiarz.h"
#include "../../../Headers/Game/GameManager.h"

UIManager::UIManager(AppContext &context) : buttonManager(context), context(context) {
    createGameStateButtons();
}

bool UIManager::handleEvent(const sf::Event &event, const sf::RenderWindow &window) {
    const sf::Vector2f mouseCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (buttonManager.handleButtonEvent(event, mouseCoords)) return true;

    return false;
}

void UIManager::drawUI(sf::RenderWindow &window) const {
    gameStateUIRenderer.drawGameUI(window);
    buttonManager.drawButtons(window);
}

void UIManager::update() {
    buttonManager.update();
}

void UIManager::createGameStateButtons() {
    Button::ButtonStyle* buttonStyle = buttonManager.getButtonStyle("Default");
    Button::ButtonData buttonData {{120.f, 50.f}, {10.f, 1070.f}, "Exit App", [](const AppContext &context) {
        context.window.close();
    }};
    buttonManager.addButton("Exit", buttonData, *buttonStyle);
    buttonData = {{120.f, 50.f}, {10.f, 60.f}, "Bot 1", [this](const AppContext &context) {
        std::unique_ptr<IBot> bot = std::make_unique<ChciwyJakub>();
        context.gameManager.setBot(std::move(bot));
        this->buttonManager.updateButtonState("ChciwyJakub", Button::State::Disabled);
        this->buttonManager.updateButtonState("Golemiarz", Button::State::Disabled);
        this->buttonManager.updateButtonVisibility("White", true);
        this->buttonManager.updateButtonVisibility("Black", true);
    }, Button::State::Normal, true};
    buttonManager.addButton("ChciwyJakub", buttonData, *buttonStyle);
    buttonData = {{120.f,50.f}, {10.f, 110.f}, "Bot 2", [this](const AppContext &context) {
        std::unique_ptr<IBot> bot = std::make_unique<Golemiarz>();
        context.gameManager.setBot(std::move(bot));
        this->buttonManager.updateButtonState("Golemiarz", Button::State::Disabled);
        this->buttonManager.updateButtonState("ChciwyJakub", Button::State::Disabled);
        this->buttonManager.updateButtonVisibility("White", true);
        this->buttonManager.updateButtonVisibility("Black", true);
    }, Button::State::Normal, true};
    buttonManager.addButton("Golemiarz", buttonData, *buttonStyle);
    buttonData = {{120.f, 50.f}, {10.f, 160.f}, "White", [this](const AppContext &context) {
        context.gameManager.setLocalPlayer(Game::Player::White);
        this->buttonManager.updateButtonState("White", Button::State::Disabled);
        this->buttonManager.updateButtonState("Black", Button::State::Disabled);
    }, Button::State::Normal, false};
    buttonManager.addButton("White", buttonData, *buttonStyle);

    buttonData = {{120.f, 50.f}, {10.f, 210.f}, "Black", [this](const AppContext &context) {
        context.gameManager.setLocalPlayer(Game::Player::Black);
        this->buttonManager.updateButtonState("White", Button::State::Disabled);
        this->buttonManager.updateButtonState("Black", Button::State::Disabled);
    }, Button::State::Normal, false};
    buttonManager.addButton("Black", buttonData, *buttonStyle);
}








