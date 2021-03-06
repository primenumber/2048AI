#include <map>
#include "grid.hpp"
#include "random.hpp"
#include "score.hpp"

namespace ai2048 {
namespace search {

struct Value {
  Value(int vmin, int vmax) : value_min(vmin), value_max(vmax) {};
  constexpr static int kValueMin = -1000000000;
  constexpr static int kValueMax = 1000000000;
  int value_min;
  int value_max;
};

int search(const grid::Grid& grid);
grid::Direction opti(const grid::Grid& grid);

std::array<int, 4> simple_search(const grid::Grid& grid);
std::array<double, 4> great_search(const grid::Grid& grid);
std::array<int, 4> alpha_beta_search(const grid::Grid& grid);
std::array<double, 4> Monte_Carlo_search(const grid::Grid& grid);

} // namespace search
} // namespace ai2048
