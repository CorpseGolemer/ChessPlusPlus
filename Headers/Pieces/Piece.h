
#ifndef PIECE_H
#define PIECE_H

#include <cstdint>
#include <vector>

class Board;
class Move;

using int8 = int8_t;

enum class PieceType : uint8_t {
    Rook = 0,
    Knight = 1,
    Bishop = 2,
    Queen = 3,
    King = 4,
    Pawn = 5,
    None = 6
};

// Klasa abstrakcyjna, której potomkowie to konkretne figury. Są tu wspólne pola i metody dla wszystkich figur.
class Piece {

protected:
    // Pole określające kolor figury
    bool isWhite;
    // Pole określające symbol figury
    PieceType symbol;
    // Pole określające wartość figury
    int value = 0;

    // Konstruktor klasy Piece
    Piece(bool isWhite, PieceType symbol, int value);

public:
    // Mapa przypisująca każdemu symbolowi odpowiedni znak
    static constexpr unsigned char symbolMap[6] = {'R', 'N', 'B', 'Q', 'K', 'p'};

    // Wirtualny destruktor klasy Piece
    virtual ~Piece() = default;

    [[nodiscard]]
    // Metoda zwracająca kolor
    bool getColor() const;
    [[nodiscard]]
    // Metoda zwracająca symbol
    PieceType getSymbol() const;
    /// Metoda zwraca symbol bierki jako unsigned char wraz z info o kolorze
    [[nodiscard]] unsigned char coloredSymbol() const;
    // Metoda zwracająca wartość
    [[nodiscard]] int getValue() const;
    // Metoda zapobiegająca powtarzaniu tego samego kodu dla figur, które mają nieograniczony ruch kierunkowy (w przypadku braku przeszkód (nwm czy to dobrze napisałem))
    static void addSlidingMoves(const Board& board, int8 row, int8 col, std::pmr::vector<Move> & allMoves, const int8 directions[][2], int8 dirCount);
    // Metoda zapobiegająca powtarzaniu tego samego kodu dla figur, które wykonują pojedynczy ruch w każdym z (określonych) kierunków
    static void addSingleMoves(const Board& board, int8 row, int8 col, std::pmr::vector<Move> & allMoves, const int8 moves[][2], int8 movesCount);

    // Metoda wirtualna movesInPosition generuje dostępne ruchy w danej pozycji przez konkretną bierkę i umieszcza je w wektorze allMoves
    // Dostaje referencję to planszy, swoją obecną pozycję, oraz obecny wektor ruchów
    // Jest wirtualna, ponieważ każda figura musi dać swore ruchy do allMoves, ale także każda z figur rusza się inaczej, więc metoda nie będzie wyglądać jednakowo dla wszystkich bierek
    // = 0 oznacza, że metoda jest czysto wirtualna i musi mieć implementację w klasie pochodnej, podczas gdy w tej klasie implementacji nie ma
    virtual void movesInPosition(const Board& board, int8 row, int8 col, std::pmr::vector<Move> & allMoves) = 0;

    // Używana do ustawiania pola notMoved z klasy Pawn
    virtual void setNotMoved(bool) {}
    // Używana do pobierania pola notMoved z klasy Pawn
    [[nodiscard]] virtual bool getNotMoved() const { return false; }
};



#endif //PIECE_H
