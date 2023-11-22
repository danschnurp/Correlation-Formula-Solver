//
// Created by Dan Schnurpfeil on 21.09.2023.
//
#include "io/Loader.h"
#include "preprocessing/utils.h"
#include <thread>
#include <iostream>
#include "genetic_alg/Population.h"
#include <memory>

void print_time(auto start_time) {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Time taken: " << duration.count() / 1000 / 60 << " minutes "
              << duration.count() / 1000.0 << " seconds" << std::endl  << std::endl;
}

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
    std::cout << "starting preprocess... " << std::endl;
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

    const auto width = 90;
    const auto height = 60;
    const auto a = 2.0f; // saxpy scaling factor

    auto y = std::vector<float>(width * height, 0.71f);
    auto x = std::vector<float>(width * height, 0.65f);

    std::cout << "VULKAN correctness demo: result = 2 * 0.65 + 0.71 ...should be 2.01" << std::endl;

    ComputationUnit f("../saxpy.spv");
    auto d_y = vuh::Array<float>::fromHost(y, f.device, f.physDevice);
    auto d_x = vuh::Array<float>::fromHost(x, f.device, f.physDevice);

    f.compute(d_y, d_x, {width, height, a});

    auto out_tst = std::vector<float>{};
    d_y.to_host(out_tst); // and now out_tst should contain the result of saxpy (y = y + ax)
    std::cout << "result " << out_tst[0] << std::endl;


  try {

    std::cout  << std::endl << "starting... " << std::endl;
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


      std::cout << "loaded hr " << data.second->x.size() << std::endl;
      std::cout << "loaded acc " << data.first->x.size() << std::endl;
      print_time(start_time);

    start_time = std::chrono::high_resolution_clock::now();
    //  performing some preprocessing steps on the data before it is used further
    preprocess(data, acc_filename);

    std::cout << "clean acc " << data.first->x.size() << std::endl;
    print_time(start_time);


    auto population = std::make_unique<Population>();
    std::cout << "starting correlation compute... " << std::endl;

    population->prepareForFitFunction(data.second->x);
    std::vector<Equation> new_equations;
    std::vector<float> population_results;
    std::vector<float> equation_results;
    for (int i = 0; i < population->populationSize; ++i) {
      try {

          start_time = std::chrono::high_resolution_clock::now();

          equation_results = population->evaluate(data.first->x,data.first->y,data.first->z,i);
//          equation_results = population->evaluateCPU(data.first->x,data.first->y,data.first->z,i);

        new_equations.push_back(population->equations[i]);
          std::cout << "evaluation of equation... " << std::endl;
          print_time(start_time);


          start_time = std::chrono::high_resolution_clock::now();
        float corr = population->countFitFunction(equation_results);
        population_results.emplace_back(corr);
          print_time(start_time);

      }
      catch (std::invalid_argument &ex) {
//        std::cerr << std::endl << ex.what() << std::endl;
        equation_results.clear();
        continue;
      }
    }
      std::cout << "parents: " << population_results.size() << std::endl;

    start_time = std::chrono::high_resolution_clock::now();

    population->equations = new_equations;
    std::cout << "started crossbreeding... " << std::endl;
     std::vector<Equation> children = population->crossbreed();
      std::cout << "children: " << children.size() << std::endl;

      print_time(start_time);
  }
    catch (std::exception &err) {
      std::cerr << std::endl << err.what() << std::endl;
      return 1;
    }
    std::cout << "done... " << std::endl;
    return 0;
}
