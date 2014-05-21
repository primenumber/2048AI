#pragma once
#include <algorithm>
#include <array>
#include <utility>
#include <vector>

namespace ai2048 {
namespace grid {

using Direction = int;

constexpr Direction UP = 0; 
constexpr Direction RIGHT = 1;
constexpr Direction DOWN = 2;
constexpr Direction LEFT = 3;

constexpr std::array<Direction, 4> directions = {{UP, RIGHT, DOWN, LEFT}};

constexpr std::array<int, 18> value_table = {{
  0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072
}};

union Line {
 public:
  //member values
  uint32_t data;
  std::array<uint8_t, 4> tiles;
  //member functions
  Line(const int32_t data_) : data(data_) {}
  Line(const std::array<uint8_t, 4> tiles_) : tiles(tiles_) {}
  const uint8_t& operator[](const int index) const { return tiles[index]; }
  uint8_t& operator[](const int index) { return tiles[index]; }
  int at(const int index) const {
    int value = 1 << tiles[index];
    return value > 1 ? value : 0;
  }
  void set(const int index, int value) {
    tiles[index] = std::find(std::begin(value_table), std::end(value_table), value) - std::begin(value_table);
  }
  void reverse() {
    data = (data >> 16) | (data << 16);
    data = ((data & 0xFF00FF00) >> 8) | ((data & 0x00FF00FF) << 8);
  }
};

union Grid {
 public:
  std::array<Line, 4> lines;
  std::array<uint8_t, 16> tiles;
  std::array<uint64_t, 2> double_lines;
  Grid() : double_lines{{0,0}} {}
  Grid(const Grid& that) = default;
  Grid(Grid&& that) = default;
  Grid& operator=(const Grid& rhs) = default;
  Grid& operator=(Grid&& rhs) = default;
  int at(const int i, const int j) const;
  uint8_t at_raw(const int i, const int j) const;
  Line& operator[](const int index) { return lines[index]; }
  const Line& operator[](const int index) const { return lines[index]; }
  Grid Rotate(int) const;
  Grid& RotateThis(int);
  bool is_movable(Direction) const;
  std::vector<Direction> movable_directions() const;
  std::vector<std::pair<int, int>> zero_tiles() const;
  Grid move(Direction) const;
  int move_this(Direction);
  int sum_tiles() const;
  int64_t sq_sum_tiles() const;
  Grid& Transpose();
  Grid& FlipHorizontal();
  Grid& FlipVertical();
 private:
  bool is_movable_up() const;
  bool is_movable_right() const;
  bool is_movable_down() const;
  bool is_movable_left() const;
  Grid move_up() const;
  Grid move_right() const;
  Grid move_down() const;
  Grid move_left() const;
};

bool operator<(const Grid&, const Grid&);
bool operator==(const Grid&, const Grid&);


} // namespace grid
} // namespace ai2048
