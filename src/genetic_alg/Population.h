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

  float minimumEquationCoefficients;
  float maximumEquationCoefficients;
  float maximumEquationInitLength;

  // wavefront parameters
  uint32_t width, height;

  // determining x / y / Z
  std::vector<std::vector<float>> xyzVector;

  /**
   * The function selects equations from a population based on their fitness compared to a mean result, ensuring that the
   * population does not go extinct.
   * @param mean_result The parameter `mean_result` represents the mean fitness value of the population. It is used as a
   * threshold for selecting equations in the population.
   * @param difficulty The "difficulty" parameter represents the level of difficulty or challenge in the selection process.
   * It is used to determine the threshold for selecting survivors based on their fitness compared to the mean result.
   */
  void selectMean(float mean_result, float difficulty);

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

  /**
   * The function evaluates an equation using different operations and returns the result.
   *
   * @param equation The `equation` parameter is of type `Equation`
   *
   * @return The function `evaluate` returns a `std::vector<float>` containing the evaluated values of the equation.
   */
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

  /**
   * The function computes the fitness of each equation in the population by evaluating it either on the GPU or CPU and
   * stores the fitness values in a vector.
   */
  void compute_fitness();

 public:
  int populationSize = 250;
  std::vector<float> fitness;
  std::vector<Equation> equations;
  std::vector<float> fitness_children;
  std::vector<Equation> equations_children;
  std::shared_ptr<Equation> bestOne;
  float bestCorr = 0;

  /**
   * The function initializes a population of equations using given data and parameters, and optionally sets up GPU
   * computation units.
   *
   * @param data
   * @param useGPU A boolean value indicating whether to use GPU for computation or not.
   * @param WORKGROUP_SIZE WORKGROUP_SIZE is the number of work-items in a work-group. It determines the number of parallel
   * computations that can be performed simultaneously on a GPU.
   * @param minimumEquationCoefficientsp The parameter "minimumEquationCoefficientsp" represents the minimum value for the
   * coefficients of the equations in the population.
   * @param maximumEquationCoefficientsp The parameter "maximumEquationCoefficientsp" represents the maximum value that can
   * be assigned to the coefficients of the equations in the population.
   * @param maximumEquationInitLengthp The parameter "maximumEquationInitLengthp" represents the maximum initial length of an
   * equation in the population. It is used to determine the maximum number of coefficients that can be randomly generated
   * for each equation during initialization.
   */
  Population(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data, bool useGpu, int WORKGROUP_SIZE,
             float minimumEquationCoefficients, float maximumEquationCoefficients,
             float maximumEquationInitLength);

  /**
   * The function creates a new generation of equations by selecting the fittest equations from the
   * current generation, crossbreeding them, and completing the population with randomly generated equations if necessary.
   *
   * @param wave parameter which makes selection more difficult
   */
  void create_one_generation(float wave);

  friend std::ostream &operator<<(std::ostream &os, const Population &population);

/**
 * The function prepares the population for fitness evaluation by calculating the height and width of the wavefront,
 * subtracting the mean from each label, and calculating the variance of the labels.
 *
 * @param y The parameter "y" is a vector of floats representing a set of labels or target values.
 */
  void prepareForFitFunction(const std::vector<float> &y, int WORKGROUP_SIZE);
};


#endif //PPR_POPULATION_H
