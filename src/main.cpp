//
// Created by Dan Schnurpfeil on 21.09.2023.
//
#include "preprocessing/utils.h"
#include <thread>
#include <iostream>
#include "genetic_alg/Population.h"
#include <memory>
#include <filesystem>
#include "io/svg_gen.h"

struct Options {
  std::string dataPathHR;
  std::string dataPathACC;
  bool loadSequentially = false;

  float minimumEquationCoefficients = 0.999;
  float maximumEquationCoefficients = 1.001;
  float maximumEquationInitLength = 4;

  int epochs = 10;

  bool useGPU = true;
  int workgroupSize = 16;
  std::string shadersDir = "../";
};

void printHelp() {
  std::cout << "Usage: ppr(.exe) --data_path_hr <path_HR> --data_path_acc <path_ACC> [--not_use_gpu "
               "--load_data_sequentially --gpu_workgroup_size <INTEGER>\n"
               " --minimum_equation_coefficients <INTEGER> \n"
               "--maximum_equation_coefficient <INTEGER> \n"
               "--epochs <INTEGER> \n"
               "--maximum_equation_init_length ]\n";
}

/**
 * The function `parseArgs` parses command line arguments and returns an `Options` object.
 *
 * @param argc The `argc` parameter is an integer that represents the number of command-line arguments passed to the
 * program. It includes the name of the program itself as the first argument.
 * @param argv An array of C-style strings (char*) representing the command-line arguments passed to the program.
 *
 * @return an object of type `Options`.
 */
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
    } else if (arg == "--epochs" && i + 1 < argc) {
      options.epochs = std::stoi(argv[++i]);
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

/**
 * The function performs a Vulkan correctness test by computing a saxpy operation on two vectors using a specified shader.
 *
 * @param shader_dir The `shader_dir` parameter is a string that represents the directory where the shader file is located.
 * It is used to construct the full path to the shader file by appending the shader filename to the `shader_dir` string.
 * @param WORKGROUP_SIZE The `WORKGROUP_SIZE` parameter is an integer that determines the number of workgroups in each
 * dimension of the compute shader. It specifies the number of shader invocations that will be executed concurrently. The
 * value of `WORKGROUP_SIZE` will depend on the specific requirements of your application and the capabilities of
 */
void vulkan_correctness_test(std::string &shader_dir, int WORKGROUP_SIZE) {
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
  d_y.to_host(out_tst);
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

  if (options.useGPU) vulkan_correctness_test(options.shadersDir, WORKGROUP_SIZE);

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
    // todo dont ignore ( * ) before ( + - )
    // todo simplifying the equations
    for (int epoch = 0; epoch < options.epochs; ++epoch) {
      population->create_one_generation(1.25 + 0.1 * (epoch % 20));
      std::cout << "epoch " << epoch << " done... " << std::endl << "##########################" << std::endl;
    }

    make_svg(population->bestOne,
             population->bestCorr,
             data,
             options.dataPathACC.substr(options.dataPathACC.size() - 7, 3));

  }
        catch (std::exception & err)
        {
            std::cerr << std::endl << err.what() << std::endl;
            return 1;
        }
    std::cout << "done... " << std::endl;
    return 0;
}
