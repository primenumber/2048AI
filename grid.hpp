#pragma once
#include <iostream>
#include <algorithm>
#include <array>
#include <utility>
#include <vector>
#include <sstream>

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

constexpr uint32_t MinimumLineBits = 20;
constexpr uint32_t LineArraySize = 1 << MinimumLineBits;

union Line {
 public:
  //member values
  uint32_t data;
  std::array<uint8_t, 4> tiles;
  //member functions
  Line() : data(0) {}
  Line(const uint32_t data_) : data(data_) {}
  Line(const std::array<uint8_t, 4> tiles_) : tiles(tiles_) {}
  const uint8_t& operator[](const int index) const { return tiles[index]; }
  uint8_t& operator[](const int index) { return tiles[index]; }
  int at(const int index) const {
    return value_table[tiles[index]];
  }
  int sum() const {
    return sum_array[to20bit(data)];
  }
  int estimate_score() const {
    return estimscr_array[to20bit(data)];
  }
  void set(const int index, int value) {
    tiles[index] = std::find(std::begin(value_table), std::end(value_table), value) - std::begin(value_table);
  }
  void reverse() {
    data = (data >> 16) | (data << 16);
    data = ((data & 0xFF00FF00) >> 8) | ((data & 0x00FF00FF) << 8);
  }
  Line& move() {
    return *this = move_array[to20bit(data)];
  }
  bool is_movable() const {
    return movable_array[to20bit(data)];
  }
  std::string toString() const {
    std::stringstream ss;
    ss << '[';
    for (int i = 0; i < 4; ++i) {
      if (i) ss << ", ";
      ss << at(i);
    }
    ss << ']';
    return ss.str();
  }
  //static values
  static std::vector<Line> move_array;
  static std::vector<bool> movable_array;
  static std::vector<int> sum_array;
  static std::vector<int> estimscr_array;
  //static functions
  static void Init();
  static Line make_from_20bit(const uint32_t data_) {
    return Line(to32bit(data_));
  }
  static uint32_t to20bit(uint32_t data_) {
    data_ = ((data_ & 0xFF00FF00) >> 3) | (data_ & 0x00FF00FF);
    return ((data_ & 0xFFFF0000) >> 6) | (data_ & 0x0000FFFF);
  }
  static uint32_t to32bit(uint32_t data_) {
    data_ = ((data_ & 0x000FFC00) << 6) | (data_ & 0x000003FF);
    return ((data_ & 0x03E003E0) << 3) | (data_ & 0x001F001F);
  }
  static uint32_t convertEndian(uint32_t data_) {
    data_ = ((data_ & 0xFFFF0000) >> 16) | ((data_ & 0x0000FFFF) << 16);
    data_ = ((data_ & 0xFF00FF00) >> 8) | ((data_ & 0x00FF00FF) << 8);
    return data_;
  }
  template<typename T>
  static std::vector<T> MakeLineTable(const std::function<T(Line)>& func) {
    std::vector<T> result(LineArraySize);
    int i(0);
    std::generate(std::begin(result), std::end(result), [&]{ return func(make_from_20bit(i++)); });
    return result;
  }
 private:
  Line& move_impl();
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
  int estimate_score() const;
  int64_t sq_sum_tiles() const;
  Grid& Transpose();
  Grid& FlipHorizontal();
  Grid& FlipVertical();
 private:
  bool is_movable_left() const;
  Grid move_left() const;
};

bool operator<(const Grid&, const Grid&);
bool operator==(const Line&, const Line&);
bool operator!=(const Line&, const Line&);
bool operator==(const Grid&, const Grid&);

} // namespace grid
} // namespace ai2048

namespace std {

template<>
class hash<ai2048::grid::Grid> {
 public:
  size_t operator()(const ai2048::grid::Grid& grid) const {
    return grid.double_lines[0] ^ grid.double_lines[1];
  }
};

} // namespace std
