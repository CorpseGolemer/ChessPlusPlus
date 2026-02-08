#pragma once

#include <unordered_map>

#include "Move.h"

/// Klasa odpowiada za wszystko, co związane z historią partii
class GameHistory {

public:

    // ===== PUBLICZNE STRUCTY =====

    /// Struct, który trzyma informacje
    struct HistoryEntry {
        Move move;
        Move::UndoState undoState;
        int prevHalfMoveCount = 0;
    };

private:
    // ===== PRYWATNE POLA KLASY =====

    /// Game to przyjaciel
    friend class Game;
    /// Wektor przechowujący wszystkie wymagane informacje do odtworzenia partii (w przód i tył)
    std::vector<HistoryEntry> gameHistory;
    /// Mapa do zliczania wystąpień danej pozycji
    std::unordered_map<u64, int> repetitionCount;

public:

    // ===== PUBLICZNE METODY =====

    /// Dodaje nowy HistoryEntry na koniec wektora
    void pushHistoryEntry(const Move &move, const Move::UndoState &undoState, int prevHalfMoveCount = 0);
    /// Usuwa HistoryEntry z końca wektora
    void popHistoryEntry();
    /// Zwraca informację czy historia jest pusta
    [[nodiscard]] bool isEmpty() const;
    /// Zwraca ostatni HistoryEntry z wektora
    [[nodiscard]] const HistoryEntry &getLastHistoryEntry() const;
    /// Pozwala zmienić ostatni wpis w historii
    void setLastHistoryEntry(const HistoryEntry &historyEntry);
    /// Dodaje hasha pozycji do wektora
    void pushHash(u64 hash);
    /// Usuwa ostatni hash pozycji z historii
    void popHash(u64 hash);
    /// Zwraca informację czy nastąpiło trzykrotne powtórzenie pozycji (przeszukuje całą mapę)
    bool isThreefoldRepetition() const;
    /// Zwraca informację czy nastąpiło trzykrotne powtórzenie pozycji (sprawdza tylko ostatni hash)
    bool isThreefoldRepetition(u64 hash) const;
};
