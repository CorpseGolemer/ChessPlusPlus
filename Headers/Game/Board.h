#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <memory>
#include <vector>
#include <optional>
#include "../Pieces/Piece.h"
#include "Move.h"
#include "Zobrist.h"

/// Klasa planszy przechowuje planszę jako arry'a Piece*, ustawia pozycję startową, zarządza systemem ruchów.
class Board {
public:

    // ===== PUBLICZNE CONSTEXPR =====

    /// Wielkość planszy. BOARD_SIZE_X x BOARD_SIZE_Y
    static constexpr int BOARD_SIZE_X = 8, BOARD_SIZE_Y = 8;

private:

    // ===== PRYWATNE POLA KLASY =====

    /// Hash obecnej pozycji
    u64 zobristHash = 0;
    /// Wektor, będący właścicielem wszystkich figur
    std::vector<std::unique_ptr<Piece>> pieces;
    /// Prywatna plansza do gry
    std::array<std::array<Piece*, BOARD_SIZE_X>, BOARD_SIZE_Y> board{};
    /// Trzyma ruchy dla danej pozycji, żeby ich nie generować za każdym razem
    mutable std::optional<std::pmr::vector<Move>> allMoves;
    /// Flagi pozycji (czyja tura)
    bool whitesTurn = true;
    /// Czy jest szach
    bool isCheck = false;
    /// Para int'ów wskazująca, na które pole wejdzie pion bijący w przelocie
    std::optional<std::pair<int8, int8>> enPassantTarget;
    /// Bool'e wskazujące, czy dalej mamy prawa do konkretnej roszady, potrzebne, aby umożliwić roszowanie
    std::array<bool, 4> castlingRights{true, true, true, true};
    /// Dwie pary int'ów wskazujące pozycje konkretnego króla
    std::array<std::pair<int8, int8>, 2> kingPositions = {
        {
            {7,4},
            {0,4}
        }
    };
    /// Promocja
    std::optional<Move> pendingPromotion;

    // ===== PRYWATNE METODY POMOCNICZE =====

    /// Metoda ustawiająca na planszy standardową pozycję startową w szachach
    void setStartingPosition();
    /// Metoda uzupełniająca wektor wszystkich LEGALNYCH ruchów dla danej pozycji
    void findAllMoves();
    /// Metoda zwraca tylko ruchy POWIERZCHOWNE dla WYBRANEGO koloru (bez sprawdzania, szacha itp.)
    [[nodiscard]] std::pmr::vector<Move> findShallowMoves() const;
    /// Metoda oddaje pełny ruch z allMoves na podstawie podanego niepełnego (same pozycje)
    std::optional<Move> getFullMove(const Move & shallowMove);
    /// Pomocnicza metoda, która zajmuje się ruchami pionów (oczyszcza trochę applyMove)
    void handlePawnMoveLogic(const Move & move, Move::UndoState* undoState);
    /// Pomocnicza metoda, która aktualizuje prawa roszady
    void updateKingRookFlags(const Move & move, bool color);
    /// Metoda tworzy obraz do odwrócenia podanego ruchu (!!NIE WYKONUJE GO!!, !!OBOWIĄZKOWO W APPLY MOVE!!)
    Move::UndoState createMoveUndoImage(const Move & move);
    /// Nowa metoda dodana dzisiaj to jest 27.1.2026, ulepszenie wydajności
    bool isSquareAttacked(std::pair<int8,int8>& square, bool byColor) const;

public:
    /// Konstruktor
    Board();
    /// Destruktor
    ~Board();

    /// NIE MA KONSTRUKTORA KOPIUJĄCEGO
    Board(const Board &) = delete;

    // ===== METODY SPRAWDZAJĄCE MOŻLIWOŚĆ WYKONANIA ROSZADY =====

    /// Zwraca czy można wykonać krótką roszadę
    bool canCastleShort(bool forColor) const;
    /// Zwraca czy można wykonać długą roszadę
    bool canCastleLong(bool forColor) const;

    // ===== GETTERY I SETTERY =====

    /// Metoda zwracająca obecne pole, na którym można zbić w przelocie
    std::pair<int8,int8> getEnPassant() const;
    /// Metoda, jeśli wektora allMoves nie ma, to go tworzy, a jeśli jest to go podaje
    std::pmr::vector<Move> & getAllMoves();
    /// Metoda podaje wektor, a jeśli go nie ma, to nie podaje nic (zwykle podajemy pod przez const &, więc musi korzystać z const metod)
    [[nodiscard]] std::optional<std::pmr::vector<Move>> & getMoves() const;
    /// Metoda wywoływana tylko przez Game ustawia kto się obecnie rusza
    void setTurn(bool whitesTurn);
    /// Wybiera konkretny Piece* z array''a board
    [[nodiscard]] Piece* getPiece(int8 row, int8 col) const;
    [[nodiscard]] Piece* getPiece(const std::pair<int8, int8>& pos) const;
    /// Metoda zwraca hash obecnej pozycji
    u64 getZobristHash() const;
    /// Metoda zwraca informację czy trwa promocja (if >0) i kolumnę promocji (0-7)
    int isPromoting() const;
    /// Metoda resetuje pendingPromotion z zewnątrz
    void resetPromotion();
    /// Metoda zwraca aktualnego gracza przy ruchu
    bool isWhitesTurn() const;
    /// Metoda kopiuje pola tego board, ale pieces i board zostają stare
    void copy(const std::unique_ptr<Board>& board) const;

    // ===== METODY INFORMACJI O POZYCJI =====

    /// Sprawdza, czy pozycja ma daną flagę
    [[nodiscard]] bool inCheck() const;
    /// Metoda tylko do debug przelicza całego hasha pozycji od zera
    void computeFullHash();
    /// Sprawdza, czy materiał na szachownicy jest niewystarczający, by wyłonić zwycięzcę
    bool isInsufficientMaterial() const;
    /// Sprawdza, czy wybrana strona ma za mało materiału, aby dać mata
    bool isInsufficientMaterial(bool color) const;

    // ===== METODY ZARZĄDZANIA PLANSZĄ =====

    /// Metoda zwraca informację czy ruch jest możliwy do wykonania
    bool tryMakeMove(Move & move);
    /// Metoda, która fizycznie wykonuje ruch na board (tworzy też obraz cofnięcia wykonywanego ruchu)
    void applyMove(const Move & move, Move::UndoState& undoState, bool verbose = false);
    /// Metoda, która odpowiada za cofnięcie wykonanego ruchu
    void undoMove(const Move & move, const Move::UndoState & undoState, bool verbose = false);
    /// Metoda, która zmienia piona promocyjnego na figurę wybraną przez gracza
    Move applyPromotion(PieceType choice);
    /// Tworzy nową figurę na podanej pozycji
    void createPiece(PieceType symbol, bool isWhite, const std::pair<int8,int8> & position);
    /// Metoda zwraca true, jeśli dane współrzędne znajdują się na szachownicy (!!!DANYCH NIE PODAWAĆ W PIKSELACH!!!)
    static bool isOnBoard(const std::pair<int8, int8> & position);
    /// Metoda generuje ruchy (wywołuje findAllMoves, ale jest publiczna więc można użyć z zewnątrz)
    void generateMoves() {findAllMoves();}
};

#endif // BOARD_H
