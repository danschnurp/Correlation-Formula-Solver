//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_UTILS_H
#define PPR_UTILS_H
#include "../io/RecordHR.h"
#include "../io/RecordACC.h"
#include <vector>

void remove_redundant(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data);

/**
 * The normalize function takes a vector of doubles, calculates the mean and standard deviation, and then normalizes the
 * data by subtracting the mean and dividing by the standard deviation, and then scales the data to a range of 0 to 1.
 * for ACC: x, y, z and HR: x
 */
void normalize_data(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data);



#endif //PPR_UTILS_H
