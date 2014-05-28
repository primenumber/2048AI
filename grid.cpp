#include "grid.hpp"

namespace ai2048 {
namespace grid {

std::array<Line, (1 << 20)> Line::move_array;
std::array<bool, (1 << 20)> Line::movable_array;

void Line::Init() {
  for (int i = 0; i < (1 << 20); ++i) {
    Line c_line(make_from_20bit(i));
    move_array[i] = c_line.move_impl();
    movable_array[i] = (make_from_20bit(i) != move_array[i]);
  }
}

Line& Line::move_impl() {
  bool joined = false;
  bool movable = false;
  for (int j = 1; j < 4; ++j) {
    if (tiles[j] == 0) continue;
    int k;
    for (k = j - 1; k >= 0; --k) {
      if (tiles[k] != 0) {
        if (tiles[k] != tiles[k+1] || joined) {
          joined = false;
        } else {
          ++tiles[k];
          tiles[k+1] = 0;
          joined = true;
          movable = true;
        }
        break;
      } else {
        tiles[k] = tiles[k+1];
        tiles[k+1] = 0;
        movable = true;
      }
    }
    if (k < 0) joined = false;
  }
  return *this;
}

// public functions
int Grid::at(const int i, const int j) const {
  int value = 1 << tiles[i * 4 + j];
  return value > 1 ? value : 0;
}

uint8_t Grid::at_raw(const int i, const int j) const {
  return tiles[i * 4 + j];
}

Grid Grid::Rotate(int rotation) const {
  return Grid(*this).RotateThis(rotation);
}

Grid& Grid::RotateThis(int rotation) {
  switch(rotation) {
   case 1:
    FlipHorizontal();
    Transpose();
    break;
   case 2:
    FlipHorizontal();
    FlipVertical();
    break;
   case 3:
    Transpose();
    FlipHorizontal();
    break;
  }
  return *this;
}

bool Grid::is_movable(Direction direction) const {
  Grid c_grid = Rotate((direction + 1) % 4);
  return c_grid.is_movable_left();
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
      if (at_raw(i, j) == 0)
        results.emplace_back(i, j);
  return results;
}

Grid Grid::move(Direction direction) const {
  Grid c_grid = Rotate((direction + 1) % 4);
  c_grid = c_grid.move_left();
  c_grid.RotateThis((3 - direction) % 4);
  return c_grid;
}

int Grid::sum_tiles() const {
  int sum = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      sum += at(i, j);
  return sum;
}

int64_t Grid::sq_sum_tiles() const {
  int64_t sum = 0;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      sum += (int64_t)at(i, j) * at(i, j);
  return sum;
}

// for Little-endian
Grid& Grid::Transpose() {
  double_lines[0] = (double_lines[0] & UINT64_C(0xFF00FF0000FF00FF))
      | ((double_lines[0] & UINT64_C(0x00FF00FF00000000)) >> 24)
      | ((double_lines[0] & UINT64_C(0x00000000FF00FF00)) << 24);
  double_lines[1] = (double_lines[1] & UINT64_C(0xFF00FF0000FF00FF))
      | ((double_lines[1] & UINT64_C(0x00FF00FF00000000)) >> 24)
      | ((double_lines[1] & UINT64_C(0x00000000FF00FF00)) << 24);
  uint64_t tmp = double_lines[0] & UINT64_C(0xFFFF0000FFFF0000);
  double_lines[0] = (double_lines[0] & UINT64_C(0x0000FFFF0000FFFF))
      | ((double_lines[1] & UINT64_C(0x0000FFFF0000FFFF)) << 16);
  double_lines[1] = (double_lines[1] & UINT64_C(0xFFFF0000FFFF0000))
      | (tmp >> 16);
  return *this;
}

Grid& Grid::FlipHorizontal() {
  double_lines[0] = ((double_lines[0] & UINT64_C(0xFFFF0000FFFF0000)) >> 16)
      | ((double_lines[0] & UINT64_C(0x0000FFFF0000FFFF)) << 16);
  double_lines[0] = ((double_lines[0] & UINT64_C(0xFF00FF00FF00FF00)) >> 8)
      | ((double_lines[0] & UINT64_C(0x00FF00FF00FF00FF)) << 8);
  double_lines[1] = ((double_lines[1] & UINT64_C(0xFFFF0000FFFF0000)) >> 16)
      | ((double_lines[1] & UINT64_C(0x0000FFFF0000FFFF)) << 16);
  double_lines[1] = ((double_lines[1] & UINT64_C(0xFF00FF00FF00FF00)) >> 8)
      | ((double_lines[1] & UINT64_C(0x00FF00FF00FF00FF)) << 8);
  return *this;
}

Grid& Grid::FlipVertical() {
  std::swap(lines[0], lines[3]);
  std::swap(lines[1], lines[2]);
  return *this;
}

// private methods
bool Grid::is_movable_left() const {
  for (const Line& line : lines) {
    if (line.is_movable())
      return true;
  }
  return false;
}

Grid Grid::move_left() const {
  Grid cp(*this);
  for (Line& line : cp.lines) {
    line.move();
  }
  return cp;
}

// grobal functions
bool operator<(const Grid& lhs, const Grid& rhs) {
  return lhs.tiles < rhs.tiles;
}

bool operator==(const Line& lhs, const Line& rhs) {
  return lhs.data == rhs.data;
}

bool operator!=(const Line& lhs, const Line& rhs) {
  return lhs.data != rhs.data;
}

bool operator==(const Grid& lhs, const Grid& rhs) {
  return lhs.tiles == rhs.tiles;
}

} // namespace grid
} // namespace ai2048
