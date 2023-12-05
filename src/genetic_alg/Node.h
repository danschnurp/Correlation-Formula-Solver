//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_NODE_H
#define PPR_NODE_H

#include <string>
#include <random>

/**
 * The function computes the mean and standard deviation of a given interval.
 *
 * @param interval_start The starting value of the interval.
 * @param interval_end The parameter "interval_end" represents the end value of the interval or range.
 *
 * @return a pair of floats, where the first element is the mean value and the second element is the standard deviation.
 */
std::pair<float, float> compute_mean_std(float interval_start, float interval_end);

/**
 * The Node constructor initializes the operand with a random integer and the value with a random float using a normal
 * distribution.
 */
class Node {
public:
    float value;
    int operand;
    int xyz;

    Node(float minimumEquationCoefficients, float maximumEquationCoefficients);
};


#endif //PPR_NODE_H
