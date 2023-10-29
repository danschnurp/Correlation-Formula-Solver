//
// Created by Dan Schnurpfeil on 24.09.2023.
//

#include "Loader.h"

std::vector<RecordHR> DataGodLoader::load_HR_data(const std::string &input_path) {
    std::ifstream file;
    file.open(input_path, std::ios::in);
    if (! file.is_open()) throw  std::runtime_error{"HR_xxx.csv not found..."};


    while (std::getline(file, line)) {
        if (line == "datetime, hr") continue;
      ss.clear();
      ss.str(line);
      // Parse the input string
      ss >> std::get_time(&timeinfo, "%Y-%m-%d %H:%M:%S");;
      ss.ignore(); // Ignore the comma (,)
      ss >> x;

      data_hr.emplace_back(timeinfo, x);
    }
  file.close();
  return data_hr;
}

std::vector<RecordACC> DataGodLoader::load_ACC_data(const std::string &input_path) {

    std::ifstream file;
    file.open(input_path, std::ios::binary);
    if (! file.is_open()) throw  std::runtime_error{"ACC_xxx.csv not found..."};

  std::getline(file, line);
    while (std::getline(file, line)) {
      ss.clear();
      ss.str(line);
      // Parse the input string

      ss >> std::get_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
      ss.ignore(); // Ignore the dot (.)
      ss >> microsecond;
      ss.ignore(); // Ignore the comma (,)
      ss >> x;
      ss.ignore(); // Ignore the comma (,)
      ss >> y;
      ss.ignore(); // Ignore the comma (,)
      ss >> z;
      data_acc.emplace_back(timeinfo, x, microsecond, y, z);
    }
  file.close();
  return data_acc;
}
