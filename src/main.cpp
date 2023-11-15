//
// Created by Dan Schnurpfeil on 21.09.2023.
//
#include "io/Loader.h"
#include "preprocessing/utils.h"
#include <thread>
#include <iostream>
#include "VulkanGpu.h"
#include "genetic_alg/Population.h"
#include <memory>


/**
 * The preprocess function normalizes and processes accelerometer and heart rate data, removes redundant data, and saves
 * the cleared accelerometer data.
 *
 * @param shared_ptr A shared pointer is a smart pointer that can be used to manage the lifetime of dynamically allocated
 * objects. It allows multiple shared pointers to share ownership of the same object, and automatically deletes the object
 * when the last shared pointer that owns it is destroyed.
 * @param acc_filename The `acc_filename` parameter is a reference to a string that represents the filename of the
 * accelerometer data file.
 */
void preprocess(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data,
                std::string &acc_filename) {
    if (acc_filename.find("_cleared.csv") == std::string::npos) {
        normalize(data.first->x);
        normalize(data.first->y);
        normalize(data.first->z);
    }
    normalize(data.second->x);
    if (acc_filename.find("_cleared.csv") == std::string::npos) {
        remove_redundant(data);
        save_cleared_ACC_data("../data/ACC_007_cleared.csv", data.first);
    }
    interpolate(data);
}

int main(int argc, char **argv) {
  auto gpu_comp_unit = std::make_unique<VulkanGpu>();
  gpu_comp_unit->prepare();
    try {

      std::cout << "starting... " << std::endl;
      bool load_sequential = false;

      auto start_time = std::chrono::high_resolution_clock::now();
      std::string hr_filename = "../data/HR_007.csv";
      std::string acc_filename = "../data/ACC_007_cleared.csv";

      auto data = load_data(hr_filename, acc_filename, load_sequential);

      if (data.first->x.empty()) {
        return 1;
      }
      if (data.second->x.empty()) {
        return 1;
      }

      auto end_time = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << "Time taken: " << duration.count() / 1000 / 60 << " minutes "
                  << duration.count() / 1000 % 60 << " seconds" << std::endl;

//        std::cout << "loaded hr " << data.second->x.size() << std::endl;
//      std::cout << "loaded acc " << data.first->x.size() << std::endl;

        start_time = std::chrono::high_resolution_clock::now();
        //  performing some preprocessing steps on the data before it is used further
        preprocess(data, acc_filename);
        //
      std::cout << "clean acc " << data.first->x.size() << std::endl;
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << "Time taken: " << duration.count() / 1000 / 60 << " minutes "
        << duration.count() / 1000 % 60 << " seconds" << std::endl;

        auto population = std::make_unique<Population>();
//        todo some values are out fo range 0,1

// test code
//        std::cout << population;
//
//        for (int i = 0; i < population.populationSize; ++i) {
//            double_t result = population.equations[i]->evaluate(0.5, 0.5, 0.5);
//            std::cout << "evaluation for x 1: " << result << std::endl;
//        }


    }
    catch (std::exception &err) {
      std::cerr << std::endl << err.what() << std::endl;
      return 1;
    }
    std::cout << "done... " << std::endl;
    return 0;
}
