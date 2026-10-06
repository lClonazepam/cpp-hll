#include "hll.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

int main() {
  kit::HyperLogLog hll(12);
  for (int i = 0; i < 5000; ++i) hll.add("id-" + std::to_string(i));
  for (int i = 0; i < 5000; ++i) hll.add("id-" + std::to_string(i));
  const double est = hll.estimate();
  assert(std::abs(est - 5000.0) / 5000.0 < 0.08);
  std::cout << "hll ok estimate=" << est << "\n";
}
