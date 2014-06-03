#include "search.hpp"

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

std::array<int, 4> simple_search_big(const grid::Grid& grid) {
  const int max_depth = 8;
  std::vector<HistoryGrid> all_grids = HistoryGrid(grid, 0).getAllNextStatesFillWith2();
  for (int i = 0; i < max_depth; ++i) {
    std::vector<HistoryGrid> next_grids;
    for (auto& now_grid : all_grids) {
      auto nexts = now_grid.getAllNextStatesFillWith2(now_grid.direction);
      next_grids.insert(std::end(next_grids), std::begin(nexts), std::end(nexts));
    }
    if (next_grids.size() == 0) break;
    all_grids.swap(next_grids);
  }
  std::array<int, 4> scores = {{0, 0, 0, 0}};
  for (const auto& last_grid : all_grids) {
    scores[last_grid.direction] = std::max(scores[last_grid.direction], last_grid.score);
  }
  return scores;
}

std::array<int, 4> simple_search(const grid::Grid& grid) {
  const int max_node = 10000;
  const int max_depth = 8;
  std::vector<HistoryGrid> all_grids = HistoryGrid(grid, 0).getAllNextStatesFillWith2();
  for (int i = 0; i < max_depth; ++i) {
    std::vector<HistoryGrid> next_grids;
    for (auto& now_grid : all_grids) {
      auto nexts = now_grid.getAllNextStatesFillWith2(now_grid.direction);
      next_grids.insert(std::end(next_grids), std::begin(nexts), std::end(nexts));
    }
    if (next_grids.size() == 0) return simple_search_big(grid);
    all_grids.swap(next_grids);
    std::partial_sort(std::begin(all_grids),
        std::min(std::end(all_grids), std::begin(all_grids) + max_node),
        std::end(all_grids),
        std::greater<HistoryGrid>());
    if (all_grids.size() > max_node) 
      all_grids.resize(max_node, HistoryGrid(grid));
  }
  std::array<int, 4> scores = {{0, 0, 0, 0}};
  for (const auto& last_grid : all_grids) {
    scores[last_grid.direction] = std::max(scores[last_grid.direction], last_grid.score);
  }
  return scores;
}

} // namespace search
} // namespace ai2048
