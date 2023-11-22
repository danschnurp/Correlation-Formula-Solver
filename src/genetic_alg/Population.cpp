//
// Created by Dan Schnurpfeil on 06.11.2023.
//


#include "Population.h"
#include <iostream>

Population::Population() {
    for (int i = 0; i < populationSize; ++i) {
        equations.emplace_back(Equation());
    }
    fPlus = std::make_unique<ComputationUnit>("../plus.spv");
    fMinus = std::make_unique<ComputationUnit>("../minus.spv");
    fMultiply = std::make_unique<ComputationUnit>("../multiply.spv");
    fDivide = std::make_unique<ComputationUnit>("../divide.spv");
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

        std::uniform_int_distribution<int> uni_dist(0, firstHalf[i].nodes.size() - 1);
        int firstCrossbreedingPoint = static_cast<int>(uni_dist(e2));

        std::uniform_int_distribution<int> uni_dist2(0, firstHalf[i].nodes.size() - 1);
        int secondCrossbreedingPoint = static_cast<int>(uni_dist2(e2));

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
    return covXY / (std::sqrt(varX) * varLabels);
}

void Population::prepareForFitFunction(const std::vector<float> &y) {

    // making wavefront
    height = 16;
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

std::vector<float> Population::evaluateCPU(const std::vector<float>& x, const std::vector<float>& y,
                                        const std::vector<float>& z, int equationIndex) {
    std::vector<float> equation_results;
    for (int j = 0; j < x.size(); ++j) {
        equation_results.emplace_back(equations[equationIndex].evaluate(x[j], y[j], z[j]));
    }
    return equation_results;

}

std::vector<float> Population::evaluate(const std::vector<float>& x, const std::vector<float>& y,
                                      const std::vector<float>& z, int equationIndex) {
    std::vector<std::vector<float>> xyzVector;
    xyzVector.push_back(x);
    xyzVector.push_back(y);
    xyzVector.push_back(z);
    auto out_tst = std::vector<float>(x.size(), equations[equationIndex].root);


    for (const auto &i: equations[equationIndex].nodes) {
        auto variable = xyzVector[i.xyz];
        if (i.operand == 0) {
            auto d_y = vuh::Array<float>::fromHost(out_tst, fPlus->device, fPlus->physDevice);
            auto d_x = vuh::Array<float>::fromHost(variable, fPlus->device, fPlus->physDevice);
            fPlus->compute(d_y, d_x, {width, height, i.value});
            d_y.to_host(out_tst);
        } else if (i.operand == 1) {
            auto d_y = vuh::Array<float>::fromHost(out_tst, fMinus->device, fMinus->physDevice);
            auto d_x = vuh::Array<float>::fromHost(variable, fMinus->device, fMinus->physDevice);
            fMinus->compute(d_y, d_x, {width, height, i.value});
            d_y.to_host(out_tst);
        } else if (i.operand == 2) {
            auto d_y = vuh::Array<float>::fromHost(out_tst, fMultiply->device, fMultiply->physDevice);
            auto d_x = vuh::Array<float>::fromHost(variable, fMultiply->device, fMultiply->physDevice);
            fMultiply->compute(d_y, d_x, {width, height, i.value});
            d_y.to_host(out_tst);
        } else if (i.operand == 3) {
            auto d_y = vuh::Array<float>::fromHost(out_tst, fDivide->device, fDivide->physDevice);
            auto d_x = vuh::Array<float>::fromHost(variable, fDivide->device, fDivide->physDevice);
            fDivide->compute(d_y, d_x, {width, height, i.value});
            d_y.to_host(out_tst);
        }

    }
    return out_tst;
}