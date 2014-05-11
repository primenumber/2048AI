#include "gametree.hpp"
#include <algorithm>

namespace ai2048 {
namespace gamedag {

int GameDAG::alpha_beta_search(
    std::map<game::Game, std::shared_ptr<GameDAG>> game_map,
    int depth, int alpha, int beta) {
  if (depth == 0) {
    if (game.state == game::State::Player)
      return -score::static_score(game.grid);
    else
      return score::static_score(game.grid);
  }
  auto nexts = game.getAllNextStates();
  if (nexts.empty()) {
    if (game.state == game::State::Player)
      return -score::static_score(game.grid);
    else
      return score::static_score(game.grid);
  }
  int count = 0;
  for (const auto& next_state : nexts) {
    ++count;
    auto game_itr = game_map.find(std::get<0>(next_state));
    std::shared_ptr<GameDAG> next_ptr;
    if (game_itr == std::end(game_map)) {
      children.emplace_back(std::make_shared<GameDAG>(std::get<0>(next_state)),
          std::get<1>(next_state),
          std::get<2>(next_state),
          std::get<3>(next_state));
      next_ptr = children.back().child_ptr;
      game_map.insert(std::make_pair(std::get<0>(next_state), next_ptr));
    } else {
      next_ptr = game_itr->second;
      if (-next_ptr->value_max >= beta) {
        if (!children.empty())
          std::sort(std::begin(children), std::end(children) - 1, std::greater<Edge>());
        return beta;
      }
      if (-next_ptr->value_min <= alpha) continue;
      alpha = std::max(alpha, -next_ptr->value_max);
    }
    const int value = -next_ptr->alpha_beta_search(game_map, depth - 1, -beta, -alpha);
    if (value == beta) {
      next_ptr->value_min = -beta;
      std::sort(std::begin(children), std::end(children), std::greater<Edge>());
      return beta;
    } else if (alpha == value) {
      next_ptr->value_max = -alpha;
    } else {
      next_ptr->value_min = next_ptr->value_max = -value;
      alpha = value;
    }
  }
  std::sort(std::begin(children), std::end(children), std::greater<Edge>());
  return alpha;
}

grid::Direction GameDAG::getOptimal(
    std::map<game::Game, std::shared_ptr<GameDAG>> game_map, int depth) {
  alpha_beta_search(game_map, depth, kVALUE_MIN, kVALUE_MAX);
  return children.front().i;
}

bool operator<(const Edge& lhs, const Edge& rhs) {
  if (lhs.child_ptr->value_max < rhs.child_ptr->value_min)
    return true;
  return false;
}

bool operator<(const std::shared_ptr<GameDAG>& lhs, const std::shared_ptr<GameDAG>& rhs) {
  return lhs->value_max < rhs->value_max;
}

bool operator<(const GameDAG& lhs, const GameDAG& rhs) {
  return lhs.value_max < rhs.value_max;
}

} // gamedag
} // ai2048
