//
// Created by Dan Schnurpfeil on 24.09.2023.
//

#ifndef PPRKA_SVG_RECORDACC_H
#define PPRKA_SVG_RECORDACC_H
#include "RecordHR.h"

class RecordACC : RecordHR {

public:
    RecordACC(time_t timestamp, double x, double microsecond, double y, double z);

private:
    double microsecond;
    double y;
    double z;


};


#endif //PPRKA_SVG_RECORDACC_H
