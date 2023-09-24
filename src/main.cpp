//
// Created by Dan Schnurpfeil on 21.09.2023.
//
#include "Loader.h"
#include <memory>

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
        std::cout << "Time taken: " << duration.count() / 1000 << "seconds" << std::endl;


    }
    catch (std::exception & err) {
        std::cerr << std::endl << err.what() << std::endl;
        return 1;
    }

}