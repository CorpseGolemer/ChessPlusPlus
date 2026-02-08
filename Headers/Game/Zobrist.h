#pragma once
#include <array>
#include <chrono>
#include <random>

/// Nie chcę mi się pisać
using u64 = uint64_t;

/// Klasa, będąca hashem pozycji szachowej (przygotowanie do transposition table i innych)
class Zobrist {

    // ===== PRYWATNE STRUCTY =====

    /// Generator liczb pseudolosowych
    struct SplitMix64 {
        /// Dowolna wartość
        u64 seed;

        /// Konstruktor
        explicit SplitMix64(const u64 seed = 0) : seed(seed) {
            // Jeśli nie podamy seed, ustawi zamiast niego czas, który minął od 1 stycznia 1970
            if (this->seed == 0) {
                this->seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            }
        }

        /// Generuje liczbę pseudolosową (u64 liczba = obiekt.next())
        u64 next() {
            u64 z = (seed += 0x9e3779b97f4a7c15);
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
            return z ^ (z >> 31);
        }
    };

public:

    // ===== PUBLICZNE STRUCTY =====

    struct MT19937 {
        static std::mt19937& getInstance() {
            static std::mt19937 instance(std::random_device{}());
            return instance;
        }
        static int getInt(const int min, const int max) {
            std::uniform_int_distribution dist(min, max);
            return dist(getInstance());
        }

        static double getDouble(const double min, const double max) {
            std::uniform_real_distribution dist(min, max);
            return dist(getInstance());
        }

        static double getGauss(const double mean, const double stdDev) {
            std::normal_distribution dist(mean, stdDev);
            return dist(getInstance());
        }
    };

    // ===== POLA KLASY =====

    /// Figura (typ (figura + kolor), pole)
    static std::array<std::array<u64, 64>, 12> piece;
    /// Kto ma ruch
    static u64 sideToMove;
    /// Kolumna enPassant (rząd zależny od sideToMove)
    static std::array<u64, 8> enPassant;
    /// Prawa do roszady
    static std::array<u64, 16> castlingRights;

    // ===== METODY =====

    /// Metoda inicjalizująca, wykonywana raz na początku main
    static void init();
};