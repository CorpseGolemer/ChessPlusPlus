#ifndef MOVE_H
#define MOVE_H

#include <array>

#include "Zobrist.h"
#include "../Pieces/Piece.h"

class Piece;

/// Enumerator, który zamienia wszystkie bool'e na jedną zmienną w postaci liczby binarnej
enum MoveFlags{
    None = 0, // 00000000
    Capture = 1 << 0, // 00000001
    CastleShort = 1 << 1, // 00000010
    CastleLong = 1 << 2, // 00000100
    Check = 1 << 3, // 00001000
    Mate = 1 << 4, // 00010000
    Promotion = 1 << 5, // 00100000
    EnPassant = 1 << 6, // 01000000
    Draw = 1 << 7 // 10000000
};

using int8 = int8_t;

/// Klasa odpowiadająca za strukturę ruchu
class Move {

    // ===== POLA KLASY =====

    static unsigned char symbolMap[7];
    /// Obecna pozycja bierki (potrzebna tylko do porównywania ruchów)
    std::pair<int8, int8> position;
    /// Docelowa pozycja bierki
    std::pair<int8, int8> nextPosition;
    /// Flagi ruchu
    MoveFlags flags;
    /// Symbol ruszanej bierki
    PieceType symbol;

public:
    /// Symbol figury, którą stanie się pion po promocji
    PieceType promotedPiece;

    /// Konstruktor klasy Move, który ustawia odpowiednie pola, a gdy nie podano, wstawia wartości bazowe (tylko zabezpieczenie)
    explicit Move(const std::pair<int, int> & position = std::make_pair(-1,-1), const std::pair<int, int> & nextPosition = std::make_pair(-1,-1),
        const MoveFlags flags = None,
        const PieceType symbol = PieceType::None,
        const PieceType promotedPiece = PieceType::None)
    :
    position(position), nextPosition(nextPosition),
    flags(flags),
    symbol(symbol),
    promotedPiece(promotedPiece)
    {}

    // ===== PUBLICZNE METODY =====

    /// Oddaje obecną pozycję bierki
    [[nodiscard]]
    const std::pair<int8, int8> & getPosition() const;
    /// Oddaje docelową pozycję figury
    [[nodiscard]]
    const std::pair<int8,int8> & getNextPosition() const;
    /// Sprawdź, czy ruch ma podaną flagę
    [[nodiscard]] bool hasMoveFlag(MoveFlags flag) const;
    /// Podaje symbol figury
    [[nodiscard]] PieceType getPieceSymbol() const;
    /// Funckja, która mówi objektom klasy, jak mają reagować na podany operator (w tym przypadku == )
    bool operator==(const Move & move) const;
    /// Funckja pomocnicza wypisująca ruch w konsoli
    void debugPrint() const;
    /// Funkcja dodaje flagę do ruchu
    void addFlag(MoveFlags flag);

    // ===== PUBLICZNE STRUCTY =====

    /// Trzyma obraz przywracania stanu sprzed ruchu
    struct UndoState {
        Piece* capturedPiece = nullptr;
        Piece* promotedPiece = nullptr;

        std::array<bool, 4> castlingRights{true, true, true, true};
        std::array<std::pair<int8, int8>, 2> kingPositions{std::make_pair(7, 4), std::make_pair(0, 4)};
        std::pair<int8, int8> prevEnPassant = std::make_pair(-1, -1);
        bool wasCheck = false;
        bool pawnNotMoved = false;

        void debugPrint() const;
    };
};
#endif //MOVE_H
