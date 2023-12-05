//
// Created by Dan Schnurpfeil on 06.11.2023.
//

#include "utils.h"

#include <cmath>

void preprocess(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data,
                std::string &acc_filename) {
  std::cout << "starting preprocess... " << std::endl;
  if (acc_filename.find("_cleared.csv") == std::string::npos) {

    reduce_to_seconds(data);
    remove_redundant(data);
//    acc_filename.pop_back();
//    acc_filename.pop_back();
//    acc_filename.pop_back();
//    acc_filename.pop_back();
//    std::shared_ptr<RecordACC> data_first = std::make_shared<RecordACC>(*data.first);

//    save_cleared_ACC_data(acc_filename + "_cleared.csv", data_first);
//    std::cout << std::endl << std::endl << "created " << acc_filename
//                  << "_cleared.csv ... you can use it for nex run"
//                  << std::endl << std::endl;

  }
  normalize(data.first->x);
  normalize(data.first->y);
  normalize(data.first->z);
  normalize(data.second->x);
//  interpolate(data);
}
void interpolate(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data) {
  int interpolate_counter = 0;
  std::tm curr = data.first->timestamp[0];
  std::vector<float> interpolated;
  int hr_counter = 0;
  float start;
  float step;
  for (std::tm &i : data.first->timestamp) {

        if (i.tm_year == curr.tm_year && i.tm_mon == curr.tm_mon && i.tm_mday == curr.tm_mday &&
        i.tm_hour == curr.tm_hour && i.tm_min == curr.tm_min && i.tm_sec == curr.tm_sec) {
            interpolate_counter++;
        }
        else {
            start = data.second->x[hr_counter];
            hr_counter++;
            step = (data.second->x[hr_counter] - start) / (interpolate_counter + 1);
            for (int j = 0; j < interpolate_counter + 1; ++j) {
                interpolated.emplace_back(start + j * step);
            }
            hr_counter++;
            interpolate_counter = 0;
            curr = i;
        }
    }
    data.second->x = interpolated;
    if (data.second->x.size() != data.first->x.size())
        throw std::runtime_error{"bad computation!!!"};
}

void remove_redundant_acc(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data) {

  int hr_counter = 0;
  std::tm max_tm1 = {};
  // Set the maximum representable values for some members
  max_tm1.tm_sec = std::numeric_limits<int>::max();
  max_tm1.tm_min = std::numeric_limits<int>::max();
  max_tm1.tm_hour = std::numeric_limits<int>::max();
  max_tm1.tm_mday = std::numeric_limits<int>::max();
  max_tm1.tm_mon = std::numeric_limits<int>::max();
  max_tm1.tm_year = std::numeric_limits<int>::max();

  for (int i = 0; i < data.first->timestamp.size(); ++i) {

    if (data.first->timestamp[i].tm_year == data.second->timestamp[hr_counter].tm_year &&
        data.first->timestamp[i].tm_mon == data.second->timestamp[hr_counter].tm_mon &&
        data.first->timestamp[i].tm_mday == data.second->timestamp[hr_counter].tm_mday &&
        data.first->timestamp[i].tm_hour == data.second->timestamp[hr_counter].tm_hour &&
        data.first->timestamp[i].tm_min == data.second->timestamp[hr_counter].tm_min &&
        data.first->timestamp[i].tm_sec == data.second->timestamp[hr_counter].tm_sec
        ) {
      hr_counter++;
    } else {
      std::tm max_tm = {};
      // Set the maximum representable values for some members
      max_tm.tm_sec = std::numeric_limits<int>::max();
      max_tm.tm_min = std::numeric_limits<int>::max();
      max_tm.tm_hour = std::numeric_limits<int>::max();
      max_tm.tm_mday = std::numeric_limits<int>::max();
      max_tm.tm_mon = std::numeric_limits<int>::max();
      max_tm.tm_year = std::numeric_limits<int>::max();
      data.first->timestamp[i] = max_tm;
      data.first->x[i] = std::numeric_limits<float>::max();
      data.first->y[i] = std::numeric_limits<float>::max();
      data.first->z[i] = std::numeric_limits<float>::max();
      data.first->microsecond[i] = std::numeric_limits<float>::max();
    }

  }

  std::erase_if(data.first->timestamp, [&](tm value) {

    return value.tm_year == max_tm1.tm_year &&
        value.tm_mon == max_tm1.tm_mon &&
        value.tm_mday == max_tm1.tm_mday &&
        value.tm_hour == max_tm1.tm_hour &&
        value.tm_min == max_tm1.tm_min &&
        value.tm_sec == max_tm1.tm_sec;
  });

  std::erase_if(data.first->x, [&](float value) {
    return value == std::numeric_limits<float>::max();
  });
  std::erase_if(data.first->y, [&](float value) {
    return value == std::numeric_limits<float>::max();
  });
  std::erase_if(data.first->z, [&](float value) {
    return value == std::numeric_limits<float>::max();
  });
  std::erase_if(data.first->microsecond, [&](float value) {
    return value == std::numeric_limits<float>::max();
  });
}

void remove_redundant(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data) {
  if (data.first->timestamp.size() > data.second->timestamp.size()) {
    remove_redundant_acc(data);
    remove_redundant_hr(data);
  } else {
    remove_redundant_hr(data);
    remove_redundant_acc(data);
  }
}

