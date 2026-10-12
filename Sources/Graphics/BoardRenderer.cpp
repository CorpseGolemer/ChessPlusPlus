#include <algorithm>
#include "../../Headers/Graphics/BoardRenderer.h"
#include <ranges>
#include "../../Headers/Graphics/Colors.h"
#include "../../Headers/Game/GameManager.h"
#include "../../Headers/Graphics/UI/ButtonManager.h"
#include <numbers>

void BoardRenderer::RenderVertices::RenderStates::initializeStates() {
    tileDescStates.texture = &Resources::get().getFont(DESCRIPTION_FONT).getTexture(24);
    tileDescStates.coordinateType = sf::CoordinateType::Pixels;

    for (int i=0; i <12; i++) {
        sf::RenderStates states = sf::RenderStates::Default;
        states.texture = &Resources::get().getTextureByIndex(i);
        states.coordinateType = sf::CoordinateType::Pixels;
        piecesStates[i] = states;
    }
}


// ===== KONSTRUKTOR =====

/// Konstruktor klasy
BoardRenderer::BoardRenderer(const GameManager & gameManager)
: gameManager(gameManager), promotionRenderer(TILE_SIZE)
{
    // Ustawianie bazowych kolorów pól
    for (int i = 0 ; i < 8; i++) {
        for (int j = 0 ; j < 8; j++) {
            tileColors[i][j] = Colors::getBaseTileColor(i, j);
            swappedTileColors[i][j] = Colors::getBaseTileColor(i, j, true);
        }
    }

    vertices.states.initializeStates();
    updateBoard();
    draggingSprite.setScale({0.88f, 0.88});
    draggingSprite.setOrigin({draggingSprite.getLocalBounds().size.x / 2.f, draggingSprite.getLocalBounds().size.y / 2.f});

    // Ustawianie parametrów tileHover
    tileHover.setFillColor(sf::Color::Transparent);
    tileHover.setOutlineThickness(TILE_SIZE * 0.05f);
    tileHover.setOrigin(sf::Vector2f(TILE_SIZE * 0.9f / 2.f, TILE_SIZE * 0.9f / 2.f));

    windowDim.setPosition({BOARD_ORIGIN.x,BOARD_ORIGIN.y - 8.f * TILE_SIZE});
    windowDim.setFillColor(Colors::windowDim);
}

void BoardRenderer::onGameEvent(const GameEvent event) {
    switch (event) {
        case GameEvent::SelectionChanged:
            notifyChange(GameEvent::SelectionChanged);
            break;
        case GameEvent::BoardChanged:
            notifyChange(GameEvent::BoardChanged);
            break;
        case GameEvent::GameOver: {
            notifyChange(GameEvent::GameOver);
            break;
        }
        default: break;
    }
}

// ===== GETTERY =====

/// Implementacja getPromotionRenderer
PromotionRenderer &BoardRenderer::getPromotionRenderer() {
    return promotionRenderer;
}

// ===== FUNKCJE ZAJMUJĄCE SIĘ OBSŁUGĄ RUCHU MYSZY =====

/// Implementacja onMouseMoce
void BoardRenderer::onMouseMoved(sf::Vector2f mousePos) {
    notifyChange(GameEvent::HoverChanged);
    if (gameManager.getPromotionFile().has_value()) promotionRenderer.handleHover(mousePos);

    mousePos.x = std::clamp(mousePos.x, BOARD_ORIGIN.x, BOARD_ORIGIN.x + 8 * TILE_SIZE - 1);
    mousePos.y = std::clamp(mousePos.y, BOARD_ORIGIN.y - 8 * TILE_SIZE + 1, BOARD_ORIGIN.y - 1);
    const std::pair<int, int> hoverPos = convertPixelToTile(static_cast<int>(mousePos.x), static_cast<int>(mousePos.y));
    cache.hover.hoverTile = hoverPos;
    cache.hover.hoverPosition = {mousePos.x, mousePos.y};
}

// ===== FUNKCJE KONWERSJI =====

