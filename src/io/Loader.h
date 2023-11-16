//
// Created by Dan Schnurpfeil on 24.09.2023.
//

#ifndef PPRKA_SVG_LOADER_H
#define PPRKA_SVG_LOADER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include "RecordHR.h"
#include "RecordACC.h"

/**
 * The function `load_data` loads HR and ACC data either sequentially or in parallel and returns a pair of shared pointers
 * to the loaded data.
 *
 * @param hr_filename The `hr_filename` parameter is a string that represents the filename of the HR (Heart Rate) data file
 * that needs to be loaded.
 * @param acc_filename The `acc_filename` parameter is a string that represents the filename of the ACC data file.
 * @param load_sequential A boolean flag indicating whether the data should be loaded sequentially or in parallel.
 *
 * @return The function `load_data` returns a `std::pair` containing two `std::shared_ptr` objects. The first element of
 * the pair is a `std::shared_ptr<RecordACC>` object named `data_acc`, and the second element is a
 * `std::shared_ptr<RecordHR>` object named `data_hr`.
 */
std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> load_data(
    std::string &hr_filename, std::string &acc_filename, bool load_sequential);

void save_cleared_ACC_data(const std::string &output_path, const std::shared_ptr<RecordACC> &data);

class DataGodLoader {
private:
    std::string line;
  int microsecond;
    float x, y, z;

    std::istringstream ss;
    std::tm timeinfo{};

public:
  /**
   * The function `load_HR_data` reads data from a binary file, parses it, and returns a shared pointer to a `RecordHR`
   * object containing the parsed data.
   *
   * @param input_path The input_path parameter is a string that represents the file path of the input file that contains the
   * HR data.
   *
   * @return A shared pointer to an object of type `RecordHR` is being returned.
   */
  std::shared_ptr<RecordHR> load_HR_data(const std::string &input_path);
  /**
   * The function `load_ACC_data` reads data from a binary file and parses it into a shared pointer to a `RecordACC` object.
   *
   * @param input_path The input_path parameter is a string that represents the file path of the ACC_xxx.csv file that needs
   * to be loaded.
   *
   * @return The function `load_ACC_data` returns a `std::shared_ptr` to an object of type `RecordACC`.
   */
  std::shared_ptr<RecordACC> load_ACC_data(const std::string &input_path);

};

#endif //PPRKA_SVG_LOADER_H
