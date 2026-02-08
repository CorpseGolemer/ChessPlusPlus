#pragma once
#include "IBot.h"

class Golemiarz final : public IBot {

    // ===== POLA KLASY =====
    /// \brief Osobna plansza dla Golema
    std::unique_ptr<Board> board;
    /// \brief Mutex
    std::mutex boardMutex;
    /// \brief Info o anulowaniu ruchu (na razie nieużywane)
    std::atomic<bool> shouldCancel{false};
    /// \brief Informacja czy bot jest w trakcie obliczania (nwm czy serio potrzebne)
    std::atomic<bool> calculating{false};
    /// Informacja, jakim kolorem gra Golemiarz
    bool myColor = false;

    /// Liczenie ruchu
    Move calculateMove();
    /// Algorytm minimax (rekurencyjny)
    int minimax(int depth, bool isMaximizing);
    /// Funkcja ocenia aktualny stan szachownicy
    [[nodiscard]] int evaluateCurrentBoard() const;

public:

    // ===== OVERRIDE =====

    /// \brief Kiedy wybrany kopiuje sobie planszę obecną
    void syncFromBoard(std::unique_ptr<Board> board) override;
    /// \brief W przypadku ruchu trzeba aktualizować board
    void onMoveApplied(const Move &move) override;
    /// \brief Funkcja rozpoczyna nowy wątek
    std::future<Move> chooseMoveAsync() override;
    /// Chyba nie trzeba tłumaczyć
    void cancelCalculation() override;
    /// Chyba nie trzeba tłumaczyć
    bool isCalculating() override;
};
