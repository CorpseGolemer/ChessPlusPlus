#include "../../../Headers/Graphics/UI/ButtonManager.h"
#include <algorithm>
#include <iostream>
#include <ranges>
#include "../../../Headers/Graphics/BoardRenderer.h"
#include "../../../Headers/Graphics/UI/Button.h"

/// FUNKCJA Z SFML (ALE CHCĘ MIEĆ DO NIEJ DOSTĘP)
void ButtonManager::addGlyphQuad(sf::VertexArray& vertices, const sf::Vector2f position, const sf::Color color, const sf::Glyph& glyph, const float italicShear)
{
    constexpr sf::Vector2f padding(1.f, 1.f);

    const sf::Vector2f p1 = glyph.bounds.position - padding;
    const sf::Vector2f p2 = glyph.bounds.position + glyph.bounds.size + padding;

    const auto uv1 = sf::Vector2f(glyph.textureRect.position) - padding;
    const auto uv2 = sf::Vector2f(glyph.textureRect.position + glyph.textureRect.size) + padding;

    vertices.append({position + sf::Vector2f(p1.x - italicShear * p1.y, p1.y), color, {uv1.x, uv1.y}});
    vertices.append({position + sf::Vector2f(p2.x - italicShear * p1.y, p1.y), color, {uv2.x, uv1.y}});
    vertices.append({position + sf::Vector2f(p1.x - italicShear * p2.y, p2.y), color, {uv1.x, uv2.y}});
    vertices.append({position + sf::Vector2f(p1.x - italicShear * p2.y, p2.y), color, {uv1.x, uv2.y}});
    vertices.append({position + sf::Vector2f(p2.x - italicShear * p1.y, p1.y), color, {uv2.x, uv1.y}});
    vertices.append({position + sf::Vector2f(p2.x - italicShear * p2.y, p2.y), color, {uv2.x, uv2.y}});
}

ButtonManager::ButtonManager(AppContext& context) : context(context) {
    addButtonStyle(Button::ButtonStyle::Default);
}

ButtonManager::~ButtonManager() {
    clearAllButtons();
}


void ButtonManager::updateBackgroundsVertices() {
    if (!this->needsVertexUpdate) return;

    backgroundArray.clear();
    backgroundArray.setPrimitiveType(sf::PrimitiveType::Triangles);
    if (buttons.empty()) return;

    for (const auto& button: buttons | std::views::values) {
        const Button::ButtonData& data = button->getButtonData();
        if (!data.visible) continue;

        BoardRenderer::addQuad(backgroundArray, data.position, button->getCurrentColor(), data.size);
    }
    needsVertexUpdate = false;
}

void ButtonManager::updateTextVertices() {
    if (buttons.empty()) return;
    for (const auto& button: buttons | std::views::values) {
        if (!button->getButtonData().visible) continue;
        const Button::ButtonStyle& style = button->getButtonStyle();
        VertexArrayIdentifier id {style.fontName, style.textSize};
        auto it = textMap.find(id);
        if (it == textMap.end()) {
            VertexArrayObject obj {{}, true, false};
            textMap.emplace(id, obj);
            it = textMap.find(id);
        }
        auto& [vertexArray, needsUpdate, updating] = it->second;
        if (!updating && needsUpdate) {
            vertexArray.clear();
            vertexArray.setPrimitiveType(sf::PrimitiveType::Triangles);
            updating = true;
        }
        if (updating) {
            handleButtonText(vertexArray, *button);
        }
    }
    for (auto& obj : textMap | std::views::values) {
        if (obj.updating) {
            obj.needsUpdate = false;
            obj.updating = false;
        }
    }
}

bool ButtonManager::handleButtonEvent(const sf::Event &event, const sf::Vector2f &mouseCoords) {
    if (buttons.empty()) return false;

    for (const auto &button : buttons | std::views::values) {
        auto [consumed, needsUpdate, clicked] = button->handleEvent(event, mouseCoords);
        if (needsUpdate) {
            needsVertexUpdate = true;
        }
        if (clicked) {
            button->getButtonData().action(context);
            return true;
        }
        if (consumed) {
            return true;
        }
    }

    return false;
}

void ButtonManager::clearAllButtons() {
    buttons.clear();
    buttonStyles.clear();
    textMap.clear();
    backgroundArray.clear();
}

void ButtonManager::drawButtons(sf::RenderTarget &window) const {
    window.draw(backgroundArray);
    for (const auto& [id, obj] : textMap) {
        sf::RenderStates states = sf::RenderStates::Default;
        states.texture = &Resources::get().getFont(id.id).getTexture(id.textSize);
        states.coordinateType = sf::CoordinateType::Pixels;
        window.draw(obj.vertexArray, states);
    }

}

void ButtonManager::addButtonStyle(const Button::ButtonStyle &style) {
    if (!std::ranges::any_of(buttonStyles, [&style](const Button::ButtonStyle& s) {
        return s == style;
    })) {
        buttonStyles.push_back(style);
    }
}

