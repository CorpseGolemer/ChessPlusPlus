#include "../../Headers/Game/Board.h"
#include <iostream>

#include "../../Headers/Pieces/Bishop.h"
#include "../../Headers/Pieces/Knight.h"
#include "../../Headers/Pieces/Pawn.h"
#include "../../Headers/Pieces/Queen.h"
#include "../../Headers/Pieces/Rook.h"
#include "../../Headers/Pieces/King.h"

/// Implementacja konstruktora klasy Board
Board::Board() {
    setStartingPosition();
    computeFullHash();
}

/// Implementacja destruktora
Board::~Board() {
    pieces.clear();
}

/// Implementacja metody setStartingPosition
void Board::setStartingPosition() {
    // Ustawiamy wszystko w starej pozycji na nullptr
    for (auto& i : board) {
        for (auto& j : i) {
            j = nullptr;
        }
    }
    // Czyścimy figury
    pieces.clear();

    // ===== BIAŁE FIGURY =====

    pieces.push_back(std::make_unique<Rook>(true));
    board[0][0] = pieces.back().get();

    pieces.push_back(std::make_unique<Knight>(true));
    board[0][1] = pieces.back().get();

    pieces.push_back(std::make_unique<Bishop>(true));
    board[0][2] = pieces.back().get();

    pieces.push_back(std::make_unique<Queen>(true));
    board[0][3] = pieces.back().get();

    pieces.push_back(std::make_unique<King>(true));
    board[0][4] = pieces.back().get();

    pieces.push_back(std::make_unique<Bishop>(true));
    board[0][5] = pieces.back().get();

    pieces.push_back(std::make_unique<Knight>(true));
    board[0][6] = pieces.back().get();

    pieces.push_back(std::make_unique<Rook>(true));
    board[0][7] = pieces.back().get();

    // ===== CZARNE FIGURY =====

    pieces.push_back(std::make_unique<Rook>(false));
    board[7][0] = pieces.back().get();

    pieces.push_back(std::make_unique<Knight>(false));
    board[7][1] = pieces.back().get();

    pieces.push_back(std::make_unique<Bishop>(false));
    board[7][2] = pieces.back().get();

    pieces.push_back(std::make_unique<Queen>(false));
    board[7][3] = pieces.back().get();

    pieces.push_back(std::make_unique<King>(false));
    board[7][4] = pieces.back().get();

    pieces.push_back(std::make_unique<Bishop>(false));
    board[7][5] = pieces.back().get();

    pieces.push_back(std::make_unique<Knight>(false));
    board[7][6] = pieces.back().get();

    pieces.push_back(std::make_unique<Rook>(false));
    board[7][7] = pieces.back().get();

    // ===== PIONY =====

    for (int i=0; i<16; i++) {
        if (i <= 7) {
            pieces.push_back(std::make_unique<Pawn>(true));
            board[1][i] = pieces.back().get();
        }
        else {
            pieces.push_back(std::make_unique<Pawn>(false));
            board[6][i-8] = pieces.back().get();
        }
    }

}

/// Implementacja metody uzupełniającej wektor allMoves
void Board::findAllMoves() {
    // Wygeneruj powierzchowne ruchy dla tej pozycji
    std::pmr::vector<Move> allMoves = findShallowMoves();
    std::pmr::vector<Move> legalMoves;
    legalMoves.reserve(allMoves.size());

    const bool myColor = whitesTurn;
    const bool oppColor = !whitesTurn;


    for (auto& move : allMoves) {
        Move::UndoState tempUndo;
        applyMove(move, tempUndo);

        if (isSquareAttacked(kingPositions[myColor], oppColor)) {
            undoMove(move, tempUndo);
            continue;
        }

        if (move.hasMoveFlag(CastleShort)) {
            std::pair<int8, int8> pos {myColor ? 0 : 7, 5};
            if (isSquareAttacked(pos, oppColor)) {
                undoMove(move, tempUndo);
                continue;
            }
        }
        else if (move.hasMoveFlag(CastleLong)) {
            std::pair<int8, int8> pos {myColor ? 0 : 7, 3};
            // Pole przelotowe (3) nie może być atakowane
            if (isSquareAttacked(pos, oppColor)) {
                undoMove(move, tempUndo);
                continue;
            }
        }

        if (isSquareAttacked(kingPositions[oppColor], myColor)) {
            move.addFlag(Check);
        }

        legalMoves.push_back(move);
        undoMove(move, tempUndo);
    }

    this->allMoves = legalMoves;

}

