//
// Created by Dan Schnurpfeil on 21.09.2023.
//
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char **argv) {
    std::ifstream file;
    file.open("../data/HR_007.csv", std::ios::in);
    std::string line;

    while (std::getline(file, line)) {
        std::cout <<  "line" << line << std::endl;
    }

}