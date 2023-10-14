//
// Created by Dan Schnurpfeil on 21.09.2023.
//
#include "Loader.h"
#include <memory>
#include <thread>
#include <CL/opencl.hpp>

bool prepare_opencl() {

  cl::Platform platform;
  cl::Device device;
  std::cout << std::endl;

  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);

  if (platforms.empty()) {
    std::cerr << "No OpenCL platforms found." << std::endl;
    return false;
  }

  platform = platforms[0];

  std::vector<cl::Device> devices;
  platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

  if (devices.empty()) {
    std::cerr << "No suitable OpenCL devices found." << std::endl;
    return false;
  }

  device = devices[0];

  cl::Context context(device);

  return true;

//  cl::Program::Sources sources;
//  sources.push_back({your_kernel_source_code_here});
//
//  cl::Program program(context, sources);

//program.build();

//  cl::Kernel kernel(program, "your_kernel_function_name");

//kernel arguments

//  kernel.setArg(0, arg0);
//  kernel.setArg(1, arg1);
// ...

//execute

//  cl::CommandQueue queue(context, device);
//  cl::NDRange global_work_size(/* specify global work size */);
//  queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_work_size);
//  queue.finish();


// read back

//  cl::Buffer buffer(context, CL_MEM_READ_WRITE, sizeof(/* specify buffer size */));
//  queue.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(/* specify buffer size */), result_data);



}

int main(int argc, char **argv) {
    try {

      auto start_time = std::chrono::high_resolution_clock::now();
      std::vector<RecordHR> data_hr;
      std::vector<RecordACC> data_acc;
      std::string hr_filename = "../data/HR_007.csv";
      std::string acc_filename = "../data/ACC_007.csv";
      auto hr_thread_loader = std::thread([&hr_filename, &data_hr]() {
        try {
          data_hr = load_HR_data(hr_filename);
        }
        catch (std::exception &err) {
          std::cerr << std::endl << err.what() << std::endl;
        }

      });

      auto acc_thread_loader = std::thread([&acc_filename, &data_acc]() {
        try {
          data_acc = load_ACC_data(acc_filename);
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

      if (data_acc.empty()) {
        return 1;
      }
      if (data_hr.empty()) {
        return 1;
      }

      auto end_time = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
      std::cout << "Time taken: " << duration.count() / 1000 << " seconds" << std::endl;
      std::cout << "done acc " << data_acc.size() << std::endl;
      std::cout << "done hr " << data_hr.size() << std::endl;

      if (!prepare_opencl()) {
        throw std::runtime_error{"OpenCL not found..."};
      }

    }
    catch (std::exception &err) {
      std::cerr << std::endl << err.what() << std::endl;
      return 1;
    }

}