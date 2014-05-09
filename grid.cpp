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
  switch (direction) {
    case UP: return is_movable_up();
    case RIGHT: return is_movable_right();
    case DOWN: return is_movable_down();
    case LEFT: return is_movable_left();
    default: return false;
  }
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
  switch (direction) {
    case 0: return this->move_up();
    case 1: return this->move_right();
    case 2: return this->move_down();
    case 3: return this->move_left();
    default: return *this;
  }
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

// private methods
bool Grid::is_movable_up() const {
  for (int i = 0; i < 4; ++i) {
    for (int j = 1; j < 4; ++j) {
      if (table[j][i] == 0) continue;
      if (table[j][i] == table[j-1][i] || table[j-1][i] == 0)
        return true;
    }
  }
  return false;
}

bool Grid::is_movable_right() const {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (table[i][j] == 0) continue;
      if (table[i][j] == table[i][j+1] || table[i][j+1] == 0)
        return true;
    }
  }
  return false;
}

bool Grid::is_movable_down() const {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (table[j][i] == 0) continue;
      if (table[j][i] == table[j+1][i] || table[j+1][i] == 0)
        return true;
    }
  }
  return false;
}

bool Grid::is_movable_left() const {
  for (int i = 0; i < 4; ++i) {
    for (int j = 1; j < 4; ++j) {
      if (table[i][j] == 0) continue;
      if (table[i][j] == table[i][j-1] || table[i][j-1] == 0)
        return true;
    }
  }
  return false;
}

Grid Grid::move_up() const {
  Grid c_grid = Grid(*this);
  bool joined = false;
  for (int i = 0; i < 4; ++i) {
    for (int j = 1; j < 4; ++j) {
      int k;
      for (k = j - 1; k >= 0; --k) {
        if (c_grid.table[k][i] != 0) {
          if (c_grid.table[k][i] != c_grid.table[k+1][i] || joined) {
            joined = false;
            break;
          } else {
            c_grid.table[k][i] *= 2;
            c_grid.table[k+1][i] = 0;
            joined = true;
            break;
          }
        } else {
          c_grid.table[k][i] = c_grid.table[k+1][i];
          c_grid.table[k+1][i] = 0;
        }
      }
      if (k < 0) joined = false;
    }
  }
  return c_grid;
}

Grid Grid::move_right() const {
  Grid c_grid = Grid(*this);
  bool joined = false;
  for (int i = 0; i < 4; ++i) {
    for (int j = 2; j >= 0; --j) {
      int k;
      for (k = j + 1; k < 4; ++k) {
        if (c_grid.table[i][k] != 0) {
          if (c_grid.table[i][k] != c_grid.table[i][k-1] || joined) {
            joined = false;
            break;
          } else {
            c_grid.table[i][k] *= 2;
            c_grid.table[i][k-1] = 0;
            joined = true;
            break;
          }
        } else {
          c_grid.table[i][k] = c_grid.table[i][k-1];
          c_grid.table[i][k-1] = 0;
        }
      }
      if (k >= 4) joined = false;
    }
  }
  return c_grid;
}

Grid Grid::move_down() const {
  Grid c_grid = Grid(*this);
  bool joined = false;
  for (int i = 0; i < 4; ++i) {
    for (int j = 2; j >= 0; --j) {
      int k;
      for (k = j + 1; k < 4; ++k) {
        if (c_grid.table[k][i] != 0) {
          if (c_grid.table[k][i] != c_grid.table[k-1][i] || joined) {
            joined = false;
            break;
          } else {
            c_grid.table[k][i] *= 2;
            c_grid.table[k-1][i] = 0;
            joined = true;
            break;
          }
        } else {
          c_grid.table[k][i] = c_grid.table[k-1][i];
          c_grid.table[k-1][i] = 0;
        }
      }
      if (k >= 4) joined = false;
    }
  }
  return c_grid;
}

Grid Grid::move_left() const {
  Grid c_grid = Grid(*this);
  bool joined = false;
  for (int i = 0; i < 4; ++i) {
    for (int j = 1; j < 4; ++j) {
      int k;
      for (k = j - 1; k >= 0; --k) {
        if (c_grid.table[i][k] != 0) {
          if (c_grid.table[i][k] != c_grid.table[i][k+1] || joined) {
            joined = false;
            break;
          } else {
            c_grid.table[i][k] *= 2;
            c_grid.table[i][k+1] = 0;
            joined = true;
            break;
          }
        } else {
          c_grid.table[i][k] = c_grid.table[i][k+1];
          c_grid.table[i][k+1] = 0;
        }
      }
      if (k < 0) joined = false;
    }
  }
  return c_grid;
}

// grobal functions
bool operator<(const Grid& lhs, const Grid& rhs) {
  return lhs.table < rhs.table;
}

} // namespace grid
} // namespace ai2048
