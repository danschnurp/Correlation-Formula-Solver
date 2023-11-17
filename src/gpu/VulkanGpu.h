//
// Created by Dan Schnurpfeil on 3.11.2023.
//

#ifndef PPR_SRC_VULKANGPU_H_
#define PPR_SRC_VULKANGPU_H_

#include <vector>

class VulkanGpu {
 public:

  float compute_correlation(  const std::vector<float>& dataArray_x, const std::vector<float>& dataArray_y);

};

#endif //PPR_SRC_VULKANGPU_H_
