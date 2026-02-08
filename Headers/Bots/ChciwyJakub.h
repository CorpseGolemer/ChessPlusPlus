#pragma once
#include <atomic>

#include "IBot.h"
#include "../Game/Board.h"

class ChciwyJakub final : public IBot {

    // ===== STRUCTY =====

    struct MoveEvaluations {
        Move move;
        int score = 0;
    };

    // ===== POLA KLASY =====

    /// \brief Osobna plansza bota
    std::unique_ptr<Board> board;
    /// \brief Mutex
    std::mutex boardMutex;
    /// \brief Informacja czy bot powinien anulować obliczenia (może kiedyś użyję)
    std::atomic<bool> shouldCancel{false};
    /// \brief Informacja czy bot dalej liczy
    std::atomic<bool> calculating{false};
    /// Ostatni ruch w grze
    std::optional<Move> lastMoveInGame;
    /// Ilość ruchów wykonanych
    std::atomic<int> madeMoves = 0;

    // ===== METODY PRYWATNE =====

    /// \brief Metoda symuluje obliczanie ruchu
    Move calculateMove();
    /// \brief Metoda oblicza najlepszy ruch (WEDŁUG BOTA XD) w pozycji
    [[nodiscard]] Move findBestMove() const;
    /// Jakub przyznaje temu ruchowi konkretną ocenę
    [[nodiscard]] MoveEvaluations evaluateMove(const Move& move) const;
    /// Jakub wybiera ruch na podstawie ocen
    static Move selectedJakubMove(const std::vector<MoveEvaluations>& evaluations);

public:
    /// \brief Kiedy bot zostaje wybrany dostaje aktualny stan planszy
    void syncFromBoard(std::unique_ptr<Board> board) override;
    /// \brief Po jakimkolwiek ruchu GameManager odpala tę funkcję botowi
    void onMoveApplied(const Move &move) override;
    /// \brief Po obliczeniu ruchu bot oddaje ruch GameManagerowi
    std::future<Move> chooseMoveAsync() override;
    /// \brief Anuluj obliczanie ruchu (np. gracz w środku liczenia postanowił kliknąć cofnięcie ruchu)
    void cancelCalculation() override;
    /// \brief Informacja czy bot dalej liczy ruch
    bool isCalculating() override;
};
