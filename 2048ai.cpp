#include <chrono>
#include <iostream>
#include <memory>
#include "grid.hpp"
#include "random.hpp"
#include "search.hpp"
#include "gamedag.hpp"

void init() {
  init_random();
  ai2048::grid::Line::Init();
  ai2048::score::Init();
}

int main() {
  using namespace ai2048::grid;
  init();
  while (true) {
    ai2048::grid::Grid grid;
    for (auto& a : grid.lines) {
      for (int i = 0; i < 4; ++i) {
        int v;
        std::cin >> v;
        a.set(i, v);
      }
    }
    ai2048::grid::Direction move = ai2048::search::opti(grid);
    std::cout << "{\"type\":\"move\",\"direction\":"<< move <<"}"<< std::endl;
  }
  return 0;
    //auto movable = grid.movable_directions();
    //if (movable.empty()) break;
    //ai2048::grid::Direction move;// = ai2048::search::alpha_beta_search(grid);
    //std::shared_ptr<ai2048::gamedag::GameDAG> root(
        //std::make_shared<ai2048::gamedag::GameDAG>(
            //ai2048::game::Game(grid, ai2048::game::State::Player)));
    //std::map<ai2048::game::Game, std::shared_ptr<ai2048::gamedag::GameDAG>> m;
    //for (int d = 5; d < 40; ++d) {
      //auto start = std::chrono::system_clock::now();
      //move = root->getOptimal(m, d);
      //auto end = std::chrono::system_clock::now();
      //auto elapsed = end - start;
      //if (elapsed.count() > 50000) {
        //std::cout << "{\"type\":\"debug\",\"score\":" << d << "}" << std::endl;
        //break;
      //}
    //}
    //std::cout << "{\"type\":\"move\",\"direction\":"<< move <<"}"<< std::endl;
  //}
  //return 0;
}

