#include "Loader.h"
#include <memory>
#include <thread>
#include <iostream>
#include "VulkanGpu.h"




int main(int argc, char **argv) {

  auto gpu_comp_unit = std::make_unique<VulkanGpu>();
  gpu_comp_unit->prepare();
  return 0;

    try {
        std::cout << "starting... " << std::endl;

        bool load_sequential = false;

      std::cout << "load sequentially: " << load_sequential << std::endl;

      auto start_time = std::chrono::high_resolution_clock::now();
      std::vector<RecordHR> data_hr;
        auto loader_hr = std::make_unique<DataGodLoader>();
      std::string hr_filename = "../data/HR_007.csv";
        std::string acc_filename = "../data/ACC_007.csv";
        auto loader_acc = std::make_unique<DataGodLoader>();
        std::vector<RecordACC> data_acc;

        if (load_sequential) {
            data_acc = loader_acc->load_ACC_data(acc_filename);
            data_hr = loader_hr->load_HR_data(hr_filename);
        }
       else {
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

      if (data_acc.empty()) {
        return 1;
      }
      if (data_hr.empty()) {
        return 1;
      }

      auto end_time = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
      std::cout << "Time taken: " << duration.count() / 1000 << " seconds" << std::endl;
      std::cout << "loaded acc " << data_acc.size() << std::endl;
      std::cout << "loaded hr " << data_hr.size() << std::endl;

    }
    catch (std::exception &err) {
      std::cerr << std::endl << err.what() << std::endl;
      return 1;
    }
    std::cout << "done... " << std::endl;
    return 0;

}