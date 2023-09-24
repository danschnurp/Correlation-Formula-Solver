//
// Created by dartixus on 24/09/2023.
//

#include "RecordHR.h"
#include <sstream>

RecordHR::RecordHR(double value, const std::string &timeStr) : value(value) {

    std::istringstream ss(timeStr);

    int year, month, day, hour, minute, second;

    // Parse the input string
    ss >> year;
    ss.ignore(); // Ignore the dash (-)
    ss >> month;
    ss.ignore(); // Ignore the dash (-)
    ss >> day;
    ss.ignore(); // Ignore the space
    ss >> hour;
    ss.ignore(); // Ignore the colon (:)
    ss >> minute;
    ss.ignore(); // Ignore the colon (:)
    ss >> second;

    timeInfo.tm_year = year - 1900; // Years since 1900
    timeInfo.tm_mon = month - 1;    // Months are 0-based
    timeInfo.tm_mday = day;
    timeInfo.tm_hour = hour;
    timeInfo.tm_min = minute;
    timeInfo.tm_sec = second;

}


double RecordHR::getValue() const {
    return value;
}

const tm &RecordHR::getTimeInfo() const {
    return timeInfo;
}
