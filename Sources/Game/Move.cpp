#include "../../Headers/Game/Move.h"
#include "../../Headers/Pieces/Piece.h"
#include <iostream>

unsigned char Move::symbolMap[7] = {'R', 'N', 'B', 'Q', 'K', 'P', 'X'};

const std::pair<int8, int8> & Move::getPosition() const {
    return position;
}

const std::pair<int8, int8> & Move::getNextPosition() const {
    return nextPosition;
}

bool Move::hasMoveFlag(const MoveFlags flag) const {
    return (this->flags & flag) != 0;
}

PieceType Move::getPieceSymbol() const {
    return this->symbol;
}

bool Move::operator==(const Move & move) const {
    return
    this->position.first == move.position.first
    && this->position.second == move.position.second
    && this->nextPosition.first == move.nextPosition.first
    && this->nextPosition.second == move.nextPosition.second;
}

void Move::debugPrint() const {
    std::cout << symbolMap[static_cast<unsigned char>(symbol)] << " " << static_cast<int>(position.first) << "-" << static_cast<int>(position.second) << " to " << static_cast<int>(nextPosition.first) << "-"<< static_cast<int>(nextPosition.second) << std::endl;
    std::cout << "isCapture: "<< hasMoveFlag(Capture) << std::endl;
    std::cout << "isCastleShort: " << hasMoveFlag(CastleShort) <<std::endl;
    std::cout << "isCastleLong: " << hasMoveFlag(CastleLong) << std::endl;
    std::cout << "isCheck: " << hasMoveFlag(Check) << std::endl;
    std::cout << "isMate: " << hasMoveFlag(Mate) << std::endl;
    std::cout << "isPromotion: " << hasMoveFlag(Promotion) << std::endl;
    std::cout << "isEnPassant: " << hasMoveFlag(EnPassant) << std::endl;
    std::cout << "isStaleMate: " << hasMoveFlag(Draw) << std::endl;
    std::cout << static_cast<int>(promotedPiece) << std::endl;
    std::cout << std::endl;
}

void Move::addFlag(const MoveFlags flag) {
    flags = static_cast<MoveFlags>(flags | flag);
}

void Move::UndoState::debugPrint() const {
    if (capturedPiece != nullptr) std::cout << "Captured piece: " << capturedPiece->coloredSymbol() << std::endl;
    if (promotedPiece != nullptr) std::cout << "Promoted piece: " << static_cast<int>(promotedPiece->coloredSymbol()) << std::endl;
    for (const auto right : castlingRights) std::cout << castlingRights[right] << std::endl;
    std::cout << static_cast<int>(prevEnPassant.first) <<" " << static_cast<int>(prevEnPassant.second)<< std::endl;
    for (auto [fst, snd] : kingPositions) std::cout << static_cast<int>(fst) << " " << static_cast<int>(snd) << std::endl;
}



