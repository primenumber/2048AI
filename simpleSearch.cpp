#include "search.hpp"
#include <boost/optional.hpp>

namespace ai2048 {
namespace search {

struct HistoryGrid {
  grid::Grid grid;
  grid::Direction direction;
  int score;
  HistoryGrid(grid::Grid grid_, grid::Direction direction_)
      : grid(grid_), direction(direction_), score(score::static_score(grid)) {}
  HistoryGrid(grid::Grid grid_) : grid(grid_), score(score::static_score(grid)) {}
  std::vector<HistoryGrid> getAllNextStatesFillWith2() const {
    auto movable = grid.movable_directions();
    std::vector<HistoryGrid> nexts;
    for (grid::Direction direction : movable) {
      grid::Grid moved = grid.move(direction);
      auto zeros = moved.zero_tiles();
      for (auto tile : zeros)
        moved[tile.first][tile.second] = 1;
      nexts.emplace_back(moved, direction);
    }
    return nexts;
  }
  std::vector<HistoryGrid> getAllNextStatesFillWith2(grid::Direction history) const {
    auto movable = grid.movable_directions();
    std::vector<HistoryGrid> nexts;
    for (grid::Direction direction : movable) {
      grid::Grid moved = grid.move(direction);
      auto zeros = moved.zero_tiles();
      for (auto tile : zeros)
        moved[tile.first][tile.second] = 1;
      nexts.emplace_back(moved, history);
    }
    return nexts;
  }
};

bool operator>(const HistoryGrid& lhs, const HistoryGrid& rhs) {
  return lhs.score > rhs.score;
}

int simple_search(const grid::Grid& grid) {
  const int max_node = 3000;
  std::vector<HistoryGrid> all_grids = HistoryGrid(grid, 0).getAllNextStatesFillWith2();
  for (int i = 0; i < 20; ++i) {
    std::vector<HistoryGrid> next_grids;
    for (auto& now_grid : all_grids) {
      auto nexts = now_grid.getAllNextStatesFillWith2(now_grid.direction);
      next_grids.insert(std::end(next_grids), std::begin(nexts), std::end(nexts));
    }
    if (next_grids.size() == 0) break;
    all_grids.swap(next_grids);
    std::sort(std::begin(all_grids), std::end(all_grids), std::greater<HistoryGrid>());
    if (all_grids.size() > max_node) 
      all_grids.resize(max_node, HistoryGrid(grid));
  }
  int score_max = 0;
  int optimal = 0;
  for (auto& hoge : all_grids) {
    int score = score::static_score(hoge.grid);
    if (score > score_max) {
      score_max = score;
      optimal = hoge.direction;
    }
  }
  return optimal;
}


} // namespace search
} // namespace ai2048
