//
// Created by dartixus on 24/09/2023.
//

#ifndef PPRKA_SVG_RECORDHR_H
#define PPRKA_SVG_RECORDHR_H
#include <chrono>

class RecordHR {

public:
    RecordHR(time_t timestamp, double x);

    time_t getTimestamp() const;

protected:
    time_t timestamp;
    double x;

};


#endif //PPRKA_SVG_RECORDHR_H
