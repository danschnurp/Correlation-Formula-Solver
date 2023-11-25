//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_POPULATION_H
#define PPR_POPULATION_H
#include <array>
#include "Equation.h"
#include "../gpu/ComputationUnit.h"
#include "../io/RecordACC.h"
#include <memory>
#include <ostream>

float mean(const std::vector<float> &data);

void print_time(auto start_time);

class Population {
 private:
  float varLabels = 0;
  std::vector<float> precomputedLabels;

  std::unique_ptr<ComputationUnit> fPlus;
  std::unique_ptr<ComputationUnit> fMinus;
  std::unique_ptr<ComputationUnit> fMultiply;

  bool useGpu = true;

  // wavefront parameters
  uint32_t width, height;

  // determining x / y / Z
  std::vector<std::vector<float>> xyzVector;

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
   * The function `crossbreed()` takes a population of equations, performs crossover between pairs of equations, and returns
   * the resulting children equations.
   *
   * @return The function `crossbreed()` returns a vector of Equation objects, which are the children generated through
   * crossover breeding.
   */
  std::vector<Equation> crossbreed();

  std::vector<float> evaluate(const Equation &equation);
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
                                 const std::vector<float> &z, Equation &equation);

  void compute_fitness();

 public:
  int populationSize = 200;
  std::vector<float> fitness;
  std::vector<Equation> equations;
  std::vector<float> fitness_children;
  std::vector<Equation> equations_children;

  Population(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data, bool useGpu);

  void create_one_generation(int wave);

  friend std::ostream &operator<<(std::ostream &os, const Population &population);

/**
 * The function prepares the population for fitness evaluation by calculating the height and width of the wavefront,
 * subtracting the mean from each label, and calculating the variance of the labels.
 *
 * @param y The parameter "y" is a vector of floats representing a set of labels or target values.
 */
  void prepareForFitFunction(const std::vector<float> &y);
};


#endif //PPR_POPULATION_H
