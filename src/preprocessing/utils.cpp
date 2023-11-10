//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#include "utils.h"

#include <cmath>


void remove_redundant(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data) {

    int hr_counter = 0;

    std::tm max_tm1 = {};
    // Set the maximum representable values for some members
    max_tm1.tm_sec = std::numeric_limits<int>::max();
    max_tm1.tm_min = std::numeric_limits<int>::max();
    max_tm1.tm_hour = std::numeric_limits<int>::max();
    max_tm1.tm_mday = std::numeric_limits<int>::max();
    max_tm1.tm_mon = std::numeric_limits<int>::max();
    max_tm1.tm_year = std::numeric_limits<int>::max();

    for (int i = 0; i < data.first->timestamp.size() - 1; ++i) {
        if (data.first->timestamp[i].tm_gmtoff == data.second->timestamp[hr_counter].tm_gmtoff) {
            hr_counter++;

        } else {
            std::tm max_tm = {};
            // Set the maximum representable values for some members
            max_tm.tm_sec = std::numeric_limits<int>::max();
            max_tm.tm_min = std::numeric_limits<int>::max();
            max_tm.tm_hour = std::numeric_limits<int>::max();
            max_tm.tm_mday = std::numeric_limits<int>::max();
            max_tm.tm_mon = std::numeric_limits<int>::max();
            max_tm.tm_year = std::numeric_limits<int>::max();
            data.first->timestamp[i] = max_tm;
            data.first->x[i] = std::numeric_limits<double>::max();
            data.first->y[i] = std::numeric_limits<double>::max();
            data.first->z[i] = std::numeric_limits<double>::max();
            data.first->microsecond[i] = std::numeric_limits<double>::max();
       }
    }

    std::erase_if(data.first->timestamp, [&](tm value) {
        return value.tm_gmtoff == max_tm1.tm_gmtoff;});
    std::erase_if(data.first->x, [&](double value) {
        return value == std::numeric_limits<double>::max();});
    std::erase_if(data.first->y, [&](double value) {
        return value == std::numeric_limits<double>::max();});
    std::erase_if(data.first->z, [&](double value) {
        return value == std::numeric_limits<double>::max();});
    std::erase_if(data.first->microsecond, [&](double value) {
        return value == std::numeric_limits<double>::max();});

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
