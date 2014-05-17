#pragma once
#include <tuple>
#include <vector>
#include <boost/operators.hpp>
#include "grid.hpp"

namespace ai2048 {
namespace game {

enum class State {
  Player,
  Host,
};

bool operator<(const State, const State);

class Game {
 public:
  Game(const grid::Grid& grid_, State state_) : grid(grid_), state(state_) {}
  Game(const Game&) = default;
  Game(Game&&) = default;
  Game& operator=(const Game&) = default;
  Game& operator=(Game&&) = default;
  std::vector<std::tuple<Game, int, int ,int>> getAllNextStates() const;
  std::vector<std::tuple<Game, int, int ,int>> getAllNextStatesFillWith2() const;
  grid::Grid grid;
  State state;
};

bool operator==(const Game&, const Game&);
bool operator<(const Game&, const Game&);

} // namespace game
} // namespace ai2048
