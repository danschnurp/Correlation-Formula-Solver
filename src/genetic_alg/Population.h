//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_POPULATION_H
#define PPR_POPULATION_H
#include <array>
#include "Equation.h"
#include "../gpu/ComputationUnit.h"
#include <memory>
#include <ostream>

float mean(const std::vector<float> &data);

class Population {
    float varLabels = 0;
    std::vector<float> precomputedLabels;

    std::unique_ptr<ComputationUnit> fPlus;
  std::unique_ptr<ComputationUnit> fMinus;
  std::unique_ptr<ComputationUnit> fMultiply;

  // wavefront parameters
  uint32_t width, height;

 public:
  int populationSize = 200;
  std::vector<float> fitness;
  std::vector<Equation> equations;

  /**
   * The Population constructor initializes a population of equations and their fitness values, and creates computation units
   * for addition, subtraction, and multiplication.
   */
  Population();

  /**
   * The function calculates the Pearson correlation coefficient between the input vector and precomputed labels.
   *
   * @param x A vector of float values representing the input data.
   *
   * @return the absolute value of the Pearson correlation coefficient between the input vector `x` and the precomputed
   * labels.
   */
  float countFitFunction(const std::vector<float> &x);

  /**
   * The function prepares the population for fitness evaluation by calculating the height and width of the wavefront,
   * subtracting the mean from each label, and calculating the variance of the labels.
   *
   * @param y The parameter "y" is a vector of floats representing a set of labels or target values.
   */
  void prepareForFitFunction(const std::vector<float> &y);

  /**
   * The function `crossbreed()` takes a population of equations, performs crossover between pairs of equations, and returns
   * the resulting children equations.
   *
   * @return The function `crossbreed()` returns a vector of Equation objects, which are the children generated through
   * crossover breeding.
   */
  std::vector<Equation> crossbreed();

  /**
   * The function evaluates a set of equations using given input vectors and returns the result.
   *
   * @param x A vector of float values representing the x-coordinates of the points in the population.
   * @param y The parameter `y` is a vector of float values. It is being passed as a reference to the `evaluate` function.
   * @param z The parameter `z` is a vector of float values. It is being used as one of the input variables in the evaluation
   * of equations.
   * @param equationIndex The `equationIndex` parameter is an integer that represents the index of the equation to be
   * evaluated. It is used to access the equation from the `equations` vector.
   *
   * @return The function `evaluate` returns a `std::vector<float>` containing the evaluated values based on the given
   * equations and input vectors.
   */
  std::vector<float> evaluate(std::vector<float> &x, std::vector<float> &y,
                              std::vector<float> &z, int equationIndex);
  /**
   * The function evaluates a set of equations using given input vectors and returns the results.
   *
   * @param x A vector of float values representing the x-coordinates of the points to be evaluated.
   * @param y The parameter "y" is a vector of float values.
   * @param z The parameter "z" is a vector of float values.
   * @param equationIndex The `equationIndex` parameter is an integer that represents the index of the equation to be
   * evaluated. It is used to access the equation from the `equations` vector.
   *
   * @return a vector of floats, which contains the results of evaluating the equations for each set of x, y, and z values.
   */
  std::vector<float> evaluateCPU(const std::vector<float> &x, const std::vector<float> &y,
                                 const std::vector<float> &z, int equationIndex);

  friend std::ostream &operator<<(std::ostream &os, const Population &population);

};


#endif //PPR_POPULATION_H
