//
// Created by Dan Schnurpfeil on 21.09.2023.
//
#include "Loader.h"
#include <memory>
#include <OpenCL/opencl.h>

int prepare_opencl() {
  // todo device_id context ...ven
  cl_device_id device_id;
  int gpu = 1;
  int err = clGetDeviceIDs(NULL, gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
  if (err != CL_SUCCESS) {
    std::cout <<   "Error: Failed to create a device group!" << std::endl;
    return EXIT_FAILURE;
  } else {
    std::cout <<   "Device group created" << std::endl;
  }

  cl_context context;
  // Create a compute context
  //
  context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
  if (!context) {
    std::cout <<  "Error: Failed to create a compute context!" << std::endl;
    return EXIT_FAILURE;
  } else {
    std::cout <<   "Compute context created" << std::endl;
  }

  cl_command_queue commands;
  // Create a command commands
  //
  commands = clCreateCommandQueue(context, device_id, 0, &err);
  if (!commands) {
    std::cout << "Error: Failed to create a Command Queue!" << std::endl;
    return EXIT_FAILURE;
  }  else {
    std::cout <<   "Command Queue created" << std::endl;
  }
  return EXIT_SUCCESS;

}

int main(int argc, char **argv) {
  try {
    auto loader = std::make_unique<Loader>();

    auto data_hr = loader->load_HR_data("../data/HR_007.csv");
    std::cout <<  "done hr " << data_hr.size() << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    auto data_acc = loader->load_ACC_data("../data/ACC_007.csv");
    std::cout <<  "done acc " << data_acc.size() << std::endl;

    auto end_time = std::chrono::high_resolution_clock::now();
    // Calculate the duration
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    // Print the duration in milliseconds
    std::cout << "Time taken: " << duration.count() / 1000 << " seconds" << std::endl;
    if (prepare_opencl()) {
      throw  std::runtime_error{"OpenCL not found..."};
    }

  }
  catch (std::exception & err) {
    std::cerr << std::endl << err.what() << std::endl;
    return 1;
  }

}