void remove_redundant_hr(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data) {

  int acc_counter = 0;
  bool different = true;
  std::tm max_tm1 = {};
  // Set the maximum representable values for some members
  max_tm1.tm_sec = std::numeric_limits<int>::max();
  max_tm1.tm_min = std::numeric_limits<int>::max();
  max_tm1.tm_hour = std::numeric_limits<int>::max();
  max_tm1.tm_mday = std::numeric_limits<int>::max();
  max_tm1.tm_mon = std::numeric_limits<int>::max();
  max_tm1.tm_year = std::numeric_limits<int>::max();

  for (int i = 0; i < data.second->timestamp.size(); ++i) {
    if (data.second->timestamp[i].tm_year == data.first->timestamp[acc_counter].tm_year &&
        data.second->timestamp[i].tm_mon == data.first->timestamp[acc_counter].tm_mon &&
        data.second->timestamp[i].tm_mday == data.first->timestamp[acc_counter].tm_mday &&
        data.second->timestamp[i].tm_hour == data.first->timestamp[acc_counter].tm_hour &&
        data.second->timestamp[i].tm_min == data.first->timestamp[acc_counter].tm_min &&
        data.second->timestamp[i].tm_sec == data.first->timestamp[acc_counter].tm_sec
        ) {
      acc_counter++;
    } else {
      std::tm max_tm = {};
      // Set the maximum representable values for some members
      max_tm.tm_sec = std::numeric_limits<int>::max();
      max_tm.tm_min = std::numeric_limits<int>::max();
      max_tm.tm_hour = std::numeric_limits<int>::max();
      max_tm.tm_mday = std::numeric_limits<int>::max();
      max_tm.tm_mon = std::numeric_limits<int>::max();
      max_tm.tm_year = std::numeric_limits<int>::max();
      data.second->timestamp[i] = max_tm;
      data.second->x[i] = std::numeric_limits<float>::max();
    }
  }
    std::erase_if(data.second->timestamp, [&](tm value) {

      return value.tm_year == max_tm1.tm_year &&
          value.tm_mon == max_tm1.tm_mon &&
          value.tm_mday == max_tm1.tm_mday &&
          value.tm_hour == max_tm1.tm_hour &&
          value.tm_min == max_tm1.tm_min &&
          value.tm_sec == max_tm1.tm_sec;
    });
    std::erase_if(data.second->x, [&](float value) {
      return value == std::numeric_limits<float>::max();
    });

}

void reduce_to_seconds(std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data) {
//  todo ACC data are in angles maybe ... not in  m/s^2 the possible correlation can be shifted ... 🤔

  std::tm max_tm1 = {};
  // Set the maximum representable values for some members
  max_tm1.tm_sec = std::numeric_limits<int>::max();
  max_tm1.tm_min = std::numeric_limits<int>::max();
  max_tm1.tm_hour = std::numeric_limits<int>::max();
  max_tm1.tm_mday = std::numeric_limits<int>::max();
  max_tm1.tm_mon = std::numeric_limits<int>::max();
  max_tm1.tm_year = std::numeric_limits<int>::max();

  for (int i = 0; i < data.first->timestamp.size(); ++i) {
    if (data.first->microsecond[i] == 0.0
        ) {
      data.first->timestamp[i].tm_year += 1900;
      data.first->timestamp[i].tm_mon += 1;
    } else {
      std::tm max_tm = {};
      // Set the maximum representable values for some members
      max_tm.tm_sec = std::numeric_limits<int>::max();
      max_tm.tm_min = std::numeric_limits<int>::max();
      max_tm.tm_hour = std::numeric_limits<int>::max();
      max_tm.tm_mday = std::numeric_limits<int>::max();
      max_tm.tm_mon = std::numeric_limits<int>::max();
      max_tm.tm_year = std::numeric_limits<int>::max();
      data.first->timestamp[i] = max_tm;
      data.first->x[i] = std::numeric_limits<float>::max();
      data.first->y[i] = std::numeric_limits<float>::max();
      data.first->z[i] = std::numeric_limits<float>::max();
      data.first->microsecond[i] = std::numeric_limits<float>::max();
    }
  }
  std::erase_if(data.first->timestamp, [&](tm value) {

    return value.tm_year == max_tm1.tm_year &&
        value.tm_mon == max_tm1.tm_mon &&
        value.tm_mday == max_tm1.tm_mday &&
        value.tm_hour == max_tm1.tm_hour &&
        value.tm_min == max_tm1.tm_min &&
        value.tm_sec == max_tm1.tm_sec;
  });

  std::erase_if(data.first->x, [&](float value) {
    return value == std::numeric_limits<float>::max();
  });
  std::erase_if(data.first->y, [&](float value) {
    return value == std::numeric_limits<float>::max();
  });
  std::erase_if(data.first->z, [&](float value) {
    return value == std::numeric_limits<float>::max();
  });
  std::erase_if(data.first->microsecond, [&](float value) {
    return value == std::numeric_limits<float>::max();
  });
}


float calculateMean(const std::vector<float> &data) {
    float sum = 0.0;
    for (const float &value : data) {
        sum += value;
    }
    return sum / data.size();
}


float calculateStandardDeviation(const std::vector<float> &data, float mean) {
    float sum = 0.0;
    for (const float &value : data) {
        sum += std::pow(value - mean, 2);
    }
    return std::sqrt(sum / data.size());
}

void normalize(std::vector<float> &data) {
    float mean = calculateMean(data);
    float stddev = calculateStandardDeviation(data, mean);

    for (auto &i: data) {
        i = (i - mean) / stddev;
    }

    float min_val = *std::min_element(data.begin(), data.end());
    float max_val = *std::max_element(data.begin(), data.end());
    for (float& value : data) {
        value = (value - min_val) / (max_val - min_val);
    }
}
