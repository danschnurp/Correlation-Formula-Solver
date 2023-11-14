//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_NODE_H
#define PPR_NODE_H

#include <string>
#include <random>

std::pair<double, double> compute_mean_std(double interval_start, double interval_end);

/**
 * The Node constructor initializes the operand with a random integer and the value with a random double using a normal
 * distribution.
 */
class Node {
public:
    double value;
    int operand;
    int xyz;

    Node();
};


#endif //PPR_NODE_H
