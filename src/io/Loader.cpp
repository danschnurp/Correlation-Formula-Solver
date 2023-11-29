//
// Created by Dan Schnurpfeil on 24.09.2023.
//

#include "Loader.h"
#include <thread>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <charconv>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <filesystem>

std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> load_data(
    std::string &hr_filename, std::string &acc_filename, bool load_sequential) {
  std::cout << "load sequentially: " << load_sequential << std::endl;

  auto loader_hr = std::make_unique<DataGodLoader>();

  auto loader_acc = std::make_unique<DataGodLoader>();
  std::shared_ptr<RecordACC> data_acc;
  std::shared_ptr<RecordHR> data_hr;
  if (load_sequential) {
    data_hr = loader_hr->load_HR_data(hr_filename);
    data_acc = loader_acc->load_ACC_data(acc_filename);
  } else {
    // `hr_thread_loader` and `acc_thread_loader`, load data from two different files concurrently.
    auto hr_thread_loader = std::thread([&hr_filename, &data_hr, &loader_hr]() {
      try {
        data_hr = loader_hr->load_HR_data(hr_filename);
      }
      catch (std::exception &err) {
        std::cerr << std::endl << err.what() << std::endl;
      }
    });
    auto acc_thread_loader = std::thread([&acc_filename, &data_acc, &loader_acc]() {
      try {
        data_acc = loader_acc->load_ACC_data(acc_filename);
      }
      catch (std::exception &err) {
        std::cerr << std::endl << err.what() << std::endl;
      }
    });

    if (hr_thread_loader.joinable()) {
      hr_thread_loader.join();
    }
    if (acc_thread_loader.joinable()) {
      acc_thread_loader.join();
    }
  }

  if (data_acc->x.empty()) throw std::invalid_argument("malformed ACC_data...");
  if (data_hr->x.empty()) throw std::invalid_argument("malformed HR_data...");

  return std::make_pair(data_acc, data_hr);
}

std::shared_ptr<RecordHR> DataGodLoader::load_HR_data(const std::string &input_path) {
  std::ifstream file;
  file.open(input_path, std::ios::binary);
  if (!file.is_open()) throw std::runtime_error{"ACC_xxx.csv not found..."};
  auto data = std::make_shared<RecordHR>();
  std::getline(file, line);
  while (std::getline(file, line)) {
    ss.clear();
    ss.str(line);
    // Parse the input string
    ss >> std::get_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
      throw std::invalid_argument("malformed HR_data...");
    }
    ss.ignore(); // Ignore the comma (,)
    ss >> x;
    timeinfo.tm_year += 1900;
    timeinfo.tm_mon += 1;
    data->timestamp.emplace_back(timeinfo);
    data->x.emplace_back(x);
  }
  file.close();
  return data;
}

std::shared_ptr<RecordACC> DataGodLoader::load_ACC_data(const std::string &input_path) {
  std::ios::sync_with_stdio(false);
  std::ifstream file;
  file.open(input_path, std::ios::binary);
  if (!file.is_open()) throw std::runtime_error{"ACC_xxx.csv not found..."};
  auto data = std::make_shared<RecordACC>();

  // Get the file size
  std::size_t size = std::filesystem::file_size(input_path);
  // Map the file to memory
  std::vector<char> raw_data(size);
  file.read(raw_data.data(), size);

  // Tokenize lines and parse data
  std::string_view dataView(raw_data.data(), size);
  std::size_t pos = 0;

  // Skip the header
  std::size_t headerEnd = dataView.find('\n');
  if (headerEnd != std::string::npos) {
    pos = headerEnd;
  }

  while (pos != std::string::npos) {
    // Find the end of the line
    std::size_t end = dataView.find("\r\n", pos);
    if (end == std::string::npos) {
      end = size;
    }
    if (end == size) break;

    // Extract the line
    std::string line(dataView.substr(pos + 1, end - pos - 1));

    ss.clear();
    ss.str(line);
    // Parse the input string
    ss >> std::get_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
      throw std::invalid_argument("malformed ACC_data...");
    }
    ss.ignore(); // Ignore the dot (.)
    ss >> microsecond;
    if (microsecond == 0.0) {
      ss.ignore(); // Ignore the comma (,)
      ss >> x;
      ss.ignore(); // Ignore the comma (,)
      ss >> y;
      ss.ignore(); // Ignore the comma (,)
      ss >> z;
      data->timestamp.emplace_back(timeinfo);
      data->microsecond.emplace_back(microsecond);
      data->x.emplace_back(x);
      data->y.emplace_back(y);
      data->z.emplace_back(z);
    }
    // Move to the next line
    pos = (end == size) ? std::string::npos : end + 1;
  }
  file.close();
  return data;
}

void save_cleared_ACC_data(const std::string &output_path, const std::shared_ptr<RecordACC> &data) {
  std::ofstream file;
  file.open(output_path, std::ios::binary);
  if (!file.is_open()) throw std::runtime_error{"ACC_xxx.csv not found..."};
  file << "datetime, acc_x, acc_y, acc_z" << std::endl;
  for (int i = 0; i < data->x.size(); ++i) {
    file << data->timestamp[i].tm_year;
    file << "-";
    file << data->timestamp[i].tm_mon;
    file << "-";
    file << data->timestamp[i].tm_mday;
    file << " ";
    file << data->timestamp[i].tm_hour;
    file << ":";
    file << data->timestamp[i].tm_min;
    file << ":";
    file << data->timestamp[i].tm_sec;
    file << ".";
    file << data->microsecond[i];
    file << ",";
    file << data->x[i];
    file << ",";
    file << data->y[i];
    file << ",";
    file << data->z[i];
    file << std::endl;
  }

  file.close();

//  std::cout << "saved "

}
