//
// Created by Dan Schnurpfeil on 21.09.2023.
//
#include "preprocessing/utils.h"
#include <thread>
#include <iostream>
#include "genetic_alg/Population.h"
#include <memory>

int main(int argc, char **argv) {


//todo parameters
// input data path
//  cleared acc or no
// GPU device to run
// GPU workgroup size
// precompile spirv  check
// equation xyz interval
// evaluate on cpu or on gpu

// todo refactor gpu computation unit



  const auto width = 90;
  const auto height = 60;
  const auto a = 2.0f; // saxpy scaling factor

  auto y = std::vector<float>(width * height, 0.71f);
  auto x = std::vector<float>(width * height, 0.65f);

  std::cout << "VULKAN correctness demo: result = 2 * 0.65 + 0.71 ...should be 2.01" << std::endl;

  ComputationUnit f("../saxpy.spv", true);
  auto d_y = vuh::Array<float>::fromHost(y, f.device, f.physDevice);
    auto d_x = vuh::Array<float>::fromHost(x, f.device, f.physDevice);

    f.compute(d_y, d_x, {width, height, a});

    auto out_tst = std::vector<float>{};
    d_y.to_host(out_tst); // and now out_tst should contain the result of saxpy (y = y + ax)
    std::cout << "result " << out_tst[0] << std::endl;

    try {
        std::cout << std::endl << "starting... " << std::endl;
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
      auto population = std::make_unique<Population>(data);

        population->prepareForFitFunction(data.second->x);

        for (int epoch = 0; epoch < 1000; ++epoch) {
          population->create_one_generation(epoch % 20);
          std::cout << "epoch " << epoch << " done... " << std::endl << "##########################" << std::endl;
        }
    }
        catch (std::exception & err)
        {
            std::cerr << std::endl << err.what() << std::endl;
            return 1;
        }
    std::cout << "done... " << std::endl;
    return 0;
}
