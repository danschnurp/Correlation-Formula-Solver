//
// Created by Dan Schnurpfeil on 21.09.2023.
//
#include "preprocessing/utils.h"
#include <thread>
#include <iostream>
#include "genetic_alg/Population.h"
#include <memory>
#include <filesystem>
#include "genetic_alg/Node.h"
#include "genetic_alg/Equation.h"
#include "io/svg_gen.h"

struct Options {
  std::string dataPathHR;
  std::string dataPathACC;
  bool loadSequentially = false;

  float minimumEquationCoefficients = -5.0;
  float maximumEquationCoefficients = 5.0;
  float maximumEquationInitLength = 10;

  bool useGPU = true;
  int workgroupSize = 16;
  std::string shadersDir = "../";
};

void printHelp() {
  std::cout << "Usage: ppr(.exe) --data_path_hr <path_HR> --data_path_acc <path_ACC> [--not_use_gpu]\n";
}

Options parseArgs(int argc, char *argv[]) {
  Options options;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "--data_path_hr" && i + 1 < argc) {
      options.dataPathHR = argv[++i];
    } else if (arg == "--data_path_acc" && i + 1 < argc) {
      options.dataPathACC = argv[++i];
    } else if (arg == "--shaders_dir" && i + 1 < argc) {
      options.shadersDir = argv[++i];
    } else if (arg == "--not_use_gpu") {
      options.useGPU = false;
    } else if (arg == "--load_data_sequentially") {
      options.loadSequentially = true;
    } else if (arg == "--gpu_workgroup_size" && i + 1 < argc) {
      options.workgroupSize = std::stoi(argv[++i]);
    } else if (arg == "--minimum_equation_coefficients" && i + 1 < argc) {
      options.minimumEquationCoefficients = std::stof(argv[++i]);
    } else if (arg == "--maximum_equation_coefficients" && i + 1 < argc) {
      options.maximumEquationCoefficients = std::stof(argv[++i]);
    } else if (arg == "--maximum_equation_init_length" && i + 1 < argc) {
      options.maximumEquationInitLength = std::stof(argv[++i]);
    } else {
      // Print help if unknown argument is encountered
      printHelp();
      std::exit(EXIT_FAILURE);
    }
  }

  // Validate if required arguments are provided
  if (options.dataPathHR.empty() || options.dataPathACC.empty()
      || !std::filesystem::exists(options.dataPathHR) || !std::filesystem::exists(options.dataPathACC)) {
    printHelp();
    std::exit(EXIT_FAILURE);
  }

  return options;
}

void vulkan_corectness_test(std::string &shader_dir, int WORKGROUP_SIZE) {
  const auto width = 90;
  const auto height = 60;
  const auto a = 2.0f; // saxpy scaling factor

  auto y = std::vector<float>(width * height, 0.71f);
  auto x = std::vector<float>(width * height, 0.65f);

  std::cout << "VULKAN correctness test:" << std::endl;
  ComputationUnit f(shader_dir + "plus.spv", true, WORKGROUP_SIZE);
  auto d_y = vuh::Array<float>::fromHost(y, f.device, f.physDevice);
  auto d_x = vuh::Array<float>::fromHost(x, f.device, f.physDevice);
  f.compute(d_y, d_x, {width, height, a});
  auto out_tst = std::vector<float>{};
  d_y.to_host(out_tst); // and now out_tst should contain the result of saxpy (y = y + ax)
  if (std::abs(out_tst[0] - 2.01) < 0.00001)
    std::cout << "result OK" << std::endl;
}

int main(int argc, char **argv) {

  Options options = parseArgs(argc, argv);
  std::cout << std::endl << "Your provided parameters and default ones:" << std::endl;
  std::cout << "Data Path HR: " << options.dataPathHR << std::endl;
  std::cout << "Data Path ACC: " << options.dataPathACC << std::endl;
  std::cout << "Use GPU: " << std::boolalpha << options.useGPU << std::endl;
  if (options.useGPU) {
    std::cout << "Workgroup Size: " << options.workgroupSize << std::endl;
    std::cout << "shader " << options.shadersDir + "plus.spv" << " exists: "
              << std::filesystem::exists(options.shadersDir + "plus.spv") << std::endl;
    std::cout << "shader " << options.shadersDir + "minus.spv" << " exists: "
              << std::filesystem::exists(options.shadersDir + "minus.spv") << std::endl;
    std::cout << "shader " << options.shadersDir + "multiply.spv" << " exists: "
              << std::filesystem::exists(options.shadersDir + "multiply.spv") << std::endl;
  }

  int WORKGROUP_SIZE = options.workgroupSize;

  std::cout << "minimum_equation_coefficients: " << options.minimumEquationCoefficients << std::endl;
  std::cout << "maximum_equation_coefficients: " << options.maximumEquationCoefficients << std::endl;
  std::cout << "maximum_equation_init_length: " << options.maximumEquationInitLength << std::endl << std::endl;


  // todo refactor gpu computation unit



  if (options.useGPU) vulkan_corectness_test(options.shadersDir, WORKGROUP_SIZE);

  try {
    std::cout << std::endl << "starting... " << std::endl;
    bool load_sequential = options.loadSequentially;
    auto start_time = std::chrono::high_resolution_clock::now();
    std::string hr_filename = options.dataPathHR;
    std::string acc_filename = options.dataPathACC;

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
    std::cout << "clean hr " << data.second->x.size() << std::endl;
    print_time(start_time);
    auto population = std::make_unique<Population>(data,
                                                   options.useGPU,
                                                   WORKGROUP_SIZE,
                                                   options.minimumEquationCoefficients,
                                                   options.maximumEquationCoefficients,
                                                   options.maximumEquationInitLength);

    population->prepareForFitFunction(data.second->x, WORKGROUP_SIZE);

    for (int epoch = 0; epoch < 10; ++epoch) {
      population->create_one_generation(epoch % 20);
      std::cout << "epoch " << epoch << " done... " << std::endl << "##########################" << std::endl;
    }

    make_svg(population->bestOne, data);

  }
        catch (std::exception & err)
        {
            std::cerr << std::endl << err.what() << std::endl;
            return 1;
        }
    std::cout << "done... " << std::endl;
    return 0;
}
