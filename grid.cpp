#include "grid.hpp"

namespace ai2048.grid {

Grid Grid::rotate(int rotation) const {
  if (rotation == 0) return Grid(*this);
  Grid result = this->rotate(rotation - 1);
  switch(rotation) {
   case 2:
   case 3:
    if (rotation == 2) break;
   case 1:
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 2; ++j) {
        int tmp = result[i][j];
        result[i][j] = result[j][3-i];
        result[j][3-i] = result[3-i][3-j];
        result[3-i][3-j] = result[3-j][i];
        result[3-j][i] = tmp;
      }
    }
    break;
  }
  return result;
}

void Grid::rotate_this(int rotation) {
  if (rotation != 0) *this = this->rotate(rotation);
}

bool Grid::is_movable(Direction direction) const {
  Grid r_grid = this->rotate(direction);
  for (int i = 0; i < 4; ++i) {
    for (int j = 1; j < 4; ++j) {
      if (r_grid.table[j][i] == 0) continue;
      if (r_grid.table[j][i] == r_grid.table[j][j-1][i] || r_grid.table[j][j-1][i] == 0)
        return true;
    }
  }
  return false;
}

std::vector<Direction> Grid::movable_directions() const {
  std::vector<Direction> results;
  for (Direction i = UP, i < NONE; ++i)
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

Grid Grid::move(Direction) const {
  ;
}

} // namespace ai2048.grid

table_t rotate(table_t table, int direction) {
  for (int i = 0; i < direction; ++i) {
    table_t tmp;
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        tmp[j][k] = table[k][4-j-1];
      }
    }
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        table[j][k] = tmp[j][k];
      }
    }
  }
  return table;
}

bool movable(const table_t& table, int direction) {
  table_t r_table = rotate(table, direction);
  for (int i = 0; i < 4; ++i) {
    for (int j = 1; j < 4; ++j) {
      if (r_table[j][i] == 0) continue;
      if (r_table[j][i] == r_table[j-1][i] || r_table[j-1][i] == 0)
        return true;
    }
  }
  return false;
}

table_t move(const table_t& table, int direction) {
  table_t r_table = rotate(table, direction);
  bool joined = false;
  for (int i = 0; i < 4; ++i) {
    for (int j = 1; j < 4; ++j) {
      int k;
      for (k = j - 1; k >= 0; --k) {
        if (r_table[k][i] != 0) {
          if (r_table[k][i] != r_table[k+1][i] || joined) {
            joined = false;
            break;
          } else {
            r_table[k][i] *= 2;
            r_table[k+1][i] = 0;
            joined = true;
            break;
          }
        } else {
          r_table[k][i] = r_table[k+1][i];
          r_table[k+1][i] = 0;
        }
      }
      if (k < 0) joined = false;
    }
  }
  return rotate(r_table, 4-direction-1);
}

std::vector<std::pair<int, int>> zero_list(const table_t& table) {
  std::vector<std::pair<int, int>> results;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (table[i][j] == 0) {
        results.emplace_back(i,j);
      }
    }
  }
  return results;
}

int sum_grid(const table_t& table) {
  int sum = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      sum += table[i][j];
  return sum;
}

int64_t sq_sum_grid(const table_t& table) {
  int64_t sum = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      sum += (int64_t)table[i][j] * table[i][j];
  return sum;
}
