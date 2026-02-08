#include "../../Headers/Graphics/Resources.h"

#include <iostream>

namespace fs = std::filesystem;

sf::Texture& Resources::getTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it == textures.end()) {
        loadTexture(name);

        it = textures.find(name);
        if (it == textures.end()) {
            throw std::runtime_error("No such texture");
        }
    }
    return it->second;
}

sf::Texture& Resources::getTextureByIndex(const size_t index) const {
    if (index >= texByInt.size()) {
        throw std::out_of_range("Texture index out of range" + std::to_string(index));
    }

    return *texByInt[index];
}

void Resources::loadTexture(const std::string &name) {
    std::string path = texturePath + name;
    if (fs::path(name).extension().empty()) {
        path += ".png";
    }

    sf::Texture tex;
    if (!tex.loadFromFile(path)) {
        throw std::runtime_error("Failed to load texture: " + path);
    }

    tex.setSmooth(true);

    auto [it, inserted] = textures.emplace(name, std::move(tex));

    texByInt.push_back(&it->second);

    std::cout << "RESOURCES: Loaded texture: " << name
    << " (index: " << (texByInt.size() - 1) << ")" <<  std::endl;
}

sf::Font& Resources::getFont(const std::string& name) {
    auto it = fonts.find(name);
    if (it == fonts.end()) {
        std::string path = fontPath + name;

        if (fs::path(name).extension().empty()) {
            path += ".ttf";
        }

        sf::Font font;
        if (!font.openFromFile(path)) {
            throw std::runtime_error("Failed to load font: " + path);
        }

        auto [newIt, inserted] = fonts.emplace(name, std::move(font));
        it = newIt;

        std::cout << "RESOURCES: Loaded font: " << name << std::endl;
    }
    return it->second;
}

void Resources::clear() {
    textures.clear();
    texByInt.clear();
    fonts.clear();
    std::cout << "RESOURCES: Cleared all resources" << std::endl;
}

void Resources::loadChessResources() {
    for (int i = 0; i < 12; i++) {
        constexpr char tab2[2] = {'b', 'w'};
        constexpr char tab[6] = {'R', 'N', 'B', 'Q', 'K', 'P'};
        std::string name = std::string() + tab2[i%2] + tab[i/2];
        loadTexture(name);
    }
}

