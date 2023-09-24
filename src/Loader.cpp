//
// Created by Dan Schnurpfeil on 24.09.2023.
//

#include "Loader.h"


std::vector<RecordHR> Loader::load_HR_data(const std::string &input_path) {
    std::ifstream file;
    file.open(input_path, std::ios::in);
    if (! file.is_open()) throw  std::runtime_error{"HR_xxx.csv not found..."};
    std::vector<RecordHR> data_hr;
    while (std::getline(file, line)) {
        if (line == "datetime, hr") continue;
        parse_HR_line();
        data_hr.emplace_back(timestamp, x);
    }
    file.close();
    return data_hr;
}

void Loader::parse_HR_line() {

    ss.clear();
    ss.str(line);
    // Parse the input string
    ss >> year;
    ss.ignore(); // Ignore the dash (-)
    ss >> month;
    ss.ignore(); // Ignore the dash (-)
    ss >> day;
    ss.ignore(); // Ignore the space ( )
    ss >> hour;
    ss.ignore(); // Ignore the colon (:)
    ss >> minute;
    ss.ignore(); // Ignore the colon (:)
    ss >> second;
    ss.ignore(); // Ignore the comma (,)
    ss >> x;
    timeInfo.tm_year = year - 1900; // Years since 1900
    timeInfo.tm_mon = month - 1;    // Months are 0-based
    timeInfo.tm_mday = day;
    timeInfo.tm_hour = hour;
    timeInfo.tm_min = minute;
    timeInfo.tm_sec = second;
    timestamp = mktime(&timeInfo);

}

std::vector<RecordACC> Loader::load_ACC_data(const std::string &input_path) {
    std::ifstream file;
    file.open(input_path, std::ios::in);
    if (! file.is_open()) throw  std::runtime_error{"HR_xxx.csv not found..."};
    std::vector<RecordACC> data_acc;
    while (std::getline(file, line)) {
        if (line == "datetime, hr") continue;
        parse_ACC_line();
        data_acc.emplace_back(timestamp, x, microsecond, y, z);
    }
    file.close();
    return data_acc;
}

void Loader::parse_ACC_line() {

    ss.clear();
    ss.str(line);
    // Parse the input string
    ss >> year;
    ss.ignore(); // Ignore the dash (-)
    ss >> month;
    ss.ignore(); // Ignore the dash (-)
    ss >> day;
    ss.ignore(); // Ignore the space ( )
    ss >> hour;
    ss.ignore(); // Ignore the colon (:)
    ss >> minute;
    ss.ignore(); // Ignore the colon (:)
    ss >> second;
    ss.ignore(); // Ignore the dot (.)
    ss >> microsecond;
    ss.ignore(); // Ignore the comma (,)
    ss >> x;
    timeInfo.tm_year = year - 1900; // Years since 1900
    timeInfo.tm_mon = month - 1;    // Months are 0-based
    timeInfo.tm_mday = day;
    timeInfo.tm_hour = hour;
    timeInfo.tm_min = minute;
    timeInfo.tm_sec = second;
    timestamp = mktime(&timeInfo);
    ss.ignore(); // Ignore the comma (,)
    ss >> y;
    ss.ignore(); // Ignore the comma (,)
    ss >> z;

}
