#include "../../../Headers/Graphics/UI/GameStateUIRenderer.h"

GameStateUIRenderer::GameStateUIRenderer() {
    // Kolor lewego paska
    leftSideBar.setFillColor({0,0,0,100});
}

void GameStateUIRenderer::drawGameUI(sf::RenderWindow &window) const {
    window.draw(leftSideBar);
}


