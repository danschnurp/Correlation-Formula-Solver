//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_NODE_H
#define PPR_NODE_H

#include <string>
#include <random>

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

    Node();
};


#endif //PPR_NODE_H
