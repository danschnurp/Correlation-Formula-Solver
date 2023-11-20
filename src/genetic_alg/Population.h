//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_POPULATION_H
#define PPR_POPULATION_H
#include <array>
#include "Equation.h"
#include "../gpu/demo/example_filter.h"
#include <memory>
#include <ostream>

class Population {
    float varLabels = 0;
    std::vector<float> precomputedLabels;

    std::unique_ptr<ExampleFilter> fPlus;
    std::unique_ptr<ExampleFilter> fMinus;
    std::unique_ptr<ExampleFilter> fMultiply;
    std::unique_ptr<ExampleFilter> fDivide;

    // wavefront parameters
    uint32_t width, height;

 public:
  int populationSize = 200;
  std::vector<float> fitness;
  std::vector<Equation> equations;
  Population();

  float countFitFunction(const std::vector<float> &x);

  void prepareForFitFunction(const std::vector<float> &y);

  std::vector<Equation> crossbreed();

  std::vector<float> evaluate(const std::vector<float>& x, const std::vector<float>& y,
                                            const std::vector<float>& z, int equationIndex);

  std::vector<float> evaluateCPU(const std::vector<float>& x, const std::vector<float>& y,
                                               const std::vector<float>& z, int equationIndex);

  friend std::ostream &operator<<(std::ostream &os, const Population &population);

};


#endif //PPR_POPULATION_H
