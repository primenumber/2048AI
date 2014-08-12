#include "grid.hpp"
#include "search.hpp"

namespace ai2048 {
namespace search {

bool is_fixed_grid(grid::Grid grid) {
  for (int i = 8; i < 16; ++i)
    if (grid.tiles[i] == 0) return false;
  for (int i = 2; i < 4; ++i)
    for (int j = 0; j < 3; ++j)
      if (grid.tiles[i*4+j] == grid.tiles[i*4+j+1]) return false;
  for (int i = 0; i < 4; ++i)
    if (grid.tiles[8+i] == grid.tiles[12+i]) return false;
  for (int i = 0; i < 8; ++i)
    if (grid.tiles[i] >= 8) return false;
  return true;
}

int32_t to24bit(grid::Grid grid) {
  int32_t res = 0;
  for (int i = 0; i < 8; ++i)
    res |= (grid.tiles[i] & 0x7) << (i*3);
  return res;
}

std::pair<double, grid::Direction> optimove_impl(int32_t bits, int pos, int goal) {
  std::string filename = "data/data_2048_" + std::to_string(pos) + "_" + std::to_string(goal) + ".txt";
  FILE *fp = std::fopen(filename.c_str(), "r");
  std::fseek(fp, bits * 9, SEEK_SET);
  double prob;
  int dir;
  fscanf(fp, "%lf%d", &prob, &dir);
  fclose(fp);
  return std::make_pair(prob, dir);
}

grid::Direction opti(const grid::Grid& grid) {
  using grid::Grid;
  using grid::Direction;
  std::array<double, 4> scores = search::great_search(grid);
  for (int i = 0; i < 4; ++i) {
    Grid rotated = grid.Rotate(i);
    if (is_fixed_grid(rotated)) {
      int32_t bits = to24bit(rotated);
      for (int j = 0; j < 4; ++j) {
        int goal = rotated.tiles[8+j];
        if (goal >= 8 || goal <= 4) continue;
        auto p = optimove_impl(bits, j, goal);
        Direction dir = (p.second + i) % 4;
        Grid cp(rotated);
        for (int k = 0; k < 8; ++k) cp.tiles[k] = 0;
        ++cp.tiles[8 + j];
        double pt = score::static_score_light(cp) * pow(p.first, 2.0);
        scores[dir] = std::max(scores[dir] , pt);
      }
    }
  }
  return std::max_element(std::begin(scores), std::end(scores)) - std::begin(scores);
}

}
}
