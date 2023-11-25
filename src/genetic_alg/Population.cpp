//
// Created by Dan Schnurpfeil on 06.11.2023.
//


#include "Population.h"
#include <iostream>
#include "../gpu/demo/vulkan_helpers.hpp"

void print_time(auto start_time) {
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration =
      static_cast<time_t>(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
          / 1000.0);
  auto duration_t = localtime(&duration);
  std::cout << "Time taken: " << duration_t->tm_min << " min " << duration_t->tm_sec << " sec" << std::endl
            << std::endl;
}

Population::Population(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data) {
  for (int i = 0; i < populationSize; ++i) {
    equations_children.emplace_back(Equation());
  }

  data = data;

  xyzVector.push_back(data.first->x);
  xyzVector.push_back(data.first->y);
  xyzVector.push_back(data.first->z);

  fPlus = std::make_unique<ComputationUnit>("../plus.spv", false);
  fMinus = std::make_unique<ComputationUnit>("../minus.spv", false);
  fMultiply = std::make_unique<ComputationUnit>("../multiply.spv", false);
}

void Population::compute_fitness() {
  std::cout << "starting correlation compute... " << std::endl;
  auto start_time = std::chrono::high_resolution_clock::now();
  try {
    for (Equation &item : equations_children) {
//          fitness_children.emplace_back(
//          countFitFunction(evaluateCPU(data.first->x, data.first->y, data.first->z, item)));
      fitness_children.emplace_back(
          countFitFunction(evaluate(item)));
    }
  }
  catch (...) {
    std::cout << "..." << std::endl;
    throw std::runtime_error("...");
  }
  print_time(start_time);
}

void Population::create_one_generation(int wave) {
  compute_fitness();
  std::copy(fitness_children.begin(), fitness_children.end(), std::back_inserter(fitness));
  std::copy(equations_children.begin(), equations_children.end(), std::back_inserter(equations));
  fitness_children.clear();
  equations_children.clear();
  float mean_result = mean(fitness);
  // selection based on mean like result
  for (int j = 0; j < equations.size(); ++j) {
    if (fitness[j] <= (mean_result + mean_result / 10 * (1 + wave))) {
      equations[j].root = std::numeric_limits<float>::max();
      fitness[j] = std::numeric_limits<float>::max();
    }
  }
  std::erase_if(equations, [](Equation &value) { return value.root == std::numeric_limits<float>::max(); });
  std::erase_if(fitness, [](float &value) { return value == std::numeric_limits<float>::max(); });
  // print stats
  auto index_max = std::max_element(fitness.begin(), fitness.end());
  int index_m = std::distance(fitness.begin(), index_max);
  std::cout << "max fitness: " << fitness[index_m] << std::endl;
  std::cout << "best local equation: " << equations[index_m] << std::endl;
  std::cout << "parents population size: " << equations.size() << std::endl;
  std::cout << "average population fitness after selection: " << mean(fitness) << std::endl;
  // crossbreeding
  auto start_time = std::chrono::high_resolution_clock::now();
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(equations.begin(), equations.end(), g);
  std::cout << "started crossbreeding... " << std::endl;
  std::vector<Equation> children = crossbreed();
  std::cout << "children: " << children.size() << std::endl;
  // completes population
  if (populationSize - equations.size() < children.size()) {
    for (int j = 0; j < populationSize - equations.size(); ++j) {
      equations_children.push_back(children[j]);
    }
  } else {
    for (auto &item : children) equations_children.push_back(item);
    for (int j = 0; j < populationSize - equations_children.size(); ++j) {
      equations_children.emplace_back();
    }
  }
  print_time(start_time);
}

std::ostream &operator<<(std::ostream &os, const Population &population) {
  for (int i = 0; i < population.populationSize; ++i) {
    os << population.equations[i];
  }
  return os;
}

