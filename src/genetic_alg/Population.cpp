//
// Created by Dan Schnurpfeil on 06.11.2023.
//


#include "Population.h"

Population::Population() {
    for (int i = 0; i < populationSize; ++i) {
        equations[i] = std::make_shared<Equation>();
    }
}

std::ostream &operator<<(std::ostream &os, const Population &population) {
    for (int i = 0; i < population.populationSize; ++i) {
        os << *population.equations[i];
    }
    return os;
}
