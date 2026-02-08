#include "../../../Headers/Graphics/UI/Button.h"
#include "../../../Headers/AppContext.h"
#include "../../../Headers/Graphics/Colors.h"

const Button::ButtonStyle Button::ButtonStyle::Default = []() {
    ButtonStyle style;
    style.id = "Default";
    style.textSize = 18;
    style.textColor = sf::Color(220,220,220);
    style.normalColor = sf::Color::Transparent;
    style.hoveredColor = sf::Color(255,255,255,25);
    style.pressedColor = sf::Color(255,255,255,50);
    style.disabledColor = sf::Color(100,100,100,50);
    style.lineSpacing = 1.05f;
    style.lineSpacing = 1.1f;

    return style;
}();

sf::Text Button::measurementText = sf::Text(Resources::get().getFont("ARIALBD"));

Button::Button(const ButtonData& buttonData, ButtonStyle& buttonStyle) : buttonStyle(&buttonStyle) {
    this->buttonData = buttonData;
    measurementText.setString("Button");
    measurementText.setOrigin({measurementText.getLocalBounds().size.x / 2.f, measurementText.getLocalBounds().size.y / 2.f});
}

bool Button::contains(const sf::Vector2f &mouseCoords) const {
    return mouseCoords.x <= buttonData.position.x + buttonData.size.x &&
            mouseCoords.x >= buttonData.position.x &&
            mouseCoords.y >= buttonData.position.y - buttonData.size.y &&
            mouseCoords.y <= buttonData.position.y;
}

void Button::setPosition(const sf::Vector2f &position) {
    buttonData.position = position;
}

void Button::setText(const sf::String &text) {
    buttonData.label = text;
}

void Button::setTextColor(const sf::Color &color) const {
    buttonStyle->textColor = color;
}

bool Button::setState(const State newState) {
    if (newState == buttonData.state) return false;
    buttonData.state = newState;
    return true;
}

void Button::setCallback(const std::function<void(AppContext&)> &callback) {
    buttonData.action = callback;
}

void Button::setSize(const sf::Vector2f &size) {
    buttonData.size = size;
}

void Button::setVisible(const bool visible) {
    buttonData.visible = visible;
}

void Button::setColors(const sf::Color& normal, const sf::Color& hovered,
    const sf::Color& pressed, const sf::Color& disabled) const {
    buttonStyle->normalColor = normal;
    buttonStyle->hoveredColor = hovered;
    buttonStyle->pressedColor = pressed;
    buttonStyle->disabledColor = disabled;
}

void Button::setTextSize(const unsigned char size) const {
    buttonStyle->textSize = size;
}

sf::Color Button::getCurrentColor() const {
    switch (buttonData.state) {
        default: return sf::Color::Transparent;
        case State::Normal: return buttonStyle->normalColor;
        case State::Hovered: return buttonStyle->hoveredColor;
        case State::Pressed: return buttonStyle->pressedColor;
        case State::Disabled: return buttonStyle->disabledColor;
    }
}

const Button::ButtonData& Button::getButtonData() const {
    return buttonData;
}

const Button::ButtonStyle& Button::getButtonStyle() const {
    return *buttonStyle;
}

Button::HandleFlags Button::handleEvent(const sf::Event &event, const sf::Vector2f& mouseCords) {
    if (buttonData.state == State::Disabled || !buttonData.visible) return {false, false};
    bool consumed = false;
    bool changed = false;
    bool clicked = false;
    const bool inside = contains(mouseCords);

    if (event.is<sf::Event::MouseMoved>()) {
        if (!inside && buttonData.state != State::Pressed) {
            changed = setState(State::Normal);
        }
        if (inside && buttonData.state != State::Pressed) {
            changed = setState(State::Hovered);
        }
    }
    else if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (pressed->button == sf::Mouse::Button::Left && inside) {
            changed = setState(State::Pressed);
            consumed = true;
        }
    }
    else if (const auto* released = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (released->button == sf::Mouse::Button::Left) {
            if (buttonData.state == State::Pressed && inside) {
                consumed = true;
                if (buttonData.action) clicked = true;
            }
            changed = setState(inside ? State::Hovered : State::Normal);
        }
    }
    else if (event.is<sf::Event::MouseLeft>()) {
        if (buttonData.state != State::Pressed) {
            changed = setState(State::Normal);
        }
    }

    return {consumed, changed, clicked};
}

sf::Vector2f Button::calculateTextPosition() const {
    const auto textPos = sf::Vector2f(buttonData.position.x + buttonData.size.x / 2.f, buttonData.position.y - buttonData.size.y / 2.f);
    measurementText.setFont(Resources::get().getFont(buttonStyle->fontName));
    measurementText.setString(buttonData.label);
    measurementText.setCharacterSize(buttonStyle->textSize);
    const sf::FloatRect bounds = measurementText.getLocalBounds();
    measurementText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    measurementText.setPosition(textPos);
    return measurementText.getGlobalBounds().position;
}







