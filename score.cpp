#include <algorithm>
#include <array>
#include "score.hpp"

namespace ai2048 {
namespace score {
constexpr int sorted_weight     =   0;
constexpr int corner_weight     =   0;
constexpr int zero_weight       =   0;
constexpr int same_weight       =   0;
constexpr int movable_weight    =   0;
constexpr int max_weight        =   0;
constexpr int sq_sum_weight     =   0;
constexpr int divided_weight    =   0;
constexpr int max_space_weight  =   0;
constexpr int asi_weight        = 100;

int zero_score(const grid::Grid& grid) {
  int zero_number = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      if (grid[i][j] == 0)
        ++zero_number;
  return zero_number + 1;
}

int movable_score(const grid::Grid& grid) {
  return grid.movable_directions().size();
}

int same_score(const grid::Grid& grid) {
  int score = 0;
  int di[] = {1, 0};
  int dj[] = {0, 1};
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 2; ++k) {
        int ni = i + di[k];
        int nj = j + dj[k];
        if (ni >= 4 || nj >= 4) continue;
        if (grid[i][j] == grid[ni][nj] && grid[i][j] != 0)
          score += grid[i][j];
      }
    }
  }
  return score;
}

int divided_score(const grid::Grid& grid) {
  int sum = grid.sum_tiles();
  int max_patial_sum = 0;
  for (int i = 0; i < 2; ++i) {
    int partial_sum = 0;
    for (int j = 0; j < 4; ++j)
      partial_sum += grid.at(i*3, j);
    max_patial_sum = std::max(max_patial_sum, partial_sum);
    partial_sum = 0;
    for (int j = 0; j < 4; ++j)
      partial_sum += grid.at(j, i*3);
    max_patial_sum = std::max(max_patial_sum, partial_sum);
  }
  if (max_patial_sum == sum) return max_patial_sum;
  return max_patial_sum / (sum - max_patial_sum);
}

std::array<bool, 16> sorted_info(const grid::Grid& grid) {
  std::array<bool, 16> sorted;
  std::fill(std::begin(sorted), std::end(sorted), true);
  for (int i = 0; i < 4; ++i) {
    for (int j = 1; j < 4; ++j) {
      if (grid[i][j] < grid[i][j-1])
        sorted[i] = false;
      else if (grid[i][j] > grid[i][j-1])
        sorted[i+4] = false;
      if (grid[j][i] < grid[j-1][i])
        sorted[i+8] = false;
      else if (grid[j][i] > grid[j-1][i])
        sorted[i+12] = false;
    }
  }
  return sorted;
}

int sorted_score(const grid::Grid& grid) {
  std::array<bool, 16> sorted = sorted_info(grid);
  std::array<int, 16> sorted_sum;
  std::fill(std::begin(sorted_sum), std::end(sorted_sum), 0);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      sorted_sum[i] += grid.at(i, j);
      sorted_sum[i+4] += grid.at(i, j);
      sorted_sum[i+8] += grid.at(j, i);
      sorted_sum[i+12] += grid.at(j, i);
    }
  }
  int score = 0;
  for (int i = 0; i < 4; ++i) {
    if (sorted[i*4]) {
      score += sorted_sum[i*4];
      if (sorted[i*4+1]) {
        score += sorted_sum[i*4+1];
      }
    }
    if (sorted[i*4+3]) {
      score += sorted_sum[i*4+3];
      if (sorted[i*4+2]) {
        score += sorted_sum[i*4+2];
      }
    }
  }
  // return score;
  return std::count(std::begin(sorted), std::end(sorted), true);
}

int max_space_score(const grid::Grid& grid) {
  int max_tile = 0;
  int max_i = 0;
  int max_j = 0;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (grid.at(i, j) > max_tile) {
        max_tile = grid.at(i, j);
        max_i = i;
        max_j = j;
      }
    }
  }
  if ((max_i == 0 || max_i == 3) && (max_j == 0 || max_j == 3)) return 0;
  return -1;
}

int corner_score(const grid::Grid& grid) {
  return grid.at(0, 0) + grid.at(0, 3) + grid.at(3, 0) +grid.at(3, 3)
      - grid.at(1, 1) - grid.at(1, 2) - grid.at(2, 1) - grid.at(2, 2);
}

int asi_score(const grid::Grid& grid) {
  constexpr int vtable[2][4][4] = {
    {
      {80, 40, 40, 50},
      {35, 10, 10, 10},
      { 3,  1,  1,  3},
      { 1,  1,  1,  1}
    },{
      {50, 40, 40, 80},
      {10, 10, 10, 35},
      { 3,  1,  1,  3},
      { 1,  1,  1,  1}
    }
  };
  int vmax = 0;
  for (int i = 0; i < 4; ++i) {
    grid::Grid r_grid = grid.Rotate(i);
    for (int j = 0; j < 2; ++j) {
      int sum = 0;
      for (int k = 0; k < 4; ++k) {
        for (int l = 0; l < 4; ++l) {
          sum += vtable[j][k][l] * grid.at(k, l);
        }
      }
      vmax = std::max(vmax, sum);
    }
  }
  return vmax;
}

int static_score(const grid::Grid& grid) {
  int max_number = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      max_number = std::max(max_number, grid.at(i, j));
  int sum = grid.sum_tiles();
  return sorted_score(grid) * sum * sorted_weight
      + corner_score(grid) * corner_weight
      + same_score(grid) * sum * same_weight
      + std::log(zero_score(grid)) * sum* zero_weight
      - movable_score(grid) * sum * movable_weight
      + max_number * max_weight
      + divided_score(grid) * divided_weight
      + grid.sq_sum_tiles() * sq_sum_weight
      + max_space_score(grid) * sum * max_space_weight
      + asi_score(grid) * asi_weight;
}

int static_score_light(const grid::Grid& grid) {
  int max_number = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      max_number = std::max(max_number, grid.at(i, j));
  return sorted_score(grid) * max_number * sorted_weight
      + corner_score(grid) * corner_weight
      + std::log(zero_score(grid)) * max_number * zero_weight
      + std::sqrt(grid.sq_sum_tiles()) * sq_sum_weight;
}

} // namespace score
} // namespace ai2048