std::vector<Equation> Population::crossbreed() {
  // lucky one fucks twice
  if (equations.size() % 2 == 1) {
    equations.push_back(equations[equations.size() - 1]);
    fitness.push_back(fitness[fitness.size() - 1]);
  }
  std::vector<Equation> firstHalf(equations.size() / 2);
  std::copy(equations.begin(), equations.end() - equations.size() / 2, firstHalf.begin());
  std::vector<Equation> secondHalf(equations.size() / 2);
  std::copy(equations.end() - equations.size() / 2, equations.end(), secondHalf.begin());
  std::vector<Equation> children;

  std::random_device r;
  std::default_random_engine e2(r());
  for (int i = 0; i < equations.size() / 2; ++i) {
    // random crossover points
        std::uniform_int_distribution<int> uni_dist(0, firstHalf[i].nodes.size() - 1);
        int firstCrossbreedingPoint = static_cast<int>(uni_dist(e2));
        std::uniform_int_distribution<int> uni_dist2(0, firstHalf[i].nodes.size() - 1);
        int secondCrossbreedingPoint = static_cast<int>(uni_dist2(e2));
      // new chromosomes
        std::vector<Node> tempNodes(firstCrossbreedingPoint);
        std::vector<Node> tempNodes2(secondCrossbreedingPoint);

        children.emplace_back();
        children[children.size()-1].nodes.clear();
        for (auto &item: tempNodes) {
            children[children.size() -1].nodes.push_back(item);
        }
        for (auto &item: tempNodes2) {
            children[children.size()-1].nodes.push_back(item);
        }
    }
    return children;
}

float mean(const std::vector<float> &data) {
    float sum = 0.0;
    for (const float &value : data) {
        sum += value;
    }
    return sum / data.size();
}

float Population::countFitFunction(const std::vector<float> &x) {
    if (x.size() != precomputedLabels.size()) {
        throw std::invalid_argument("Input vectors must have the same size");
    }
    // Calculate means
    float meanX = mean(x);
    // Calculate covariance and variances
    float covXY = 0.0;
    float varX = 0.0;

    for (size_t i = 0; i < x.size(); ++i) {
        covXY += (x[i] - meanX) * precomputedLabels[i];
        varX += std::pow(x[i] - meanX, 2);
    }
    // Calculate Pearson correlation coefficient
    return std::abs(covXY / (std::sqrt(varX) * varLabels));
}

void Population::prepareForFitFunction(const std::vector<float> &y) {
    // making wavefront
    height = 32;
    while (y.size() % height > 0) height--;
    width = y.size() / height;

    float meanLabels = mean(y);
    for (const auto &item: y) {
        precomputedLabels.emplace_back(item - meanLabels);
    }
    for (const auto &item: y) {
        varLabels += (item - meanLabels) * (item - meanLabels);
    }
    varLabels = std::sqrt(varLabels);
}

std::vector<float> Population::evaluateCPU(const std::vector<float> &x, const std::vector<float> &y,
                                           const std::vector<float> &z, Equation &equation) {
  std::vector<float> equation_results;
  for (int j = 0; j < x.size(); ++j) {
    equation_results.emplace_back(equation.evaluate(x[j], y[j], z[j]));
  }
  return equation_results;
}

std::vector<float> Population::evaluate(const Equation &equation) {

  auto out_tst = std::vector<float>(xyzVector[0].size(), equation.root);
  // due to large memory consumption (9x - (+-*) and (xyz)) d_x is not precomputed
  for (const auto &i : equation.nodes) {
    if (i.operand == 0) {
      auto d_y = vuh::Array<float>::fromHost(out_tst, fPlus->device, fPlus->physDevice);
      auto d_x = vuh::Array<float>::fromHost(xyzVector[i.xyz], fPlus->device, fPlus->physDevice);
      fPlus->compute(d_y, d_x, {width, height, i.value});
      d_y.to_host(out_tst);
    } else if (i.operand == 1) {
      auto d_y = vuh::Array<float>::fromHost(out_tst, fMinus->device, fMinus->physDevice);
      auto d_x = vuh::Array<float>::fromHost(xyzVector[i.xyz], fMinus->device, fMinus->physDevice);
      fMinus->compute(d_y, d_x, {width, height, i.value});
      d_y.to_host(out_tst);
    } else if (i.operand == 2) {
      auto d_y = vuh::Array<float>::fromHost(out_tst, fMultiply->device, fMultiply->physDevice);
      auto d_x = vuh::Array<float>::fromHost(xyzVector[i.xyz], fMultiply->device, fMultiply->physDevice);
      fMultiply->compute(d_y, d_x, {width, height, i.value});
      d_y.to_host(out_tst);
    }
    }
    return out_tst;
}