//
// Created by Dan Schnurpfeil on 24.09.2023.
//

#include "RecordACC.h"

RecordACC::RecordACC(time_t timestamp, double x, double microsecond, double y, double z) : RecordHR(timestamp, x),
                                                                                           microsecond(microsecond),
                                                                                           y(y), z(z) {}
