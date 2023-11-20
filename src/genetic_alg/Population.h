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
  std::vector<float> fitness;
  std::vector<Equation> equations;
  Population();

    std::vector<Equation> crossbreed();

  friend std::ostream &operator<<(std::ostream &os, const Population &population);

};


#endif //PPR_POPULATION_H
