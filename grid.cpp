#include "grid.hpp"

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
