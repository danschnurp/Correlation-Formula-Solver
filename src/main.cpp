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
        auto data_acc = loader->load_ACC_data("../data/ACC_007.csv");
        std::cout <<  "done acc " << data_acc.size() << std::endl;

    }
    catch (std::exception & err) {
        std::cerr << std::endl << err.what() << std::endl;
        return 1;
    }

}