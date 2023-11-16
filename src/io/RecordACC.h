//
// Created by Dan Schnurpfeil on 24.09.2023.
//

#ifndef PPRKA_SVG_RECORDACC_H
#define PPRKA_SVG_RECORDACC_H
#include "RecordHR.h"

class RecordACC {

public:
    RecordACC();

    std::vector<float> microsecond;
    std::vector<float> y;
    std::vector<float> z;
    std::vector<float> x;
    std::vector<std::tm> timestamp;


};


#endif //PPRKA_SVG_RECORDACC_H
