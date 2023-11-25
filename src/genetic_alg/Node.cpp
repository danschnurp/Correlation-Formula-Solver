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

  std::random_device r;
    std::default_random_engine e2(r());


    std::uniform_int_distribution<int> dist(0, 2);
    operand = static_cast<int>(dist(e2));
    // coefficients are in interval -5, 5
    auto result = compute_mean_std(-4.0, 4.0);
    std::normal_distribution<float> normal_dist_values(result.first, result.second);
    value = normal_dist_values(e2);


    // random 0 and 2 (inclusive) and uses unif distribution for X / Y / Z
    std::uniform_int_distribution<int> dist_xyz(0, 2);
    xyz = static_cast<int>(dist_xyz(e2));

    }



