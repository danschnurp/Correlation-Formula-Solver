//
// Created by dartixus on 24/09/2023.
//

#ifndef PPRKA_SVG_RECORDHR_H
#define PPRKA_SVG_RECORDHR_H
#include <chrono>

class RecordHR {
public:
    RecordHR(double value, const std::string &timeStr);

private:
public:
    double getValue() const;

    const tm &getTimeInfo() const;

private:
    double value;
    std::tm timeInfo;

};


#endif //PPRKA_SVG_RECORDHR_H
