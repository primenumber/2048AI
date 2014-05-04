#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>
#include "grid.hpp"
#include "score.hpp"
#include "search.hpp"

/*
int calc(const table_t& table, int direction, int depth, int alpha, int beta) {
  table_t moved = move(table, direction);
  if (depth == 0) return static_score(moved);
  auto zeros = zero_list(moved);
  for (auto& point : zeros) {
    int i = point.first;
    int j = point.second;
    int numbers[] = {2, 4};
    for (int k = 0; k < 2; ++k) {
      moved[i][j] = numbers[k];
      int local_alpha = alpha;
      int local_beta = beta;
      for (int l = 0; l < 4; ++l) {
        if (!movable(moved, l)) continue;
        int score = calc(moved, l, depth - 1, local_alpha, local_beta);
        local_alpha = std::max(local_alpha, score);
        if (local_alpha >= local_beta) {
          local_alpha = local_beta;
          break;
        }
      }
      beta = std::min(beta, local_alpha);
      if (alpha >= beta) return alpha;
    }
  }
  return beta;
}
*/

int main() {
  table_t table;
  for (auto& a : table) {
    for (auto& e : a) {
      std::cin >> e;
    }
  }
  std::cout << search(table) << std::endl;
  /*
  int max_i = -1;
  int alpha = -1000000000;
  int beta = 1000000000;
  const int kDEPTH = 2;
  for (int i = 0; i < 4; ++i) {
    if (!movable(table, i)) continue;
    int score = calc(table, i, kDEPTH, alpha, beta);
    if (alpha < score) {
      alpha = score;
      max_i = i;
    }
  }
  if (max_i == -1) {
    for (int i = 0; i < 4; ++i) {
      if (movable(table, i)) max_i = i;
    }
  }
  std::cout << max_i << std::endl;
  */
  return 0;
}

