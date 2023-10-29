//
// Created by Dan Schnurpfeil on 24.09.2023.
//

#include "RecordACC.h"

RecordACC::RecordACC(std::tm timestamp, double x, double microsecond, double y, double z) : RecordHR(timestamp, x),
                                                                                           microsecond(microsecond),
                                                                                           y(y), z(z) {}
