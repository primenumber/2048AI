#include <algorithm>
#include <array>
#include "score.hpp"

constexpr int sorted_weight  =  50;
constexpr int corner_weight  =  70;
constexpr int zero_weight    =  70;
constexpr int same_weight    =  50;
constexpr int movable_weight =  30;
constexpr int max_weight     =  20;

int zero_score(const table_t& table) {
  int zero_number = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      if (table[i][j] == 0)
        ++zero_number;
  return zero_number;
}

int movable_score(const table_t& table) {
  int movable_number = 0;
  for (int i = 0; i < 4; ++i)
    if (movable(table, i)) ++movable_number;
  return (movable_number <= 1) ? -1 : 0;
}

int same_score(const table_t& table) {
  int score = 0;
  int di[] = {1, 0};
  int dj[] = {0, 1};
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 2; ++k) {
        int ni = i + di[k];
        int nj = j + dj[k];
        if (ni >= 4 || nj >= 4) continue;
        if (table[i][j] == table[ni][nj])
          score += table[i][j];
      }
    }
  }
  return score;
}

int sorted_score(const table_t& table) {
  std::array<bool, 16> sorted;
  std::array<int, 16> sorted_sum;
  std::fill(std::begin(sorted), std::end(sorted), true);
  std::fill(std::begin(sorted_sum), std::end(sorted_sum), 0);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      sorted_sum[i] += table[i][j];
      sorted_sum[i+4] += table[i][j];
      sorted_sum[i+8] += table[j][i];
      sorted_sum[i+12] += table[j][i];
    }
    for (int j = 1; j < 4; ++j) {
      if (table[i][j] < table[i][j-1])
        sorted[i] = false;
      else if (table[i][j] > table[i][j-1])
        sorted[i+4] = false;
      if (table[j][i] < table[j-1][i])
        sorted[i+8] = false;
      else if (table[j][i] > table[j-1][i])
        sorted[i+12] = false;
    }
  }
  int score = 0;
  for (int i = 0; i < 16; ++i)
    if (sorted[i])
      score += sorted_sum[i];
  return score;
}

int corner_score(const table_t& table) {
  return std::max(table[0][0] ,std::max(table[0][3] ,std::max(table[3][0] ,table[3][3])))
      - std::max(table[1][1] ,std::max(table[1][2] ,std::max(table[2][1], table[2][2])));
}

int static_score(const table_t& table) {
  int max_number = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      max_number = std::max(max_number, table[i][j]);
  return sorted_score(table) * sorted_weight
      + corner_score(table) * corner_weight
      + same_score(table) * same_weight
      + std::log(zero_score(table)) * max_number * zero_weight
      + movable_score(table) * max_number * movable_weight
      + max_number * max_weight;
}

int static_score_light(const table_t& table) {
  int max_number = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      max_number = std::max(max_number, table[i][j]);
  return same_score(table) * same_weight
      + corner_score(table) * corner_weight
      + std::log(zero_score(table)) * max_number * zero_weight;
}
