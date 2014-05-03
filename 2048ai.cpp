#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>
#include "grid.hpp"
#include "score.hpp"

int calc(const table_t& table, int direction, int depth) {
  table_t moved = move(table, direction);
  if (depth == 0) return static_score(moved);
  auto zeros = zero_list(moved);
  int minscore = 1000000000;
  for (auto& point : zeros) {
    int i = point.first;
    int j = point.second;
    int numbers[] = {2, 4};
    for (int k = 0; k < 2; ++k) {
      moved[i][j] = numbers[k];
      int maxscore = -1000000000;
      for (int l = 0; l < 4; ++l) {
        if (!movable(moved, l)) continue;
        int score = calc(moved, l, depth - 1);
        maxscore = std::max(maxscore, score);
      }
      minscore = std::min(minscore, maxscore);
    }
  }
  return minscore;
}

int main() {
  table_t table;
  for (auto& a : table) {
    for (auto& e : a) {
      std::cin >> e;
    }
  }
  int max_score = -1000000000;
  int max_i = -1;
  for (int i = 0; i < 4; ++i) {
    if (!movable(table, i)) continue;
    int score = calc(table, i, 2);
    if (max_score <= score) {
      max_score = score;
      max_i = i;
    }
  }
  std::cout << max_i << std::endl;
  return 0;
}

