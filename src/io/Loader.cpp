//
// Created by Dan Schnurpfeil on 24.09.2023.
//

#include "Loader.h"
#include <thread>



std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> load_data(
    std::string &hr_filename, std::string &acc_filename, bool load_sequential) {
  std::cout << "load sequentially: " << load_sequential << std::endl;

  auto loader_hr = std::make_unique<DataGodLoader>();

  auto loader_acc = std::make_unique<DataGodLoader>();
  std::shared_ptr<RecordACC> data_acc;
  std::shared_ptr<RecordHR> data_hr;
  if (load_sequential) {
    data_acc = loader_acc->load_ACC_data(acc_filename);
    data_hr = loader_hr->load_HR_data(hr_filename);
  } else {
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
    ss.ignore(); // Ignore the comma (,)
    ss >> x;
    data->timestamp.emplace_back(timeinfo);
    data->x.emplace_back(x);
  }
  file.close();
  return data;
}

std::shared_ptr<RecordACC> DataGodLoader::load_ACC_data(const std::string &input_path) {
  std::ifstream file;
  file.open(input_path, std::ios::binary);
  if (!file.is_open()) throw std::runtime_error{"ACC_xxx.csv not found..."};
  auto data = std::make_shared<RecordACC>();
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
    data->timestamp.emplace_back(timeinfo);
    data->microsecond.emplace_back(microsecond);
    data->x.emplace_back(x);
    data->y.emplace_back(y);
    data->z.emplace_back(z);
  }
  file.close();
  return data;
}
