//
// Created by Dan Schnurpfeil on 24.09.2023.
//

#ifndef PPRKA_SVG_LOADER_H
#define PPRKA_SVG_LOADER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "RecordHR.h"
#include "RecordACC.h"


class Loader {
public:
    std::vector<RecordHR> load_HR_data(const std::string& input_path);
    std::vector<RecordACC> load_ACC_data(const std::string& input_path);
private:
    std::string line;
    int year, month, day, hour, minute, second, microsecond;
    double x, y, z;
    time_t timestamp;
    std::istringstream ss;
    std::tm timeInfo{};

    void parse_HR_line();
    void parse_ACC_line();



};


#endif //PPRKA_SVG_LOADER_H