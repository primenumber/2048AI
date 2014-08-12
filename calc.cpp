#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "grid.hpp"

uint32_t to24bit(ai2048::grid::Grid grid) {
  uint32_t result = 0;
  for (int i = 0; i < 8; ++i) {
    result |= (grid.tiles[i + 8] & 0x7) << (i * 3);
  }
  return result;
}

ai2048::grid::Grid toGrid(uint32_t bits) {
  ai2048::grid::Grid grid;
  for (int j = 0; j < 8; ++j) {
    grid.tiles[j + 8] = (bits >> (j * 3)) & 0x7;
  }
  return grid;
}

bool is_in_24bits(ai2048::grid::Grid grid) {
  for (int i = 0; i < 8; ++i)
    if (grid.tiles[i + 8] >= 8) return false;
  return true;
}

std::vector<ai2048::grid::Direction> GetMovable(ai2048::grid::Grid grid) {
  using namespace ai2048::grid;
  using std::begin;
  using std::end;
  auto movable = grid.movable_directions();
  auto mitr = remove_if(begin(movable), end(movable),
      [=] (Direction dir) {
        return dir == 0 || !is_in_24bits(grid.move(dir));
      });
  movable.erase(mitr, end(movable));
  return movable;
}

std::vector<int> GetZeros(ai2048::grid::Grid grid) {
  using namespace ai2048::grid;
  using std::begin;
  using std::end;
  std::vector<int> result;
  auto zeros = grid.zero_tiles();
  auto zitr = remove_if(begin(zeros), end(zeros),
      [] (std::pair<int, int> p) { return p.first < 2; });
  zeros.erase(zitr, end(zeros));
  for (auto& p : zeros) result.push_back(p.first * 4 + p.second);
  return result;
}

bool is_ok(ai2048::grid::Grid grid, int i, int v) {
  return grid.tiles[12 + i] == v;
}

void search(std::vector<double>& prob, std::vector<int>& dirs, uint32_t index, int i, int v) {
  using std::vector;
  using namespace ai2048::grid;
  Grid grid = toGrid(index);
  if (is_ok(grid, i, v)) {
    prob[index] = 1.0;
    dirs[index] = 2;
    return;
  }
  auto movable = GetMovable(grid);
  if (movable.empty()) {
    prob[index] = 0.0;
    dirs[index] = 0;
    return;
  }
  double max_prob = 0.0;
  int opt = 0;
  for (Direction dir : movable) {
    Grid moved = grid.move(dir);
    double sum_prob = 0.0;
    auto zeros = GetZeros(moved);
    for (int p : zeros) {
      std::array<int, 2> nums = {{1, 2}};
      for (int num : nums) {
        moved.tiles[p] = num;
        uint32_t bits = to24bit(moved);
        if (prob[bits] < 0.0)
          search(prob, dirs, bits, i, v);
        if (num == 1)
          sum_prob += prob[bits] * 0.9;
        else
          sum_prob += prob[bits] * 0.1;
      }
      moved.tiles[p] = 0;
    }
    if (max_prob < sum_prob / zeros.size()) {
      max_prob = sum_prob / zeros.size();
      opt = dir;
    }
  }
  prob[index] = max_prob;
  dirs[index] = opt;
}

int main() {
  ai2048::grid::Line::Init();
  using namespace ai2048::grid;
  using std::begin;
  using std::end;
  for (int i = 0; i < 4; ++i) {
    for (int v = 1; v < 8; ++v) {
      std::cout << "pos: " << i << ", val: " << v << std::endl;
      std::fstream ofs;
      std::stringstream ss;
      ss << "data/data_2048_" << i << "_" << v << ".txt";
      ofs.open(ss.str(), std::ios::out);
      std::vector<double> points(1 << 24, -1.0);
      std::vector<int> dirs(1 << 24);
      for (int j = 0; j < (1 << 24); ++j) {
        if (points[j] < 0.0)
          search(points, dirs, j, i, v);
        ofs << std::setprecision(4) << std::fixed << points[j] << ' ' << dirs[j] << '\n';
      }
      ofs.close();
    }
  }
  return 0;
}
