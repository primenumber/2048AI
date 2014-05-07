#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>
#include "grid.hpp"
#include "score.hpp"
#include "search.hpp"
#include "random.hpp"

int main() {
  init_random();
  table_t table;
  for (auto& a : table) {
    for (auto& e : a) {
      std::cin >> e;
    }
  }
  int move = search(table);
  std::cout << "{\"type\":\"move\",\"direction\":"<< move <<"}"<< std::endl;
  return 0;
}

