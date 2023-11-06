//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#include "utils.h"

#include <cmath>


void interpolate(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> data) {

}


double calculateMean(const std::vector<double> &data) {
    double sum = 0.0;
    for (const double &value : data) {
        sum += value;
    }
    return sum / data.size();
}


double calculateStandardDeviation(const std::vector<double> &data, double mean) {
    double sum = 0.0;
    for (const double &value : data) {
        sum += std::pow(value - mean, 2);
    }
    return std::sqrt(sum / data.size());
}

void normalize(std::vector<double> &data) {
    double mean = calculateMean(data);
    double stddev = calculateStandardDeviation(data, mean);

    for (auto &i: data) {
        i = (i - mean) / stddev;
    }

    double min_val = *std::min_element(data.begin(), data.end());
    double max_val = *std::max_element(data.begin(), data.end());
    for (double& value : data) {
        value = (value - min_val) / (max_val - min_val);
    }
}

void normalize_data(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data) {
    normalize(data.first->x);
    normalize(data.first->y);
    normalize(data.first->z);
    normalize(data.second->x);

}
