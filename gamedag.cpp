#include "gamedag.hpp"
#include <algorithm>

namespace ai2048 {
namespace gamedag {

int GameDAG::static_score() {
  if (game.state == game::State::Player)
    return score::static_score(game.grid);
  else
    return -score::static_score(game.grid);
}

int GameDAG::gameover_score() {
  if (game.state == game::State::Player)
    return score::static_score(game.grid) - 100000000;
  else
    return -score::static_score(game.grid) + 100000000;
}

void GameDAG::make_children(
    std::map<game::Game, std::shared_ptr<GameDAG>>& game_map) {
  auto nexts = game.getAllNextStatesFillWith2();
  for (const auto& next_state : nexts) {
    auto itr = std::find_if(std::begin(children), std::end(children),
        [&next_state](const Edge& child) {
          return child.child_ptr->game == std::get<0>(next_state);
        });
    if (itr == std::end(children)) {
      children.emplace_back(std::make_shared<GameDAG>(std::get<0>(next_state)),
          std::get<1>(next_state),
          std::get<2>(next_state),
          std::get<3>(next_state));
      itr = std::end(children) - 1;
    }
    auto game_itr = game_map.find(std::get<0>(next_state));
    if (game_itr == std::end(game_map)) {
      game_map.insert(std::make_pair(std::get<0>(next_state), itr->child_ptr));
    }
  }
}

int GameDAG::alpha_beta_search(
    std::map<game::Game, std::shared_ptr<GameDAG>>& game_map,
    int depth, int alpha, int beta) {
  if (depth > score_depth) {
    value_min = kVALUE_MIN;
    value_max = kVALUE_MAX;
  }
  if (value_min == value_max) return value_min;
  const int local_alpha = std::max(alpha, value_min);
  const int local_beta = std::min(beta, value_max);
  int result = alpha_beta_impl(
      game_map,
      depth,
      local_alpha,
      local_beta);
  if (local_alpha >= result) {
    value_max = local_alpha;
  } else if (local_beta <= result) {
    value_min = local_beta;
  } else {
    value_min = value_max = result;
  }
  score_depth = std::max(score_depth, depth);
  return result;
}

grid::Direction GameDAG::getOptimal(
    std::map<game::Game, std::shared_ptr<GameDAG>>& game_map, int depth) {
  alpha_beta_search(game_map, depth, kVALUE_MIN, kVALUE_MAX);
  return children.front().i;
}

int GameDAG::alpha_beta_impl(std::map<game::Game, std::shared_ptr<GameDAG>>& game_map,
    int depth, int alpha, int beta) {
  if (depth == 0)
    return static_score();
  make_children(game_map);
  if (children.empty())
    return gameover_score();
  int count = 0;
  for (auto& edge: children) {
    ++count;
    edge.value = edge.child_ptr->alpha_beta_search(
        game_map,
        depth - 1,
        alpha,
        beta);
    if (alpha < edge.value) {
      alpha = edge.value;
      if (alpha >= beta) {
        std::sort(std::begin(children),
            std::begin(children) + count,
            std::greater<Edge>());
        return beta;
      }
    }
  }
  std::sort(std::begin(children), std::end(children), std::greater<Edge>());
  return alpha;
}

void GameDAG::update_values(int value) {
  value_min = value_max = value;
}

bool operator<(const Edge& lhs, const Edge& rhs) {
  return lhs.value < rhs.value;
}

bool operator<(
    const std::shared_ptr<GameDAG>& lhs,
    const std::shared_ptr<GameDAG>& rhs) {
  return lhs->value_max < rhs->value_max;
}

bool operator<(const GameDAG& lhs, const GameDAG& rhs) {
  return lhs.value_max < rhs.value_max;
}

} // gamedag
} // ai2048
