// by https://github.com/salvipeter/dfo
#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <numeric>

namespace NelderMead {

  using Point = std::vector<double>;
  using Function = std::function<double (const Point &)>;

  bool optimize(const Function &f, Point &x, size_t max_iteration, double tolerance,
                double step_length);

}
