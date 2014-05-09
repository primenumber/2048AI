#include "grid.hpp"

namespace ai2048 {
namespace grid {

Grid Grid::rotate(int rotation) const {
  if (rotation == 0) return *this;
  Grid result = *this;
  switch (rotation) {
   case 2:
   case 3:
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 2; ++j) {
        std::swap(result.table[i][j], result.table[3-i][3-j]);
        std::swap(result.table[j][3-i], result.table[3-j][i]);
      }
    }
    if (rotation == 2) break;
   case 1:
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 2; ++j) {
        int tmp = result.table[i][j];
        result.table[i][j] = result.table[j][3-i];
        result.table[j][3-i] = result.table[3-i][3-j];
        result.table[3-i][3-j] = result.table[3-j][i];
        result.table[3-j][i] = tmp;
      }
    }
    break;
  }
  return result;
}

bool Grid::is_movable(Direction direction) const {
  Grid r_grid = this->rotate(direction);
  for (int i = 0; i < 4; ++i) {
    for (int j = 1; j < 4; ++j) {
      if (r_grid.table[j][i] == 0) continue;
      if (r_grid.table[j][i] == r_grid.table[j-1][i] || r_grid.table[j-1][i] == 0)
        return true;
    }
  }
  return false;
}

std::vector<Direction> Grid::movable_directions() const {
  std::vector<Direction> results;
  for (int i : directions)
    if (this->is_movable(i))
      results.push_back(i);
  return results;
}

std::vector<std::pair<int, int>> Grid::zero_tiles() const {
  std::vector<std::pair<int, int>> results;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      if (table[i][j] == 0)
        results.emplace_back(i, j);
  return results;
}

Grid Grid::move(Direction direction) const {
  Grid r_grid = this->rotate(direction);
  bool joined = false;
  for (int i = 0; i < 4; ++i) {
    for (int j = 1; j < 4; ++j) {
      int k;
      for (k = j - 1; k >= 0; --k) {
        if (r_grid.table[k][i] != 0) {
          if (r_grid.table[k][i] != r_grid.table[k+1][i] || joined) {
            joined = false;
            break;
          } else {
            r_grid.table[k][i] *= 2;
            r_grid.table[k+1][i] = 0;
            joined = true;
            break;
          }
        } else {
          r_grid.table[k][i] = r_grid.table[k+1][i];
          r_grid.table[k+1][i] = 0;
        }
      }
      if (k < 0) joined = false;
    }
  }
  return r_grid;
}

int Grid::sum_tiles() const {
  int sum = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      sum += table[i][j];
  return sum;
}

int64_t Grid::sq_sum_tiles() const {
  int64_t sum = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      sum += (int64_t)table[i][j] * table[i][j];
  return sum;
}

bool operator<(const Grid& lhs, const Grid& rhs) {
  return lhs.table < rhs.table;
}

} // namespace grid
} // namespace ai2048
