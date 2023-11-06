#include "io/Loader.h"
#include <memory>
#include <thread>
#include <iostream>
#include "VulkanGpu.h"

int main(int argc, char **argv) {
  auto gpu_comp_unit = std::make_unique<VulkanGpu>();
  gpu_comp_unit->prepare();
    try {

      std::cout << "starting... " << std::endl;
      bool load_sequential = false;

      auto start_time = std::chrono::high_resolution_clock::now();
      std::string hr_filename = "../data/HR_007.csv";
      std::string acc_filename = "../data/ACC_007.csv";

      auto data = load_data(hr_filename, acc_filename, load_sequential);

      normalize(data);

      if (data.first->x.empty()) {
        return 1;
      }
      if (data.second->x.empty()) {
        return 1;
      }
      auto end_time = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
      std::cout << "Time taken: " << duration.count() / 1000 / 60 << " minutes" << std::endl;
      std::cout << "loaded acc " << data.first->x.size() << std::endl;
      std::cout << "loaded hr " << data.second->x.size() << std::endl;
    }
    catch (std::exception &err) {
      std::cerr << std::endl << err.what() << std::endl;
      return 1;
    }
    std::cout << "done... " << std::endl;
    return 0;
}
