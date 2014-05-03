#include <algorithm>
#include <array>
#include "score.hpp"

constexpr int sorted_weight  = 100;
constexpr int corner_weight  = 100;
constexpr int zero_weight    =  10;
constexpr int same_weight    =  15;
constexpr int movable_weight =  50;

int static_score(const table_t& table) {
  int zero_number = 0;
  int same_tile_number = 0;
  int max_number = 0;
  int movable_number = 0;
  for (int i = 0; i < 4; ++i) {
    if (movable(table, i)) ++movable_number;
  }
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
  int sorted_score = 0;
  for (int i = 0; i < 16; ++i)
    if (sorted[i])
      sorted_score += sorted_sum[i];
  int corner_score = table[0][0] + table[0][3] + table[3][0] + table[3][3];
  int movable_score = 0;
  if (movable_number <= 1)
    movable_score = max_number;
  return sorted_score * sorted_weight
      + corner_score * corner_weight
      + same_tile_number * max_number * same_weight
      + zero_number * max_number * zero_weight
      + movable_score * movable_weight;
}
