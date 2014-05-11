#include <chrono>
#include <iostream>
#include <memory>
#include "grid.hpp"
#include "random.hpp"
#include "search.hpp"
#include "gametree.hpp"

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
    ai2048::grid::Direction move = ai2048::search::alpha_beta_search(grid);
    //for (int d = 5; d < 40; ++d) {
    //  std::shared_ptr<ai2048::gamedag::GameDAG> root(
    //      std::make_shared<ai2048::gamedag::GameDAG>(ai2048::game::Game(grid, ai2048::game::State::Player)));
    //  std::map<ai2048::game::Game, std::shared_ptr<ai2048::gamedag::GameDAG>> m;
    //  auto start = std::chrono::system_clock::now();
    //  move = root->getOptimal(m, d);
    //  auto end = std::chrono::system_clock::now();
    //  auto elapsed = end - start;
    //  if (elapsed.count() > 50000) break;
    //}
    std::cout << "{\"type\":\"move\",\"direction\":"<< move <<"}"<< std::endl;
  }
  return 0;
}

