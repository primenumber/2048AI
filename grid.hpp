#pragma once
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

class Grid {
 public:
  using table_t = std::array<std::array<int, 4>, 4>;
  Grid() = default;
  Grid(const Grid& that) = default;
  Grid(Grid&& that) = default;
  Grid& operator=(const Grid& rhs) = default;
  Grid& operator=(Grid&& rhs) = default;
  Grid rotate(int) const;
  void rotate_this(int);
  bool is_movable(Direction) const;
  std::vector<Direction> movable_directions() const;
  std::vector<std::pair<int, int>> zero_tiles() const;
  Grid move(Direction) const;
  int move_this(Direction);
  int sum_tiles() const;
  int64_t sq_sum_tiles() const;
  table_t table;
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