/// Implementacja metody convertPixelToTile
std::pair<int,int> BoardRenderer::convertPixelToTile(const int x, const int y) {
    const float clickedCol = (static_cast<float>(x) - BOARD_ORIGIN.x) / TILE_SIZE;

    const float clickedRow = -((static_cast<float>(y) - BOARD_ORIGIN.y) / TILE_SIZE);

    if (clickedCol < 0 || clickedCol >= 8 || clickedRow < 0 || clickedRow >= 8) {
        return std::make_pair(-1, -1);
    }
    return {static_cast<int>(clickedRow), static_cast<int>(clickedCol)};
}

/// Implementacja metody convertTileToPixel
sf::Vector2f BoardRenderer::convertTileToPixel(const std::pair<int, int> & tile) {
    const float tilePosX = BOARD_ORIGIN.x + TILE_SIZE * static_cast<float>(tile.second);
    const float tilePosY = BOARD_ORIGIN.y - TILE_SIZE * static_cast<float>(tile.first);

    return {tilePosX, tilePosY};
}

/// Implementacja metody tileCenter
sf::Vector2f BoardRenderer::tileCenter(const sf::Vector2f & tilePos) {
    return tilePos + OFFSET;
}

// ===== METODY ZARZĄDZANIA CACHE =====

/// Implementacja metody update
void BoardRenderer::update() {
    if (!cache.event.has_value()) return;
    switch (cache.event.value()) {
        case GameEvent::HoverChanged: {
            handleHoverChange();
            break;
        }
        case GameEvent::SelectionChanged: {
            handleSelectionChange();
            break;
        }
        case GameEvent::BoardChanged: {
            handleBoardChanged();
            break;
        }
        case GameEvent::GameOver: {
            handleBoardChanged();
            isGameOver = true;
            break;
        }
        default: return;
    }
    cache.event.reset();
}

/// Implementacja handleHoverChange
void BoardRenderer::handleHoverChange() {
    if (gameManager.getDraggingState().active) {
        draggingSprite.setPosition(cache.hover.hoverPosition);
        auto& [fst, snd] = cache.hover.hoverTile;
        // JEŚLI MYSZ JEST NA TYM SAMYM KAFELKU WRÓĆ
        if (cache.lastHoveredTile == std::pair{fst, snd}) {
            return;
        }
        cache.lastHoveredTile = std::pair{fst, snd};
        tileHover.setPosition(tileCenter(TILE_POSITIONS[fst][snd]));

        const sf::Color & color = swappedTileColors[fst][snd];
        tileHover.setOutlineColor(sf::Color(color.r, color.g, color.b, 180));
        cache.hover.drawHover = true;
        return;
    }
    if (!cache.possiblePieceMoves.empty()) {
        auto& [fst, snd] = cache.hover.hoverTile;
        const bool flag2 = std::ranges::any_of(cache.possiblePieceMoves, [&](const auto& move) {
            return move.pos.first == fst && move.pos.second == snd;
        });
        if (flag2) {
            if (cache.lastHoveredTile == std::pair{fst, snd}) {
                return;
            }
            cache.lastHoveredTile = std::pair{fst, snd};
            tileHover.setPosition(tileCenter(TILE_POSITIONS[fst][snd]));
            const sf::Color & color = swappedTileColors[fst][snd];
            tileHover.setOutlineColor(sf::Color(color.r, color.g, color.b, 180));
            cache.hover.drawHover = true;
            return;
        }
    }
    cache.hover.drawHover = false;
    cache.lastHoveredTile = {-1,-1};
}

/// Implementacja handleSelectionChange
void BoardRenderer::handleSelectionChange() {
    vertices.tileEffects = sf::VertexArray(sf::PrimitiveType::Triangles);
    vertices.hints = sf::VertexArray(sf::PrimitiveType::Triangles);
    cache.possiblePieceMoves = gameManager.selectedPieceMoves();
    handleHoverChange();
    updateDraggedPiece();
    updateTileEffects();
    updateHints();
}

