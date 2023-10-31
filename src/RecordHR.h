//
// Created by dartixus on 24/09/2023.
//

#ifndef PPRKA_SVG_RECORDHR_H
#define PPRKA_SVG_RECORDHR_H
#include <chrono>

class RecordHR {

public:
    RecordHR();

    std::vector<double> x;
    std::vector<std::tm> timestamp;
};


#endif //PPRKA_SVG_RECORDHR_H
