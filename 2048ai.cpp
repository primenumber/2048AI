#include <iostream>
#include "grid.hpp"
#include "random.hpp"
#include "search.hpp"

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
    int move = ai2048::search::search(grid);
    std::cout << "{\"type\":\"move\",\"direction\":"<< move <<"}"<< std::endl;
  }
  return 0;
}