/// Implementacja handleBoardChanged
void BoardRenderer::handleBoardChanged() {
    handleSelectionChange();
    cache.piecesOnBoard = gameManager.piecesOnBoard();

    updatePromotionWindow();
    // Zbierz na nowo informację o bierkach
    updatePieces();
}

/// Implementacja notifyChange
void BoardRenderer::notifyChange(GameEvent event) {
    if (!cache.event.has_value()) {
        cache.event = event;
        return;
    }
    if (static_cast<int>(cache.event.value()) < static_cast<int>(event)) {
        cache.event = event;
    }
}

// ===== FUNKCJE RYSOWNICZE =====

/// Implementacja głównej metody rysującej draw
void BoardRenderer::draw(sf::RenderWindow &window) {
    window.draw(vertices.tiles);
    window.draw(vertices.tileDescriptions, vertices.states.tileDescStates);
    window.draw(vertices.tileEffects);
    for (auto &[fst, snd]: vertices.piecesMap) {
        window.draw(snd, vertices.states.piecesStates[fst]);
    }
    window.draw(vertices.hints);
    if (cache.hover.drawHover) window.draw(tileHover);
    if (cache.lastDraggingActive) window.draw(draggingSprite);
    if (cache.promoting) {
        window.draw(windowDim);
        promotionRenderer.drawAllDiamond(window);
    }
    if (isGameOver) {window.draw(windowDim);}
}

/// Implementacja drawBoard
void BoardRenderer::updateBoard() {
    if (this->vertices.tiles.getVertexCount() > 0) return;
    sf::VertexArray vertices(sf::PrimitiveType::Triangles);
    sf::VertexArray vertices2(sf::PrimitiveType::Triangles);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const auto& position = TILE_POSITIONS[i][j];
            addQuad(vertices, position, tileColors[i][j]);
            if (i == 0 ) {
                constexpr sf::Vector2f rowOffset = {78.f, -8.f};
                const sf::Glyph& glyph = Resources::get().getFont(DESCRIPTION_FONT).getGlyph('a' + j, 24, false);
                ButtonManager::addGlyphQuad(vertices2, position + rowOffset, swappedTileColors[i][j], glyph, 0.f);
            }
            if (j == 0) {
                constexpr sf::Vector2f colOffset = {8.f, -72.f};
                const sf::Glyph& glyph = Resources::get().getFont(DESCRIPTION_FONT).getGlyph('1' + i, 24, false);
                ButtonManager::addGlyphQuad(vertices2, position + colOffset, swappedTileColors[i][j], glyph, 0.f);
            }
        }
    }
    this->vertices.tiles = vertices;
    this->vertices.tileDescriptions = vertices2;
}


/// Implementacja drawTileEffects
void BoardRenderer::updateTileEffects() {
    if (this->vertices.tileEffects.getVertexCount() > 0) return;
    sf::VertexArray vertices(sf::PrimitiveType::Triangles);

    if (gameManager.getLastMove().has_value()) {
        bool flag = false, flag2 = false, flag3 = false, flag4 = false;
        const Move & move = gameManager.getLastMove().value();
        flag = std::ranges::any_of(gameManager.getHighlightedTiles(), [&](const auto& tile) {
           return tile == move.getPosition();
        });
        flag2 = std::ranges::any_of(gameManager.getHighlightedTiles(), [&](const auto& tile) {
           return tile == move.getNextPosition();
        });
        flag3 = move.getPosition() == gameManager.getSelectedTile();
        flag4 = move.getNextPosition() == gameManager.getSelectedTile();
        const auto & pos = TILE_POSITIONS[move.getPosition().first][move.getPosition().second];
        const auto & nPos = TILE_POSITIONS[move.getNextPosition().first][move.getNextPosition().second];
        if (!(flag || flag3)) addQuad(vertices, pos, Colors::lastMove);
        if (!(flag2 || flag4)) addQuad(vertices, nPos, Colors::lastMove);
    }

    if (gameManager.getSelectedTile().first != -1) {
        const auto& pos = TILE_POSITIONS[gameManager.getSelectedTile().first][gameManager.getSelectedTile().second];
        addQuad(vertices, pos, Colors::selected);
    }

    for (auto &[fst, snd] : gameManager.getHighlightedTiles()) {
        const auto& pos = TILE_POSITIONS[fst][snd];
        addQuad(vertices, pos, Colors::highlighted);
    }

    this->vertices.tileEffects = vertices;
}

