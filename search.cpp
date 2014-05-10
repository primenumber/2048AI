#include "search.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include <chrono>

extern std::mt19937 mt;

namespace ai2048 {
namespace search {

int alpha_beta_player(const grid::Grid& grid,
    std::map<grid::Grid, Value>& scores,
    int alpha, int beta, int depth) {
  for (int j = 0; j < 4; ++j) {
    if (!grid.is_movable(j)) continue;
    grid::Grid next_grid = grid.move(j);
    int score;
    auto score_itr = scores.find(next_grid);
    if (score_itr == end(scores)) {
      score = alpha_beta_host(next_grid, scores, alpha, beta, depth - 1);
      scores.insert(std::make_pair(next_grid, Value(score, alpha, beta)));
    } else {
      if (score_itr->second.beta < alpha
          && score_itr->second.value == score_itr->second.beta) {
        score = alpha_beta_host(next_grid, scores, alpha, beta, depth - 1);
        score_itr->second.value = score;
        if (score > alpha) {
          score_itr->second.beta = beta;
        } else {
          score_itr->second.alpha = score_itr->second.beta;
          score_itr->second.beta = alpha;
        }
      } else if (score_itr->second.alpha > beta
          && score_itr->second.value == score_itr->second.alpha) {
        score = alpha_beta_host(next_grid, scores, alpha, beta, depth - 1);
        score_itr->second.value = score;
        if (score < beta) {
          score_itr->second.alpha = alpha;
        } else {
          score_itr->second.beta = score_itr->second.alpha;
          score_itr->second.alpha = beta;
        }
      } else {
        score = std::max(alpha, std::min(beta, score_itr->second.value));
      }
    }
    if (score > alpha) {
      alpha = score;
      if (alpha >= beta) {
        return beta;
      }
    }
  }
  return alpha;
}

int alpha_beta_host(grid::Grid grid,
    std::map<grid::Grid, Value>& scores,
    int alpha, int beta, int depth) {
  if (depth == 0) return score::static_score_light(grid);
  auto zeros = grid.zero_tiles();
  int numbers[2] = {2, 4};
  for (auto tile : zeros) {
    for (int i = 0; i < 2; ++i) {
      grid.table[tile.first][tile.second] = numbers[i];
      int score = alpha_beta_player(grid, scores, alpha, beta, depth);
      if (score < beta) {
        beta = score;
        if (alpha >= beta) {
          return alpha;
        }
      }
    }
    grid.table[tile.first][tile.second] = 0;
  }
  return beta;
}

int alpha_beta_search(const grid::Grid& grid) {
  int optimal = -1;
  int now_score = score::static_score(grid);
  for (int kDepth = 4; kDepth < 12; ++kDepth) {
    std::map<grid::Grid, Value> scores;
    auto start = std::chrono::system_clock::now();
    int alpha = -1000000000;
    int beta = 1000000000;
    for (int i = 0; i < 4; ++i) {
      if (!grid.is_movable(i)) continue;
      grid::Grid moved = grid.move(i);
      alpha_beta_host(std::move(moved), scores, now_score - 1, now_score, kDepth - 1);
      int score = alpha_beta_host(std::move(moved), scores, alpha, beta, kDepth - 1);
      scores.insert(std::make_pair(moved, Value(score, alpha, beta)));
      if (score > alpha || optimal == -1) {
        alpha = score;
        optimal = i;
      }
    }
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - start;
    if (elapsed.count() > 50000) {
      std::cout << "{\"type\":\"debug\",\"score\":" << kDepth << "}" << std::endl;
      break;
    }
  }
  assert(optimal != -1);
  return optimal;
}

struct Play {
 public:
  Play(const int direction_) : direction(direction_), score_sum(0), search_num(0) {}
  void playout(int64_t score) {
    score_sum += score;
    ++search_num;
  }
  int direction;
  int64_t score_sum;
  int search_num;
};

std::pair<int64_t, int> playout(const grid::Grid& grid, int direction, int depth) {
  if (depth == 0) return std::make_pair(grid.sum_tiles(), 0);
  grid::Grid moved = grid.move(direction);
  auto zeros = moved.zero_tiles();
  int z = mt() % zeros.size();
  moved.table[zeros[z].first][zeros[z].second] = ((mt() % 2) >= 1) ? 2 : 4;
  auto movable = moved.movable_directions();
  int max_score = -1000000000;
  int max_i = -1;
  for (int dir : movable) {
    int score = score::static_score_light(moved.move(dir));
    if (score > max_score) {
      max_score = score;
      max_i = dir;
    }
  }
  if (!movable.empty()) {
    auto result = playout(moved, max_i, depth - 1);
    return std::make_pair(result.first + 1, result.second + 1);
  } else {
    return std::make_pair(moved.sum_tiles(), 1);
  }
}

int Monte_Carlo_search(const grid::Grid& grid) {
  std::vector<Play> movable_list;
  for (int i = 0; i < 4; ++i)
    if (grid.is_movable(i))
      movable_list.emplace_back(i);
  if (movable_list.size() == 1) return movable_list.front().direction;
  int playout_count = 0;
  for (auto& play : movable_list) {
    auto result = playout(grid, play.direction, 100);
    play.playout(result.first);
    playout_count += result.second;
  }
  int sum = grid.sum_tiles();
  for (int i = 0; i < 100 || playout_count < 5000; ++i) {
    double max_ucb1 = 0.0;
    int max_ucb1_play = -1;
    for (int j = 0; j < movable_list.size(); ++j) {
      Play& play = movable_list[j];
      int n = play.search_num;
      double ucb1 = (double)play.score_sum / n + std::sqrt(sum * 2.0 * std::log(i + movable_list.size()) / n);
      if (ucb1 > max_ucb1) {
        max_ucb1 = ucb1;
        max_ucb1_play = j;
      }
    }
    Play& selecetd_play = movable_list[max_ucb1_play];
    auto result = playout(grid, selecetd_play.direction, 100);
    selecetd_play.playout(result.first);
    playout_count += result.second;
  }
  double max_average_score = 0.0;
  int max_average_score_direction = -1;
  for (Play& play : movable_list) {
    double average = (double)play.score_sum / play.search_num;
    if (average > max_average_score) {
      max_average_score = average;
      max_average_score_direction = play.direction;
    }
  }
  std::cout << "{\"type\":\"debug\",\"score\":"<<max_average_score<<"}" << std::endl;
  return max_average_score_direction;
}

int search(const grid::Grid& grid) {
  return Monte_Carlo_search(grid);
  // return alpha_beta_search(grid);
}

} // namespace search
} // namespace ai2048
