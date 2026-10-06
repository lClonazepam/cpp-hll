#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string_view>
#include <vector>

namespace kit {

class HyperLogLog {
 public:
  explicit HyperLogLog(int precision = 12)
      : p_(std::clamp(precision, 4, 16)), m_(1u << p_), registers_(m_, 0) {}

  void add(std::string_view key) {
    const auto h = fnv(key);
    const auto idx = static_cast<std::size_t>(h >> (64 - p_));
    const auto w = h << p_;
    const auto rho = static_cast<std::uint8_t>(w == 0 ? (64 - p_ + 1) : __builtin_clzll(w) + 1);
    registers_[idx] = std::max(registers_[idx], rho);
  }

  double estimate() const {
    double sum = 0;
    int zeros = 0;
    for (auto r : registers_) {
      sum += std::ldexp(1.0, -static_cast<int>(r));
      if (r == 0) ++zeros;
    }
    const double alpha = alpha_m(m_);
    double e = alpha * m_ * m_ / sum;
    if (e <= 2.5 * m_ && zeros > 0) e = m_ * std::log(static_cast<double>(m_) / zeros);
    else if (e > (1.0 / 30.0) * kTwo64) e = -kTwo64 * std::log(1.0 - e / kTwo64);
    return e;
  }

  int precision() const { return p_; }

 private:
  static constexpr double kTwo64 = 18446744073709551616.0;
  static double alpha_m(std::uint32_t m) {
    if (m == 16) return 0.673;
    if (m == 32) return 0.697;
    if (m == 64) return 0.709;
    return 0.7213 / (1.0 + 1.079 / m);
  }
  static std::uint64_t fnv(std::string_view key) {
    std::uint64_t h = 0xcbf29ce484222325ull;
    for (unsigned char c : key) { h ^= c; h *= 0x100000001b3ull; }
    h ^= h >> 33; h *= 0xff51afd7ed558ccdull; h ^= h >> 33;
    return h;
  }
  int p_;
  std::uint32_t m_;
  std::vector<std::uint8_t> registers_;
};

}  // namespace kit
