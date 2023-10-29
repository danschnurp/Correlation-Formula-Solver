//
// Created by dartixus on 24/09/2023.
//

#ifndef PPRKA_SVG_RECORDHR_H
#define PPRKA_SVG_RECORDHR_H
#include <chrono>

class RecordHR {

public:
    RecordHR(std::tm timestamp, double x);

    std::tm getTimestamp() const;

protected:
    std::tm timestamp;
    double x;

};


#endif //PPRKA_SVG_RECORDHR_H
