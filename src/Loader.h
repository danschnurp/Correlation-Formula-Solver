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

class DataGodLoader {

private:
    std::vector<RecordACC> data_acc;
    std::string line;
    int second, microsecond;
    double x, y, z;

    std::istringstream ss;
    std::tm timeinfo{};
    std::vector<RecordHR> data_hr;

public:

std::vector<RecordHR> load_HR_data(const std::string& input_path);
  std::vector<RecordACC> load_ACC_data(const std::string& input_path);

};

#endif //PPRKA_SVG_LOADER_H
