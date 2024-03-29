//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#ifndef PPR_UTILS_H
#define PPR_UTILS_H
#include "../io/Loader.h"
#include "../io/RecordHR.h"
#include "../io/RecordACC.h"
#include <vector>
#include <chrono>

/**
 * The function removes invalid data points from the `data` pair by checking and erasing elements based
 * on certain conditions.
 */
void reduce_to_seconds(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data);

/**
 * The preprocess function normalizes and processes accelerometer and heart rate data, removes redundant data, and saves
 * the cleared accelerometer data.
 *
 * @param acc_filename The `acc_filename` parameter is a reference to a string that represents the filename of the
 * accelerometer data file.
 */
void preprocess(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data,
                std::string &acc_filename);

/**
 * The function "interpolate" takes in a pair of shared pointers to RecordACC and RecordHR objects, and interpolates the
 * values in the RecordHR object based on the timestamps in the RecordACC object.
 */
void interpolate(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data);

/**
 * removes redundant data from a pair of records based on their timestamps.
 */
void remove_redundant_acc(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data);

/**
 * removes redundant data from a pair of records based on their timestamps.
 */
void remove_redundant_hr(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data);

/**
 * The function removes redundant data from either the ACC or HR record based on which one has more timestamps.
 */
void remove_redundant(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data);

/**
 * The normalize function takes a vector of floats, calculates the mean and standard deviation, and then normalizes the
 * data by subtracting the mean and dividing by the standard deviation, and then scales the data to a range of 0 to 1.
 * for ACC: x, y, z and HR: x
 */
void normalize(std::vector<float> &data);



#endif //PPR_UTILS_H
