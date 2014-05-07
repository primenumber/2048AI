#include <array>
#include <utility>
#include <vector>

namespace ai2048.grid {

enum Direction : int {
  UP = 0,
  RIGHT,
  DOWN,
  LEFT,
  NONE
};

class Grid {
 public:
  using table_t = std::array<std::array<int, 4>, 4>;
  Grid(const Grid& that) = default;
  Grid(Grid&& that) = default;
  Grid rotate(Direction) const;
  void rotate_this(Direction);
  bool is_movable(Direction) const;
  std::vector<int> movable_directions() const;
  std::vector<std::pair<int, int>> zero_tiles() const;
  Grid move(Direction) const;
  int move(Direction);
  table_t table;
};

} // namespace ai2048.grid

table_t rotate(table_t table, int direction);
bool movable(const table_t& table, int direction);
table_t move(const table_t& table, int direction);
std::vector<std::pair<int, int>> zero_list(const table_t& table);
int sum_grid(const table_t& table);
int64_t sq_sum_grid(const table_t& table);
