//
// Created by Dan Schnurpfeil on 24/09/2023.
//

#ifndef PPRKA_SVG_RECORDHR_H
#define PPRKA_SVG_RECORDHR_H
#include <chrono>
#include <vector>

class RecordHR {

public:
    RecordHR();

    std::vector<float> x;
    std::vector<std::tm> timestamp;
};


#endif //PPRKA_SVG_RECORDHR_H