/// Implementacja findShallowMoves
std::pmr::vector<Move> Board::findShallowMoves() const {
    std::pmr::vector<Move> allMoves;
    for (int8 i = 0; i < 8; i++) {
        for (int8 j = 0; j < 8 ; j++) {
            if (Piece* piece = board[i][j]; piece != nullptr) {
                if(piece->getColor() == whitesTurn) {
                    piece->movesInPosition(*this, i, j, allMoves);
                }
            }
        }
    }
    return allMoves;
}


/// Implementacja getFullMove
std::optional<Move> Board::getFullMove(const Move &shallowMove) {
    for (const auto & move : getAllMoves()) {
        if (move == shallowMove) {
            return move;
        }
    }
    return std::nullopt;
}

/// Implementacja createMoveUndoImage
Move::UndoState Board::createMoveUndoImage(const Move &move) {
    Move::UndoState undoState;
    // Zapisujemy szczegóły obecnej pozycji
    for (int i = 0; i < 4; i++) undoState.castlingRights[i] = castlingRights[i];
    if (enPassantTarget.has_value()) undoState.prevEnPassant = enPassantTarget.value();
    else undoState.prevEnPassant = {-1, -1};
    handlePawnMoveLogic(move, &undoState);

    // Jeśli zbijamy coś
    if (move.hasMoveFlag(Capture)) {
        undoState.capturedPiece = board[move.getNextPosition().first][move.getNextPosition().second];
    }
    // Jeśli zbijamy coś jako enPassant
    else if (move.hasMoveFlag(EnPassant)) {
        undoState.capturedPiece = board[move.getPosition().first][move.getNextPosition().second];
    }

    // Jeśli następuje promocja
    if (move.hasMoveFlag(Promotion)) {
        undoState.promotedPiece = board[move.getPosition().first][move.getPosition().second];
    }

    undoState.kingPositions[0] = this->kingPositions[0];
    undoState.kingPositions[1] = this->kingPositions[1];
    undoState.wasCheck = this->isCheck;

    return undoState;
}

/// Nie wierzę, że tu jeszcze coś dodaję
bool Board::isSquareAttacked(std::pair<int8, int8> &square, const bool byColor) const {
    const auto [r, c] = square;

    // PIONY
    const int attackRow = byColor ? r - 1 : r + 1;
    if (isOnBoard({attackRow, c + 1})) {
        Piece* p = board[attackRow][c + 1];
        if (p != nullptr && p->getColor() == byColor && p->getSymbol() == PieceType::Pawn) return true;
    }
    if (isOnBoard({attackRow, c - 1})) {
        Piece* p = board[attackRow][c - 1];
        if (p != nullptr && p->getColor() == byColor && p->getSymbol() == PieceType::Pawn) return true;
    }

    // SKOCZKI
    constexpr int nMoves[8][2] = {{-2,-1}, {-2,1}, {-1,-2}, {1,-2} , {1,2}, {2,-1} ,{2,1}, {-1,2}};
    for (auto& nM : nMoves) {
        const int nr = r + nM[0], nc = c + nM[1];
        if (isOnBoard({nr,nc})) {
            Piece* p = board[nr][nc];
            if (p != nullptr && p->getColor() == byColor && p->getSymbol() == PieceType::Knight) return true;
        }
    }

    /// LINIE PROSTE
    constexpr int rMoves[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
    for (auto& rM : rMoves) {
        for (int k = 1; k < 8; k++) {
            int nr = r + rM[0] * k, nc = c + rM[1] * k;
            if (!isOnBoard({nr,nc})) break;
            Piece* p = board[nr][nc];
            if (p != nullptr) {
                if (p->getColor() == byColor && (p->getSymbol() == PieceType::Rook || p->getSymbol() == PieceType::Queen)) return true;
                break;
            }
        }
    }

    // SKOSY
    constexpr int bMoves[4][2] = {{1,1}, {-1,-1}, {1,-1}, {-1,1}};
    for (auto& bM : bMoves) {
        for (int k = 1; k < 8; k++) {
            int nr = r + bM[0] * k, nc = c + bM[1] * k;
            if (!isOnBoard({nr,nc})) break;
            Piece* p = board[nr][nc];
            if (p != nullptr) {
                if (p->getColor() == byColor && (p->getSymbol() == PieceType::Queen || p->getSymbol() == PieceType::Bishop)) return true;
                break;
            }
        }
    }

    // KRÓL
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int nr = r + i, nc = c + j;
            if (isOnBoard({nr,nc})) {
                Piece* p = board[nr][nc];
                if (p != nullptr && p->getColor() == byColor && p->getSymbol() == PieceType::King) return true;
            }
        }
    }

    return false;
}


