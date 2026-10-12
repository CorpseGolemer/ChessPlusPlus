#include "../../Headers/Game/Zobrist.h"

std::array<std::array<u64, 64>, 12> Zobrist::piece;
u64 Zobrist::sideToMove;
std::array<u64, 8> Zobrist::enPassant;
std::array<u64, 16> Zobrist::castlingRights;

void Zobrist::init() {
    // I'm blue (If I was green I would DIE)
    SplitMix64 generator(0xDABADEEDABADABEBULL);

    // Generowanie dla figur
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            piece[i][j] = generator.next();
        }
    }

    // Jeśli biały się rusza
    sideToMove = generator.next();

    // Każda możliwa kolumna enPassant
    for (int i = 0; i < 8; i++) {
        enPassant[i] = generator.next();
    }

    // Każda możliwa kombinacja roszad
    for (int i = 0; i < 16; i++) {
        castlingRights[i] = generator.next();
    }
}
