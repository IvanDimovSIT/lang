#include "RandomGenerator.h"

double RandomGenerator::generateRandom() {
    static std::random_device rand;
    static std::mt19937 generator(rand());
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}
