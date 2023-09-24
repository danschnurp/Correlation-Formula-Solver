//
// Created by dartixus on 24/09/2023.
//

#include "RecordHR.h"


RecordHR::RecordHR(time_t timestamp, double x) : timestamp(timestamp), x(x) {}

time_t RecordHR::getTimestamp() const {
    return timestamp;
}
