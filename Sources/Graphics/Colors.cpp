#include "../../Headers/Graphics/Colors.h"

sf::Color Colors::getBaseTileColor(const int i, const int j, const bool swap) {
    if (swap) {
        return (i+j)%2 != 0 ? darkSquare : lightSquare;
    }
    return (i+j)%2 == 0 ? darkSquare : lightSquare;
}
