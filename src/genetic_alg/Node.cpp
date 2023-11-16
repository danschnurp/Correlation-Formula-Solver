//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#include "Node.h"


std::pair<float, float> compute_mean_std(float interval_start, float interval_end) {

    float mean = (interval_start + interval_end) / 2.0;
    float range = interval_end - interval_start;
    float std_deviation = range / 4.0;
    return std::make_pair(mean, std_deviation);
}

Node::Node() {
    // TRNG device
    std::random_device r;
    std::default_random_engine e2(r());

    auto result = compute_mean_std(0.0, 3.0);
    std::normal_distribution<float> normal_dist(result.first, result.second);
    operand = static_cast<int>(normal_dist(e2));
    // coefficients are in interval -2, 2
    result = compute_mean_std(-2.0, 2.0);
    std::normal_distribution<float> normal_dist_values(result.first, result.second);
    value = normal_dist_values(e2);

    result = compute_mean_std(0.0, 2.0);
    std::normal_distribution<float> normal_dist_xyz(result.first, result.second);
    xyz = static_cast<int>(normal_dist_xyz(e2));

    }



