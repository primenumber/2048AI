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

constexpr std::array<Direction, 4> directions = {UP, RIGHT, DOWN, LEFT};

class Grid {
 public:
  using table_t = std::array<std::array<int, 4>, 4>;
  Grid() = default;
  Grid(const Grid& that) = default;
  Grid(Grid&& that) = default;
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
};

bool operator<(const Grid& lhs, const Grid& rhs);


} // namespace grid
} // namespace ai2048
