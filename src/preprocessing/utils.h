//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_UTILS_H
#define PPR_UTILS_H
#include "../io/RecordHR.h"
#include "../io/RecordACC.h"
#include <vector>
#include <chrono>




/**
 * The function "interpolate" takes in a pair of shared pointers to RecordACC and RecordHR objects, and interpolates the
 * values in the RecordHR object based on the timestamps in the RecordACC object.
 */
void interpolate(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data);

/**
 * The function `remove_redundant` removes redundant data from `RecordACC`
 * object.
 */
void remove_redundant(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data);

/**
 * The normalize function takes a vector of floats, calculates the mean and standard deviation, and then normalizes the
 * data by subtracting the mean and dividing by the standard deviation, and then scales the data to a range of 0 to 1.
 * for ACC: x, y, z and HR: x
 */
void normalize(std::vector<float> &data);



#endif //PPR_UTILS_H
