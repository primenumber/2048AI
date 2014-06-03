#include "search.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include <chrono>

extern std::mt19937 mt;

namespace ai2048 {
namespace search {

int search(const grid::Grid& grid) {
  using std::begin;
  using std::end;
  using std::max_element;
  auto simple_search_scores = simple_search(grid);
  return max_element(begin(simple_search_scores), end(simple_search_scores)) - begin(simple_search_scores);
}

} // namespace search
} // namespace ai2048