/// Implementacja updatePieces
void BoardRenderer::updatePieces(){
    const std::bitset<12> verticesToUpdate = gameManager.needsUpdate();

    for (int i = 0; i < 12; i++) {
        if (verticesToUpdate.test(i)) {
            auto it = vertices.piecesMap.find(i);
            if (it == vertices.piecesMap.end()) {
                vertices.piecesMap.emplace(i, sf::VertexArray(sf::PrimitiveType::Triangles));
            }
            else {
                it->second = sf::VertexArray(sf::PrimitiveType::Triangles);
            }
        }
    }

    for (const auto&[pos, coloredSymbol] : cache.piecesOnBoard) {
        if (verticesToUpdate.test(coloredSymbol)) {
            addPiece(vertices.piecesMap[coloredSymbol],
                TILE_POSITIONS[pos.first][pos.second],
                Resources::get().getTextureByIndex(coloredSymbol));
        }
    }

    for (int i = 0; i < 12; i++) {
        if (verticesToUpdate.test(i)) {
            if (vertices.piecesMap[i].getVertexCount() == 0) {
                vertices.piecesMap.erase(i);
            }
        }
    }
}

/// Implementacja drawHints
void BoardRenderer::updateHints() {
    if (cache.possiblePieceMoves.empty()) return;

    sf::VertexArray vertices(sf::PrimitiveType::Triangles);
    for (const auto & [fst, snd]: cache.possiblePieceMoves) {
        if (!fst) {
            addCircle(vertices, tileCenter(TILE_POSITIONS[snd.first][snd.second]), Colors::hint, radius,
                0.f, sf::Color::Transparent, 40);
        }
        else {
            addCircle(vertices, tileCenter(TILE_POSITIONS[snd.first][snd.second]), sf::Color::Transparent, superRadius,
                TILE_SIZE * 0.1f, Colors::hint, 40);
        }
    }
    this->vertices.hints = vertices;
}

/// Implementacja drawDraggedPiece
void BoardRenderer::updateDraggedPiece() {
    if (const auto& drg_s = gameManager.getDraggingState(); drg_s.active != cache.lastDraggingActive) {
        cache.lastDraggingActive = drg_s.active;
        draggingSprite.setTexture(Resources::get().getTextureByIndex(drg_s.symbol));
        const auto it = vertices.piecesMap.find(drg_s.symbol);
        it->second = sf::VertexArray(sf::PrimitiveType::Triangles);
        for (auto&[pos, coloredSymbol] : cache.piecesOnBoard) {
            if (coloredSymbol == drg_s.symbol) {
                const bool flag = pos == drg_s.pos && cache.lastDraggingActive;
                addPiece(vertices.piecesMap[drg_s.symbol],
                    TILE_POSITIONS[pos.first][pos.second],
                    Resources::get().getTextureByIndex(coloredSymbol), flag);
            }
        }
    }
}

/// Implementacja drawPromotionWindow
void BoardRenderer::updatePromotionWindow() {
    const std::optional<std::pair<uint8_t, uint8_t>> promotionTile = gameManager.getPromotionFile();
    if (!promotionTile.has_value()) {
        cache.promoting = false;
        promotionRenderer.deactivate(cache.hover.hoverPosition);
        return;
    }

    auto [fst, snd] = promotionTile.value();
    promotionRenderer.updatePositions(tileCenter(TILE_POSITIONS[fst][snd]), fst == 7);
    promotionRenderer.activate(cache.hover.hoverPosition);
    cache.promoting = true;
}

/// Implementacja drawAnimations
void BoardRenderer::updateAnimations(const float deltaTime) {
    promotionRenderer.doDiamondAnimation(deltaTime);
}

