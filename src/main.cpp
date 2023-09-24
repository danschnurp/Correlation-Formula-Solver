//
// Created by Dan Schnurpfeil on 21.09.2023.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "RecordHR.h"

int main(int argc, char **argv) {
    std::ifstream file;
    file.open("../data/HR_007.csv", std::ios::in);
    std::string line;
    std::vector<RecordHR> data_hr;

    while (std::getline(file, line)) {
        if (line == "datetime, hr") continue;
        std::cout <<  line.substr(20, line.size() -1 ) << std::endl;
        std::cout <<  "" << line.substr(0, 19) << std::endl;
        data_hr.emplace_back(RecordHR(std::stod(line.substr(21, line.size() - 1)), line.substr(0, 20)));
    }
    std::cout <<  "done " << data_hr.size() << std::endl;

}