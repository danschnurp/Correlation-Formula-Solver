//
// Created by dartixus on 24/09/2023.
//

#include "RecordHR.h"


RecordHR::RecordHR(std::tm timestamp, double x) : timestamp(timestamp), x(x) {}

std::tm RecordHR::getTimestamp() const {
    return timestamp;
}