/// Implementacja metody canCastleShort
bool Board::canCastleShort(const bool forColor) const {
    const int row = forColor ? 0 : 7;
    if (!castlingRights[forColor]) return false;
    if (board[row][5] != nullptr || board[row][6] != nullptr) return false;
    if (inCheck()) return false;
    return true;
}

/// Implementacja metody canCastleLong
bool Board::canCastleLong(const bool forColor) const {
    const int row = forColor ? 0 : 7;
    if (!castlingRights[2 + forColor]) return false;
    if (board[row][1] != nullptr || board[row][2] != nullptr || board[row][3] != nullptr) return false;
    if (inCheck()) return false;
    return true;
}

/// Implementacja metody getEnPassant
std::pair<int8,int8> Board::getEnPassant() const {
    if (!this->enPassantTarget.has_value()) {
        return {-1, -1};
    }
    return this->enPassantTarget.value();
}

/// Implementacja metody, która sprawdza, czy wektor allMoves jest pusty i jeśli tak to wywołuje findAllMoves
std::pmr::vector<Move> & Board::getAllMoves() {
    // Jeśli allMoves puste wygeneruj nowe
    if (!allMoves.has_value()) {
        findAllMoves();
    }
    return *allMoves;
}

/// Implementacja getMoves
std::optional<std::pmr::vector<Move>> &Board::getMoves() const {
    return allMoves;
}

/// Implementacja setTurn
void Board::setTurn(const bool whitesTurn) {
    this->whitesTurn = whitesTurn;
}

/// Implementacja metody getPiece
Piece *Board::getPiece(const int8 row, const int8 col) const {
    return board[row][col];
}

Piece *Board::getPiece(const std::pair<int8, int8> &pos) const {
    return board[pos.first][pos.second];
}

/// Implementacja getZobristHash
u64 Board::getZobristHash() const {
    return zobristHash;
}

/// Implementacja isPromoting
int Board::isPromoting() const {
    if (pendingPromotion.has_value()) {
        return pendingPromotion.value().getNextPosition().second;
    }
    return -1;
}

/// Implementacja resetPromotion
void Board::resetPromotion() {
    this->pendingPromotion.reset();
}

bool Board::isWhitesTurn() const {
    return this->whitesTurn;
}

void Board::copy(const std::unique_ptr<Board>& board) const {
    board->whitesTurn = this->whitesTurn;
    board->isCheck = this->isCheck;
    board->zobristHash = this->zobristHash;
    board->enPassantTarget = this->enPassantTarget;
    board->castlingRights = this->castlingRights;
    board->kingPositions = this->kingPositions;
}

/// Implementacja hasFlag
bool Board::inCheck() const {
    return this->isCheck;
}

/// computeFullHash
void Board::computeFullHash() {
    zobristHash = 0;

    for (int i = 0; i < BOARD_SIZE_X; i++) {
        for (int j = 0; j < BOARD_SIZE_Y; j++) {
            if (board[i][j] != nullptr) {
                const int square = 8 * i + j;
                const PieceType symbol = board[i][j]->getSymbol();
                const unsigned char pieceSymbol = static_cast<unsigned char>(symbol) * 2 + board[i][j]->getColor();

                zobristHash ^= Zobrist::piece[pieceSymbol][square];
            }
        }
    }

    if (whitesTurn) zobristHash ^= Zobrist::sideToMove;

    if (enPassantTarget.has_value()) {
        zobristHash ^= Zobrist::enPassant[enPassantTarget.value().second];
    }

    int castling = 0;
    for (int i = 0; i < 4; i++) {
        castling += castlingRights[i] * (1<<i);
    }
    zobristHash ^= Zobrist::castlingRights[castling];
}

