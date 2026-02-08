#pragma once

#include <SFML/Graphics/Texture.hpp>

#include "SFML/Graphics/Font.hpp"

/// Singleton trzymający wszystkie tekstury, fonty i dźwięki aplikacji
class Resources {
public:
    static constexpr auto texturePath = "Assets/Textures/";
    static constexpr auto fontPath = "Assets/Fonts/";

private:
    /// Mapa string na tex
    std::unordered_map<std::string, sf::Texture> textures{};
    /// Mapa string na font
    std::unordered_map<std::string, sf::Font> fonts{};
    /// Tekstury numerycznie
    std::vector<sf::Texture*> texByInt {};

    /// Konstruktor
    Resources() = default;
    /// Destruktor
    ~Resources() = default;
public:
    /// Konstruktor kopiujący (bardziej usuwający XD)
    Resources(const Resources&) = delete;
    /// Operator przypisania
    Resources& operator=(const Resources&) = delete;

    /// Punkt dostępu
    static Resources& get() {
        static Resources instance;
        return instance;
    }

    // ===== TEKSTURY =====

    /// Pobierz teksturę (po nazwie)
    sf::Texture& getTexture(const std::string& name);

    /// Pobierz teksturę (po indeksie)
    sf::Texture& getTextureByIndex(size_t index) const;

private:
    /// Załaduj teksturę
    void loadTexture(const std::string& name);

public:

    // ===== FONTY =====

    /// Pobierz fonta
    sf::Font& getFont(const std::string& name);

    // ===== UTILITIES =====

    /// Zwolnij zasoby
    void clear();

    /// Załaduj zasoby szachowe
    void loadChessResources();
};

// /// Implementacja updateHash
// void Board::updateHash(const Move & move, const Move::UndoState & undoState) {
//     const auto [fromRow, fromCol] = move.getPosition();
//     const auto [toRow, toCol] = move.getNextPosition();
//     const int fromSquare = 8 * fromRow + fromCol;
//     const int toSquare = 8 * toRow + toCol;
//
//     // Edytuj rozmieszczenie figur
//     const Piece * piece = board[fromRow][fromCol];
//     const bool movingColor = piece->getColor();
//     const unsigned char index = piece->getSymbol(true);
//     zobristHash ^= Zobrist::piece[index][fromSquare];
//
//     if (move.hasMoveFlag(Capture)) {
//         const Piece * cPiece = board[toRow][toCol];
//         const unsigned char cIndex = cPiece->getSymbol(true);
//         zobristHash ^= Zobrist::piece[cIndex][toSquare];
//     }
//     else if (move.hasMoveFlag(EnPassant)) {
//         const int enPassantPosition = 8 * fromRow + toCol;
//         const Piece * cPiece = board[fromRow][toCol];
//         const unsigned char cIndex = cPiece->getSymbol(true);
//         zobristHash ^= Zobrist::piece[cIndex][enPassantPosition];
//     }
//
//     if (move.hasMoveFlag(Promotion) && move.promotedPiece < 6) {
//         zobristHash ^= Zobrist::piece[move.promotedPiece * 2 + movingColor][toSquare];
//     }
//     else {
//         zobristHash ^= Zobrist::piece[index][toSquare];
//     }
//
//     // Zmień gracza
//     zobristHash ^= Zobrist::sideToMove;
//
//     // Zmiana enPassantTarget
//     if (undoState.prevEnPassant.second != -1) zobristHash ^= Zobrist::enPassant[undoState.prevEnPassant.second];
//     if (enPassantTarget.has_value()) {
//          zobristHash ^= Zobrist::enPassant[enPassantTarget.value().second];
//     }
//
//     // Na koniec roszady
//     int oldCastling = 0, newCastling = 0;
//     for (int i = 0; i < 4; i++) {
//         oldCastling += undoState.castlingRights[i] * (1<<i);
//         newCastling += castlingRights[i] * (1<<i);
//     }
//     zobristHash ^= Zobrist::castlingRights[oldCastling];
//     zobristHash ^= Zobrist::castlingRights[newCastling];
// }
//
// /// Implementacja reverseHash
// void Board::reverseHash(const Move &move, const Move::UndoState &undoState) {
//     const auto [fromRow, fromCol] = move.getPosition();
//     const auto [toRow, toCol] = move.getNextPosition();
//     const int fromSquare = 8 * fromRow + fromCol;
//     const int toSquare = 8 * toRow + toCol;
//
//     // Usuń figurę z pozycji next i przywróć na poprzednią
//     const Piece * piece = board[toRow][toCol];
//     const bool movingColor = piece->getColor();
//     const unsigned char index = piece->getSymbol(true);
//     zobristHash ^= Zobrist::piece[index][toSquare];
//
//     if (move.hasMoveFlag(Capture)) {
//         const unsigned char cIndex = undoState.capturedPiece->getSymbol(true);
//         zobristHash ^= Zobrist::piece[cIndex][toSquare];
//     }
//     else if (move.hasMoveFlag(EnPassant)) {
//         const int enPassantPosition = 8 * fromRow + toCol;
//         const unsigned char cIndex = undoState.capturedPiece->getSymbol(true);
//         zobristHash ^= Zobrist::piece[cIndex][enPassantPosition];
//     }
//
//     if (move.hasMoveFlag(Promotion) && move.promotedPiece < 6) {
//         zobristHash ^= Zobrist::piece[10 + movingColor][fromSquare];
//     }
//     else {
//         zobristHash ^= Zobrist::piece[index][fromSquare];
//     }
//
//     zobristHash ^= Zobrist::sideToMove;
//
//     // Zmiana enPassantTarget
//     if (enPassantTarget.has_value()) {
//         zobristHash ^= Zobrist::enPassant[enPassantTarget.value().second];
//     }
//     if (undoState.prevEnPassant.second != -1) zobristHash ^= Zobrist::enPassant[undoState.prevEnPassant.second];
//
//     // Roszady
//     int oldCastling = 0, newCastling = 0;
//     for (int i = 0; i < 4; i++) {
//         oldCastling += undoState.castlingRights[i] * (1<<i);
//         newCastling += castlingRights[i] * (1<<i);
//     }
//     zobristHash ^= Zobrist::castlingRights[newCastling];
//     zobristHash ^= Zobrist::castlingRights[oldCastling];
// }
