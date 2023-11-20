//
// Created by dartix on 17 Nov 2023.
//

#include "Demo.h"

#include "example_filter.h"
#include "vulkan_helpers.hpp"

void test_demo() {
  const auto width = 90;
  const auto height = 60;
  const auto a = 2.0f; // saxpy scaling factor

  auto y = std::vector<float>(width * height, 0.71f);
  auto x = std::vector<float>(width * height, 0.65f);

  std::cout << "VULKAN correctness demo: result = 2 * 0.65 + 0.71 ...should be 2.01" << std::endl;

  ExampleFilter f("../saxpy.spv");
  auto d_y = vuh::Array<float>::fromHost(y, f.device, f.physDevice);
  auto d_x = vuh::Array<float>::fromHost(x, f.device, f.physDevice);

  f(d_y, d_x, {width, height, a});

  auto out_tst = std::vector<float>{};
  d_y.to_host(out_tst); // and now out_tst should contain the result of saxpy (y = y + ax)
  std::cout << "result " << out_tst[0] << std::endl;

}