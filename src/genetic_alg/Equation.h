//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_EQUATION_H
#define PPR_EQUATION_H

#include <vector>
#include <ostream>
#include "Node.h"

enum Operand {
    plus=0,minus=1,multiply=2
};


enum XYZ {
    X,Y,Z
};


class Equation {
public:
    float root;
    std::vector<Node> nodes;

    Equation(float minimumEquationCoefficients, float maximumEquationCoefficients, float maximumEquationInitLength);

    friend std::ostream &operator<<(std::ostream &os, const Equation &equation);

    bool operator==(const Equation &rhs) const;

    bool operator!=(const Equation &rhs) const;
  /**
    * The function evaluates an equation using the given values of x, y, and z.
    *
    * @param x The parameter "x" represents the value of the variable "x" in the equation.
    * @param y The parameter "y" in the given code represents the value of the variable "y" in the equation being evaluated.
    * @param z The parameter "z" in the given code represents a variable that can be used in the equation evaluation. It is a
    * float_t type variable, which means it can hold decimal values.
    *
    * @return the result of evaluating the equation using the given values of x, y, and z.
    */
    float_t  evaluate(float_t x, float_t y, float_t z);

};


#endif //PPR_EQUATION_H