/// Implementacja isInsufficientMaterial
bool Board::isInsufficientMaterial() const {
    int minorCount = 0;
    int bishopCount = 0;
    std::optional<int> bishopSquareColor;

    for(int i = 0; i < BOARD_SIZE_X; i++) {
        for(int j = 0; j < BOARD_SIZE_Y; j++) {
            if (const Piece* piece = board[i][j]; piece != nullptr) {
                const PieceType symbol = piece->getSymbol();

                // Pomiń króla
                if (symbol == PieceType::King) continue;
                // Jeśli w pozycji jest wieża, pion, lub królowa nie ma remisu
                if (symbol == PieceType::Rook || symbol == PieceType::Queen || symbol == PieceType::Pawn) return  false;

                // Skoczek
                if (symbol == PieceType::Knight) {
                    minorCount++;
                    if (minorCount > 1) return false;
                }

                // Goniec
                if (symbol == PieceType::Bishop) {
                    minorCount++;
                    bishopCount++;

                    const int squareColor = (i+j) % 2;
                    if (!bishopSquareColor.has_value()) bishopSquareColor = squareColor;
                    else if (bishopSquareColor.value() != squareColor) return false;
                }
            }
        }
    }
    if (minorCount <= 1) return true;
    if (minorCount == bishopCount) return true;
    return false;
}

/// Implementacja metody tryMakeMove
bool Board::tryMakeMove(Move & move) {
    if (const auto fullMove = getFullMove(move)) {
        move = fullMove.value();
        if (move.promotedPiece != PieceType::None) {
            move.promotedPiece = PieceType::None;
        }
        return true;
    }
    std::cout << "Invalid move" << std::endl;
    return false;
}

/// Implementacja metody fizycznie wykonującej ruch
void Board::applyMove(const Move & move, Move::UndoState& undoState, const bool verbose) {
    const auto [fst, snd] = move.getPosition();
    const auto [n_fst, n_snd] = move.getNextPosition();
    assert(board[fst][snd] != nullptr && "APPLY: NIE MA BIERKI NA POLU STARTOWYM");
    const bool movingColor = board[fst][snd]->getColor();

    // Stwórz obraz odtwarzania ruchu
    undoState = createMoveUndoImage(move);
    // Update informacji o roszadzie
    updateKingRookFlags(move, movingColor);
    // Dodaj logikę, jeśli figura to pion
    handlePawnMoveLogic(move, nullptr);

    if (move.hasMoveFlag(Promotion) && move.promotedPiece == PieceType::None) {
        pendingPromotion = move;
    }

    // Usuwanie pionów
    if (move.hasMoveFlag(EnPassant)) {
        board[fst][n_snd] = nullptr;
    }
    board[n_fst][n_snd] = board[fst][snd];


    if (move.hasMoveFlag(Promotion) && move.promotedPiece < PieceType::None) {
        createPiece(move.promotedPiece, movingColor, {n_fst, n_snd});
    }

    board[fst][snd] = nullptr;

    if (move.hasMoveFlag(Check)) {
        this->isCheck = true;
    }
    else {
        this->isCheck = false;
    }

    if (move.hasMoveFlag(CastleShort)) {
        const int row = movingColor ? 0 : 7;
        board[row][5] = board[row][7];
        board[row][7] = nullptr;
    }
    if (move.hasMoveFlag(CastleLong)) {
        const int row = movingColor ? 0 : 7;
        board[row][3] = board[row][0];
        board[row][0] = nullptr;
    }
}

/// Implementacja undoMove
void Board::undoMove(const Move & move, const Move::UndoState & undoState, const bool verbose) {
    const auto [fst, snd] = move.getPosition();
    const auto [n_fst, n_snd] = move.getNextPosition();
    assert(board[n_fst][n_snd] != nullptr && "UNDO: Brak figury na miejscu docelowym!");

    const bool movingColor = board[n_fst][n_snd]->getColor();

    board[fst][snd] = board[n_fst][n_snd];

    if (move.hasMoveFlag(EnPassant)) {
        assert(undoState.capturedPiece != nullptr && "UNDO: Po biciu EnPassant capturedPiece = nullptr");
        board[fst][n_snd] = undoState.capturedPiece;
        board[n_fst][n_snd] = nullptr;
    }
    else {
        board[n_fst][n_snd] = undoState.capturedPiece;
    }

    if (move.getPieceSymbol() == PieceType::Pawn) board[fst][snd]->setNotMoved(undoState.pawnNotMoved);

    for (int i = 0; i <4; i++) { castlingRights[i] = undoState.castlingRights[i]; }

    if (undoState.prevEnPassant.first != -1) enPassantTarget = undoState.prevEnPassant;
    else enPassantTarget.reset();

    this->kingPositions[0] = undoState.kingPositions[0];
    this->kingPositions[1] = undoState.kingPositions[1];
    this->isCheck = undoState.wasCheck;

    const int row = movingColor ? 0 : 7;
    if (move.hasMoveFlag(CastleShort)) {
        assert(board[row][5] && "UNDO: Złe cofnięcie roszady, nie ma wieży na odpowiednim polu");
        board[row][7] = board[row][5];
        board[row][5] = nullptr;
    }
    else if (move.hasMoveFlag(CastleLong)) {
        assert(board[row][3] && "UNDO: Złe cofnięcie roszady, nie ma wieży na odpowiednim polu");
        board[row][0] = board[row][3];
        board[row][3] = nullptr;
    }


    if (move.hasMoveFlag(Promotion)) {
        assert(undoState.promotedPiece != nullptr && "UNDO: Promoted piece = nullptr");
        board[fst][snd] = undoState.promotedPiece;
        if (move.promotedPiece < PieceType::None) {
            pieces.pop_back();
        }
    }
}

