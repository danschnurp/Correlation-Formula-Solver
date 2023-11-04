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

std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> load_data(
    std::string &hr_filename, std::string &acc_filename, bool load_sequential);

class DataGodLoader {

private:
    std::string line;
  int microsecond;
    double x, y, z;

    std::istringstream ss;
    std::tm timeinfo{};

public:

  std::shared_ptr<RecordHR> load_HR_data(const std::string &input_path);
  std::shared_ptr<RecordACC> load_ACC_data(const std::string &input_path);

};

#endif //PPRKA_SVG_LOADER_H
