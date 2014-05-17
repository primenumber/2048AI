#pragma once
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include "game.hpp"
#include "score.hpp"

namespace ai2048 {
namespace gamedag {

class GameDAG;

class Edge : boost::operators<Edge> {
 public:
  Edge(const std::shared_ptr<GameDAG>& ptr, int i_, int j_, int k_)
      : child_ptr(ptr), i(i_), j(j_), k(k_) {}
  Edge(std::shared_ptr<GameDAG>&& ptr, int i_, int j_, int k_)
      : child_ptr(std::move(ptr)), i(i_), j(j_), k(k_) {}
  std::shared_ptr<GameDAG> child_ptr;
  int i;
  int j;
  int k;
  int value;
};

class GameDAG {
 public:
  explicit GameDAG(const game::Game& game_)
      : game(game_), children() {}
  explicit GameDAG(game::Game&& game_)
      : game(std::move(game_)), children() {}
  int alpha_beta_search(
      std::map<game::Game, std::shared_ptr<GameDAG>>& game_map,
      int depth, int alpha = kVALUE_MIN, int beta = kVALUE_MAX);
  grid::Direction getOptimal(
      std::map<game::Game, std::shared_ptr<GameDAG>>& game_map,
      int depth);
  game::Game game;
  std::vector<Edge> children;
  constexpr static int kVALUE_MIN = -1000000000;
  constexpr static int kVALUE_MAX = 1000000000;
  int value_min = kVALUE_MIN;
  int value_max = kVALUE_MAX;
  int score_depth = 0;
 private:
  int alpha_beta_impl(std::map<game::Game, std::shared_ptr<GameDAG>>& game_map,
      int depth, int alpha, int beta);
  void make_children(std::map<game::Game, std::shared_ptr<GameDAG>>& game_map);
  int static_score();
  int gameover_score();
  void update_values(int value);
};

bool operator<(const Edge&, const Edge&);
bool operator<(const std::shared_ptr<GameDAG>&, const std::shared_ptr<GameDAG>&);
bool operator<(const GameDAG&, const GameDAG&);

} // namespace gamedag
} // namespace ai2048
