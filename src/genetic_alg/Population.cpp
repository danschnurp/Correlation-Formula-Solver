//
// Created by Dan Schnurpfeil on 06.11.2023.
//


#include "Population.h"
#include <iostream>

Population::Population() {
    for (int i = 0; i < populationSize; ++i) {
        equations.emplace_back(Equation());
    }
}

std::ostream &operator<<(std::ostream &os, const Population &population) {
    for (int i = 0; i < population.populationSize; ++i) {
        os << population.equations[i];
    }
    return os;
}

std::vector<Equation> Population::crossbreed() {
    // random immigrant
    if (equations.size() % 2 == 1) equations.emplace_back(Equation());

    std::vector<Equation> firstHalf( equations.size() / 2);
    std::copy(equations.begin(), equations.end() - equations.size() / 2, firstHalf.begin());
    std::vector<Equation> secondHalf( equations.size() / 2);
    std::copy(equations.end() - equations.size() / 2, equations.end(), secondHalf.begin());

    std::vector<Equation> children;

    std::random_device r;
    std::default_random_engine e2(r());
    for (int i = 0; i < equations.size() / 2; ++i) {
        auto result = compute_mean_std(1, firstHalf[i].nodes.size() - 1);
        std::normal_distribution<float> normal_dist(result.first, result.second);
        int firstCrossbreedingPoint = static_cast<int>(normal_dist(e2));

        result = compute_mean_std(1, secondHalf[i].nodes.size() - 1);
        std::normal_distribution<float> normal_dist2(result.first, result.second);
        int secondCrossbreedingPoint = static_cast<int>(normal_dist2(e2));

        std::vector<Node> tempNodes(firstCrossbreedingPoint);
        std::vector<Node> tempNodes2(secondCrossbreedingPoint);

        children.emplace_back(Equation());
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
