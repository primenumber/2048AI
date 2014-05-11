#include "game.hpp"

namespace ai2048 {
namespace game {

bool operator<(const State lhs, const State rhs) {
  if (lhs == State::Player && rhs == State::Host) return true;
  return false;
}

std::vector<std::tuple<Game, int, int, int>> Game::getAllNextStates() const {
  std::vector<std::tuple<Game, int, int, int>> nexts;
  if (state == State::Player) {
    auto movable = grid.movable_directions();
    for (grid::Direction dir : movable)
      nexts.emplace_back(Game(grid.move(dir), State::Host), dir, 0, 0);
  } else {
    auto zeros = grid.zero_tiles();
    grid::Grid c_grid = grid;
    int add_numbers[2] = {2, 4};
    for (auto& tile : zeros) {
      for (int i = 0; i < 2; ++i) {
        c_grid.table[tile.first][tile.second] = add_numbers[i];
        nexts.emplace_back(Game(c_grid, State::Player), tile.first, tile.second, add_numbers[i]);
      }
      c_grid.table[tile.first][tile.second] = 0;
    }
  }
  return nexts;
}

bool operator==(const Game& lhs, const Game& rhs) {
  return lhs.grid == rhs.grid && lhs.state == rhs.state;
}

bool operator<(const Game& lhs, const Game& rhs) {
  return lhs.grid < rhs.grid || (lhs.grid == rhs.grid && lhs.state < rhs.state);
}

} // namespace game
} // namespace ai2048
