#include <iostream>
#include "grid.hpp"
#include "score.hpp"
#include "random.hpp"

int main() {
  using namespace ai2048::grid;
  init_random();
  while (true) {
    ai2048::grid::Grid grid;
    for (auto& a : grid.table) {
      for (auto& e : a) {
        std::cin >> e;
      }
    }
    auto movable = grid.movable_directions();
    if (movable.empty()) break;
    int max_value = -1000000000;
    int max_d = 0;
    for (auto direction : movable) {
      int score = ai2048::score::static_score(grid);
      if (score > max_value) {
        max_value = score;
        max_d = direction;
      }
    }
    std::cout << "{\"type\":\"move\",\"direction\":"<< max_d <<"}"<< std::endl;
  }
  return 0;
}

