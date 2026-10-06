# cpp-hll

Header-only HyperLogLog cardinality estimator. Count distinct keys in a few kilobytes, with duplicate-insensitive adds.

```cpp
kit::HyperLogLog hll(14);
hll.add(user_id);
double uniques = hll.estimate();
```

Typical error is about 1.04 / sqrt(2^p). MIT