/// Implementacja applyPromotion
Move Board::applyPromotion(const PieceType choice) {
    Move moveToMake;
    for (auto & move : this->allMoves.value()) {
        if (move == pendingPromotion.value() && move.promotedPiece == choice) {
            moveToMake = move;
        }
    }
    pendingPromotion.reset();
    return moveToMake;
}


/// Zmniejsza bałagan w funkcji applyMove
void Board::handlePawnMoveLogic(const Move &move, Move::UndoState* undoState) {
    if (undoState != nullptr) {
        if (move.getPieceSymbol() == PieceType::Pawn) {
            const Piece * piece = board[move.getPosition().first][move.getPosition().second];
            undoState->pawnNotMoved = piece->getNotMoved();
        }
        return;
    }
    // Sprawdzamy, czy pion
    if (move.getPieceSymbol() != PieceType::Pawn) {
        enPassantTarget.reset();
        return;
    }

    // Jeśli tak to pobieramy pozycję i figurę
    const auto [fst, snd] = move.getPosition();
    const auto [n_fst, n_snd] = move.getNextPosition();
    const auto pawn = board[fst][snd];

    // Ustawiamy, że pion został już ruszony
    pawn->setNotMoved(false);
    // Sprawdzanie, czy nie robi 2 do przodu
    if (abs(n_fst - fst) == 2) {
        // Jeśli tak to ustaw enPassantTarget
        enPassantTarget = {(fst + n_fst)/2, n_snd};
    }
    else enPassantTarget.reset();
}

/// Zmniejsza bałagan w funkcji applyMove
void Board::updateKingRookFlags(const Move &move, const bool color) {
    // Sprawdzanie, czy rusza się król lub wieża
    if (move.getPieceSymbol() == PieceType::King) {
        castlingRights[color] = castlingRights[2 + color] = false;
        kingPositions[color] = move.getNextPosition();
    }
    else if (move.getPieceSymbol() == PieceType::Rook) {
        if (move.getPosition().second == 7) castlingRights[color] = false;
        else if (move.getPosition().second == 0) castlingRights[2 + color] = false;
    }

    if (move.hasMoveFlag(Capture)) {
        if (move.getNextPosition() == std::pair{0,0}) castlingRights[3] = false;
        else if (move.getNextPosition() == std::pair{7,0}) castlingRights[2] = false;
        else if (move.getNextPosition() == std::pair{0,7}) castlingRights[1] = false;
        else if (move.getNextPosition() == std::pair{7,7}) castlingRights[0] = false;
    }
}

/// Metoda tworzy nową figurę w ustalonej pozycji
void Board::createPiece(const PieceType symbol, const bool isWhite, const std::pair<int8,int8> & position) {
    board[position.first][position.second] = nullptr;

    switch (symbol) {
        case PieceType::Queen: {pieces.push_back(std::make_unique<Queen>(isWhite)); break;}
        case PieceType::Rook: {pieces.push_back(std::make_unique<Rook>(isWhite)); break;}
        case PieceType::Knight: {pieces.push_back(std::make_unique<Knight>(isWhite)); break;}
        case PieceType::Bishop: {pieces.push_back(std::make_unique<Bishop>(isWhite)); break;}
        default: {board[position.first][position.second] = nullptr; return;}
    }

    board[position.first][position.second] = pieces.back().get();

}

/// Implementacja metody, która sprawdza, czy znajdujemy się na szachownicy
bool Board::isOnBoard(const std::pair<int8, int8> & position) {
    return position.first >= 0 && position.first < BOARD_SIZE_X && position.second >= 0 && position.second < BOARD_SIZE_Y;
}











