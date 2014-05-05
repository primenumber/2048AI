#include "random.hpp"

std::mt19937 mt;

void init_random() {
  std::random_device rd;
  mt = std::mt19937(rd());
}
