//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#include "Equation.h"

Equation::Equation() {

    std::random_device r;
    std::default_random_engine e2(r());

    // coefficients are in interval -2, 2
    auto result = compute_mean_std(-2.0, 2.0);
    std::normal_distribution<double> normal_dist_values(result.first, result.second);
    root = normal_dist_values(e2);

    // length of equation is 3 - 7
    result = compute_mean_std(3.0, 7);
    std::normal_distribution<double> normal_dist_len(result.first, result.second);
    for (int i = 0; i < static_cast<int>(round(normal_dist_len(e2))); ++i) {
        nodes.emplace_back();
    }
}

double_t determine_xyz(int xyz, double_t x, double_t y, double_t z) {
    switch (xyz) {
        case X:
            return x;
        case Y:
            return y;
        case Z:
            return z;
    }
    return -10000.0;
}


double_t Equation::evaluate(double_t x, double_t y, double_t z) {
    double_t result = root;
    double_t xyz;
    for (const auto &i: nodes) {
        switch (i.operand) {
            case plus:
                xyz = determine_xyz(i.xyz, x,y,z);
                result += i.value * xyz;
                break;
            case minus:
                xyz = determine_xyz(i.xyz, x,y,z);
                result -= i.value * xyz;
                break;
            case multiply:
                xyz = determine_xyz(i.xyz, x,y,z);
                result *= i.value * xyz;
                break;
            case divide:
                xyz = determine_xyz(i.xyz, x,y,z);
                result /= i.value * xyz;
                break;
        }
    }

    return result;
}

std::ostream &operator<<(std::ostream &os, const Equation &equation) {
    os << "equation: " << equation.root;
    for (const auto &i: equation.nodes) {
        switch (i.operand) {
            case plus:
                os << " " << "+";break;
            case minus:
                os << " " << "-";break;
            case multiply:
                os << " " << "*";break;
            case divide:
                os << " " << "/";break;
            default:
                os << " " << "None";break;
        }
        os << " (" << i.value;
        switch (i.xyz) {
            case X:
                os << " X )";break;
            case Y:
                os << " Y )";break;
            case Z:
                os << " Z )";break;
            default:
                os << " " << "None";break;
        }
    }
    os << std::endl;
    return os;
}
