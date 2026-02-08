#include <cmath>
#include "../../Headers/Graphics/PromotionRenderer.h"

#include <iostream>

#include "../../Headers/Game/GameManager.h"
#include "../../Headers/Graphics/Colors.h"

PromotionRenderer::PromotionRenderer(const float tileSize)
: tileSize(tileSize) {
    const unsigned int texSize = Resources::get().getTextureByIndex(0).getSize().x;
    for (auto& sprite: diamondSprites) {
        constexpr float scaleParameter = 0.55f;
        sprite.setOrigin(sf::Vector2f(static_cast<float>(texSize) /2.f, static_cast<float>(texSize) /2.f));
        sprite.setScale(sf::Vector2f(tileSize / static_cast<float>(texSize), tileSize / static_cast<float>(texSize)) * scaleParameter);
    }
    for (auto& diamond : diamonds) {
        centerDiamond(diamond);
        diamond.setFillColor(Colors::diamondColor);
        diamond.setOutlineColor(Colors::diamondOutlineColor);
        diamond.setOutlineThickness(maxOutline);
        diamond.setRotation(sf::degrees(45.f));
    }
}

int PromotionRenderer::isMouseOverDiamond(const sf::Vector2f mouseCoords) const {
    for (int i = 0; i < 4; i++) {
        const sf::Vector2f local = diamonds[i].getInverseTransform().transformPoint(mouseCoords);
        if (diamonds[i].getLocalBounds().contains(local)) return i;
    }
    return -1;
}

void PromotionRenderer::centerDiamond(const int index) {
    if (index < 0 || index >= diamonds.size()) return;
    diamonds[index].setOrigin({diamonds[index].getSize().x / 2.f, diamonds[index].getSize().y / 2.f});
}

void PromotionRenderer::centerDiamond(sf::RectangleShape &diamond) {
    diamond.setOrigin({diamond.getSize().x / 2.f, diamond.getSize().y / 2.f});
}

PieceType PromotionRenderer::consumeClick(const sf::Vector2f mousePos) const {
    if (const int index = isMouseOverDiamond(mousePos); index != -1) {
        const PieceType choice = GameManager::promotionWindowOrder[index];
        return choice;
    }
    return PieceType::None;
}

void PromotionRenderer::handleHover(const sf::Vector2f position) {
    std::cout << position.x << ", " << position.y << std::endl;
    const int hovered = isMouseOverDiamond(position);
    for (int i = 0; i < diamonds.size(); i++) {
        auto& diamond = diamondAnim[i];

        const bool wasActive = diamond.active;
        diamond.active = i == hovered;

        if (wasActive != diamond.active) {
            diamond.needsAnimation = true;
        }
    }
}

void PromotionRenderer::updatePositions(const sf::Vector2f firstDiamondPosition, const bool whitesPromotes) {
    const float flag = whitesPromotes ? 1.f : -1.f;
    for (int i = 0; i < diamonds.size(); i++) {
        const sf::Vector2f currentPos = firstDiamondPosition + sf::Vector2f(0.f, tileSize * static_cast<float>(i) * flag);
        diamondPositions[i] = currentPos;
        diamonds[i].setPosition(currentPos);
        centerDiamond(i);
        diamondSprites[i * 2 + whitesPromotes].setPosition(currentPos - sf::Vector2f(0.f, i == 0 ? 0.f : 2.f));
    }
    this->whitePromotes = whitesPromotes;
}

void PromotionRenderer::doDiamondAnimation(const float deltaTime) {
    for (int i =0; i < diamonds.size(); i++) {
        constexpr float animationTime = 0.25f;
        constexpr float angleSpeed = 45.f / animationTime;
        constexpr float outlineSpeed = 3.f / animationTime;
        constexpr float sizeSpeed = 33.3f / animationTime;
        constexpr float pieceSpeed = 0.33f / animationTime;

        auto& [active, needsAnimation, currentAngle, currentSize, currentOutline, currentPieceScale] = diamondAnim[i];

        if (!needsAnimation) continue;

        bool stillAnimating = false;

        if (active) {
            currentAngle = std::min(currentAngle + angleSpeed * deltaTime, maxDeg);
            currentSize = std::min(currentSize + sizeSpeed * deltaTime, maxSize);
            currentOutline = std::max(currentOutline - outlineSpeed * deltaTime, minOutline);
            currentPieceScale = std::min(currentPieceScale + pieceSpeed * deltaTime, maxPieceScale);
            if (std::abs(currentAngle - maxDeg) > 0.01f ||
                std::abs(currentSize - maxSize) > 0.01f ||
                std::abs(currentOutline - minOutline) > 0.01f ||
                std::abs(currentPieceScale - maxPieceScale) > 0.01f) stillAnimating = true;
        }
        else {
            currentAngle = std::max(currentAngle - angleSpeed * deltaTime, minDeg);
            currentSize = std::max(currentSize - sizeSpeed * deltaTime, minSize);
            currentOutline = std::min(currentOutline + outlineSpeed * deltaTime, maxOutline);
            currentPieceScale = std::max(currentPieceScale - pieceSpeed * deltaTime, minPieceScale);
            if (std::abs(currentAngle - minDeg) > 0.01f ||
                std::abs(currentSize - minSize) > 0.01f ||
                std::abs(currentOutline - maxOutline) > 0.01f ||
                std::abs(currentPieceScale - minPieceScale) > 0.01f) stillAnimating = true;
        }

        needsAnimation = stillAnimating;
        diamonds[i].setSize({currentSize, currentSize});
        diamonds[i].setRotation(sf::degrees(currentAngle));
        diamonds[i].setOutlineThickness(currentOutline);
        diamonds[i].setFillColor(active ? Colors::diamondHoveredColor : Colors::diamondColor);
        centerDiamond(i);
        diamonds[i].setPosition(diamondPositions[i]);
        diamondSprites[i * 2 + whitePromotes].setScale({currentPieceScale, currentPieceScale});
    }
}

void PromotionRenderer::drawAllDiamond(sf::RenderWindow &window) const {
    for (int i = 0; i < diamonds.size(); i++) {
        window.draw(diamonds[i]);
        window.draw(diamondSprites[i * 2 + whitePromotes]);
    }
}

void PromotionRenderer::activate(const sf::Vector2f &mousePos) {
    if (const int hovered = isMouseOverDiamond(mousePos); hovered != -1) {
        diamondAnim[hovered].active = true;
        diamondAnim[hovered].needsAnimation = true;
    }
}

void PromotionRenderer::deactivate(const sf::Vector2f &mousePos) {
    if (const int hovered = isMouseOverDiamond(mousePos); hovered != -1) {
        diamondAnim[hovered].active = false;
        diamondAnim[hovered].needsAnimation = true;
    }
}

PromotionRenderer::PromotionAnim &PromotionRenderer::getFirstDiamond() {
    return diamondAnim[0];
}



