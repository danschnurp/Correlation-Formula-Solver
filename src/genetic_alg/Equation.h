//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_EQUATION_H
#define PPR_EQUATION_H

#include <vector>
#include <ostream>
#include "Node.h"

enum Operand {
    plus=0,minus=1,multiply=2,divide=3
};


enum XYZ {
    X,Y,Z
};


class Equation {
public:
    double root;
    std::vector<Node> nodes;

    Equation();

    friend std::ostream &operator<<(std::ostream &os, const Equation &equation);

    /**
     * The function evaluates an equation using the given values of x, y, and z.
     *
     * @param x The parameter `x` represents the value of the variable `x` in the equation.
     * @param y The parameter `y` represents the value of the variable `y` in the equation.
     * @param z The parameter `z` represents the value of the variable `z` in the equation. It is used in the `evaluate`
     * function to calculate the result of the equation.
     *
     * @return the result of evaluating the equation using the given values of x, y, and z.
     */
    double_t  evaluate(double_t x, double_t y, double_t z);

};


#endif //PPR_EQUATION_H
