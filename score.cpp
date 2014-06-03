#include <algorithm>
#include <array>
#include "score.hpp"

namespace ai2048 {
namespace score {

int asi_score(const grid::Grid& grid) {
  constexpr int vtable[2][16] = {
    {
      80, 40, 40, 50,
      35, 10, 10, 10,
       3,  1,  1,  3,
       1,  1,  1,  1
    },{
      50, 40, 40, 80,
      10, 10, 10, 35,
       3,  1,  1,  3,
       1,  1,  1,  1
    }
  };
  int vmax = 0;
  for (int i = 0; i < 4; ++i) {
    grid::Grid r_grid = grid.Rotate(i);
    for (int j = 0; j < 2; ++j) {
      int sum = 0;
      for (int k = 0; k < 16; ++k) {
        sum += vtable[j][k] << r_grid.tiles[k];
      }
      vmax = std::max(vmax, sum);
    }
  }
  return vmax;
}

int static_score(const grid::Grid& grid) {
  return asi_score(grid);
}

} // namespace score
} // namespace ai2048
