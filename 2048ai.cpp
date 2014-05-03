#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>
#include "grid.hpp"

int static_score(const table_t& table) {
  int zero_number = 0;
  int same_tile_number = 0;
  int max_number = 0;
  std::array<bool, 16> sorted;
  std::array<int, 16> sorted_sum;
  std::fill(std::begin(sorted), std::end(sorted), true);
  std::fill(std::begin(sorted_sum), std::end(sorted_sum), 0);
  int di[] = {-1, 0, 1, 0};
  int dj[] = {0, 1, 0, -1};
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      max_number = std::max(max_number, table[i][j]);
      if (table[i][j] == 0)
        ++zero_number;
      for (int k = 0; k < 4; ++k) {
        int ni = i + di[k];
        int nj = j + dj[k];
        if (ni < 0 || ni >= 4 || nj < 0 || nj >= 4) continue;
        if (table[i][j] == table[ni][nj])
          ++same_tile_number;
      }
      sorted_sum[i] += table[i][j];
      sorted_sum[i+4] += table[i][j];
      sorted_sum[i+8] += table[j][i];
      sorted_sum[i+12] += table[j][i];
    }
    for (int j = 1; j < 4; ++j) {
      if (table[i][j] < table[i][j-1])
        sorted[i] = false;
      if (table[i][j] > table[i][j-1])
        sorted[i+4] = false;
      if (table[j][i] < table[j-1][i])
        sorted[i+8] = false;
      if (table[j][i] > table[j-1][i])
        sorted[i+12] = false;
    }
  }
  int sum = 0;
  for (int i = 0; i < 16; ++i)
    if (sorted[i])
      sum += sorted_sum[i];
  sum += table[0][0] + table[0][3] + table[3][0] + table[3][3];
  return sum * 100 + same_tile_number * max_number * 15 + zero_number * max_number * 10;
}

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