/// Implementacja addQuad
void BoardRenderer::addQuad(sf::VertexArray &vertices, const sf::Vector2f &position, const sf::Color &color, const sf::Vector2f& size) {
    vertices.append(sf::Vertex(position, color));
    vertices.append(sf::Vertex(sf::Vector2f(position.x + size.x, position.y), color));
    vertices.append(sf::Vertex(sf::Vector2f(position.x + size.x, position.y - size.y), color));

    vertices.append(sf::Vertex(position, color));
    vertices.append(sf::Vertex(sf::Vector2f(position.x, position.y - size.y), color));
    vertices.append(sf::Vertex(sf::Vector2f(position.x + size.x, position.y - size.y), color));
}

/// Implementacja drawPiece
void BoardRenderer::addPiece(sf::VertexArray& vertices, const sf::Vector2f &pos, const sf::Texture& tex, const bool isDraggedPiece,
    const sf::Vector2f& size, const sf::Vector2f& tileSize) {

    const sf::Vector2f padding = (tileSize - size) / 2.f;
    const sf::Vector2f p1 = pos + sf::Vector2f(padding.x, -padding.y);
    const sf::Vector2f p2 = pos + sf::Vector2f(tileSize.x, -tileSize.y) + sf::Vector2f(-padding.x, padding.y);

    constexpr auto uv1 = sf::Vector2f(0.f, 0.f);
    const auto uv2 = sf::Vector2f(static_cast<float>(tex.getSize().x), static_cast<float>(tex.getSize().y));

    const sf::Color pieceColor = isDraggedPiece ? sf::Color(255,255,255,120) : sf::Color::White;

    vertices.append({p1, pieceColor, {uv1.x, uv2.y}});
    vertices.append({{p2.x, p1.y}, pieceColor, uv2});
    vertices.append({p2, pieceColor, {uv2.x, uv1.y}});

    vertices.append({p1, pieceColor, {uv1.x, uv2.y}});
    vertices.append({{p1.x, p2.y}, pieceColor, uv1});
    vertices.append({p2, pieceColor, {uv2.x, uv1.y}});

}

/// Implementacja addCircle
void BoardRenderer::addCircle(sf::VertexArray &vertices, const sf::Vector2f &pos, const sf::Color color, const float radius,
    const float outlineThickness, const sf::Color outlineColor, const int pointCount) {
    if (pointCount < 3) return;
    for (int i = 0; i < pointCount; i++) {
        const float angle = static_cast<float>(i) / static_cast<float>(pointCount) * 2.f * std::numbers::pi_v<float>;
        const float nextAngle = (static_cast<float>(i) + 1) / static_cast<float>(pointCount) * 2.f * std::numbers::pi_v<float>;
        const float sin1 = std::sin(angle);
        const float cos1 = std::cos(angle);
        const float sin2 = std::sin(nextAngle);
        const float cos2 = std::cos(nextAngle);

        const sf::Vector2f point = {
            pos.x + cos1 * radius,
            pos.y + sin1 * radius
        };

        const sf::Vector2f nextPoint = {
            pos.x + cos2 * radius,
            pos.y + sin2 * radius
        };

        vertices.append({pos, color});
        vertices.append({point, color});
        vertices.append({nextPoint, color});

        if (outlineThickness > 0.f) {
            const sf::Vector2f outlinePoint = {
                pos.x + cos1 * (radius + outlineThickness),
                pos.y + sin1 * (radius + outlineThickness),
            };
            const sf::Vector2f outlineNext = {
                pos.x + cos2 * (radius + outlineThickness),
                pos.y + sin2 * (radius + outlineThickness),
            };

            vertices.append({point, outlineColor});
            vertices.append({nextPoint, outlineColor});
            vertices.append({outlinePoint, outlineColor});

            vertices.append({outlinePoint, outlineColor});
            vertices.append({outlineNext, outlineColor});
            vertices.append({nextPoint, outlineColor});
        }
    }
}






