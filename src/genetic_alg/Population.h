//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_POPULATION_H
#define PPR_POPULATION_H
#include <array>
#include "Equation.h"
#include <memory>
#include <ostream>

class Population {
public:
    int populationSize = 200;
    std::array<std::shared_ptr<Equation>, 200> equations;
    Population();
    friend std::ostream &operator<<(std::ostream &os, const Population &population);

};


#endif //PPR_POPULATION_H
