#pragma once

#include <bitset>

#include "Board.h"
#include "GameHistory.h"

/// Zarządza logiką gry, czyli zmienia tury, zarządza historią, daje rozkazy Board
class Game {
public:

    /// Enum class przedstawiający gracza przy ruchu
    enum class Player {
        Black = 0,
        White = 1,
        None = 2 // Jeśli gramy we 2 ludzi na jednym komputerze lub sami ze sobą
    };

    /// Enum class przedstawiający możliwe zakończenia gry
    enum class EndReason {
        None,
        Stalemate,
        Mate_White,
        Mate_Black,
        Threefold_Repetition,
        Insufficient_Material,
        Fifty_Move_Rule
    };

private:

    // ===== POLA KLASY =====

    /// Pole klasy game, w którym jest plansza
    Board board;
    /// Pole, które zawiera informację o tym, czyja teraz jest tura
    Player currentPlayer = Player::None;
    /// Historia partii
    GameHistory history;
    /// Stan gry
    EndReason endReason = EndReason::None;
    /// Jeśli == 100 to gra kończy się remisem
    int halfMoveCount = 0;
    /// Ostatnie Undo
    std::optional<std::pair<Move, Move::UndoState>> lastUndo;

public:
    /// Konstruktor
    Game();

    // ===== METODY KLASY =====

    /// Zwraca czyja tura
    Player getCurrentPlayer () const;
    /// Zmienia turę
    void changeTurn();
    /// Próbuje zmienić turę i zwraca informację o powodzeniu zmiany
    bool tryMakeMove(int clickedRow, int clickedCol, std::pair<int, int> & selectedTile);
    /// Przeładowanie metody, która próbuje wykonać ruch, aby można ją było wywoływać samym ruchem
    bool tryMakeMove(Move & moveToMake);
    /// To nie próba, to rozkaz
    void makeMove(const Move & move);
    /// Cofnij ruch
    void undoMove();
    /// Ustawia bitset zmian
    std::bitset<12> getOnBoardChanges() const;
    /// Podaje planszę gry
    const Board & getBoard() const;
    /// Metoda podaje ostatni wykonany ruch
    std::optional<Move> getLastMove() const;
    /// Metoda podaje ostatni UndoState
    std::optional<Move::UndoState> getLastUndoState() const;
    /// Metoda, która zajmuje się promocją
    void handlePromotion(bool closePromotionWindow, PieceType choice = PieceType::None);
    /// Metoda daje informację czy gra się skończyła i czym się skończyła
    EndReason isGameOver() const;
    /// Metoda sprawdza, czy nie wystąpił jeden z przypadków remisowych (oprócz pata)
    void postMoveUpdate();
    /// Ustawia gracza, który się porusza
    void setCurrentPlayer();
    /// Metoda tworzy nowy kopię planszy
    std::unique_ptr<Board> copyBoard() const;

};