void ButtonManager::addButton(const std::string& name, const Button::ButtonData &data, Button::ButtonStyle& style) {
    auto button = std::make_unique<Button>(data, style);
    addButtonStyle(style);
    buttons.emplace(name, std::move(button));
    const VertexArrayIdentifier id {style.fontName, style.textSize};
    auto vaIt = textMap.find(id);
    if (vaIt == textMap.end()) {
        VertexArrayObject obj {{}, true, false};
        textMap.emplace(id, obj);
        vaIt = textMap.find(id);
    }
    vaIt->second.needsUpdate = true;
    needsVertexUpdate = true;
}

void ButtonManager::removeButton(const std::string &name) {
    const auto it = buttons.find(name);
    if (it == buttons.end()) return;
    const auto& button = it->second;
    const auto& style = button->getButtonStyle();
    const VertexArrayIdentifier id {style.fontName, style.textSize};
    buttons.erase(it);

    bool fontStillUsed = false;
    for (const auto &otherButton: buttons | std::views::values) {
        if (const auto& otherStyle = otherButton->getButtonStyle();
            otherStyle.fontName == id.id &&
            otherStyle.textSize == id.textSize) {
            fontStillUsed = true; break;
        }
    }
    if (!fontStillUsed) {
        textMap.erase(id);
    }
    else {
        if (const auto vaIt = textMap.find(id); vaIt != textMap.end()) {
            vaIt->second.needsUpdate = true;
        }
    }
    needsVertexUpdate = true;
}

void ButtonManager::update() {
    updateBackgroundsVertices();
    updateTextVertices();
}

Button::ButtonStyle* ButtonManager::getButtonStyle(const std::string &name) {
    const auto it = std::ranges::find_if(buttonStyles, [&name](const Button::ButtonStyle& s) {
        return  s.id == name;
    });

    if (it == buttonStyles.end()) return nullptr;
    return &(*it);
}

void ButtonManager::handleButtonText(sf::VertexArray& vertices, const Button &button) {
    // Pobierz kluczowe dane
    const Button::ButtonData& data = button.getButtonData();
    const Button::ButtonStyle& style = button.getButtonStyle();
    const sf::Font& f = Resources::get().getFont(style.fontName);
    /// Pusty tekst to wracaj
    if (data.label.isEmpty()) return;

    // OBLICZAMY WARTOŚCI PRZED PĘTLĄ
    float whiteSpaceWidth = f.getGlyph(U' ', style.textSize, false).advance;
    const float letterSpacing = whiteSpaceWidth / 3.f * (style.letterSpacing - 1.f);
    whiteSpaceWidth += letterSpacing;
    const float lineSpacing = f.getLineSpacing(style.textSize) * style.lineSpacing;
    float x = button.calculateTextPosition().x;
    auto y = button.calculateTextPosition().y + static_cast<float>(style.textSize) / 1.5f;

    uint32_t prev = 0;

    // PĘTLA
    for (const uint32_t c : data.label) {
        // Pomiń znak przejścia na początek linii
        if (c == '\r') continue;

        // Dodaj Kerning (gdyby nie kerning, wszystkie litery miałyby ten sam odstęp)
        x += f.getKerning(prev, c, style.textSize, false);
        // Poprzedni znak zmieniamy
        prev = c;

        // Obsługa znaków specjalnych
        if (c == U'\n' || c == U'\t' || c == U' ') {

            switch (c) {
                case U' ': x += whiteSpaceWidth; break;
                case U'\t': x += whiteSpaceWidth * 4; break;
                case U'\n': {y += lineSpacing; x = 0; break;}
                default: ;
            }

            continue;
        }

        const sf::Glyph& g = f.getGlyph(c, style.textSize, false);
        addGlyphQuad(vertices, {x,y}, style.textColor, g, 0.f);

        x += g.advance + letterSpacing;
    }
}

void ButtonManager::updateButtonState(const std::string &id, const Button::State newState) {
    const auto it = buttons.find(id);
    if (it == buttons.end()) {
        std::cerr << "Button " << id << " not found" << std::endl;
        return;
    }
    it->second->setState(newState);
    needsVertexUpdate = true;
}

void ButtonManager::updateButtonVisibility(const std::string &id, const bool visible) {
    const auto it = buttons.find(id);
    if (it == buttons.end()) {
        std::cerr << "Button " << id << " not found" << std::endl;
        return;
    }
    it->second->setVisible(visible);
    needsVertexUpdate = true;
    const VertexArrayIdentifier Id {it->second->getButtonStyle().fontName, it->second->getButtonStyle().textSize};
    const auto vaIt = textMap.find(Id);
    if (vaIt == textMap.end()) {
        std::cerr << "Style not found" << std::endl;
        return;
    }
    vaIt->second.needsUpdate = true;
}










