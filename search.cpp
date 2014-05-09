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

int alpha_beta_host(grid::Grid grid, std::map<grid::Grid, int>& scores, int alpha, int beta, int depth) {
  if (depth == 0) return score::static_score_light(grid);
  auto zeros = grid.zero_tiles();
  int numbers[2] = {2, 4};
  for (auto tile : zeros) {
    for (int i = 0; i < 2; ++i) {
      grid.table[tile.first][tile.second] = numbers[i];
      int local_alpha = alpha;
      int local_beta = beta;
      for (int j = 0; j < 4; ++j) {
        if (!grid.is_movable(j)) continue;
        grid::Grid next_grid = grid.move(j);
        int score;
        if (scores.count(next_grid) == 0) {
          score = alpha_beta_host(next_grid, scores, alpha, beta, depth - 1);
          scores.insert(std::make_pair(next_grid, score));
        } else {
          score = scores[next_grid];
        }
        if (score > local_alpha) {
          local_alpha = score;
          if (local_alpha >= local_beta) {
            local_alpha = local_beta;
            break;
          }
        }
      }
      if (local_alpha < beta) {
        beta = local_alpha;
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
  std::map<grid::Grid, int> scores;
  for (int kDepth = 4; kDepth < 10; ++kDepth) {
    auto start = std::chrono::system_clock::now();
    int alpha = -1000000000;
    int beta = 1000000000;
    for (int i = 0; i < 4; ++i) {
      if (!grid.is_movable(i)) continue;
      grid::Grid moved = grid.move(i);
      int score = alpha_beta_host(std::move(moved), scores, alpha, beta, kDepth - 1);
      scores.insert(std::make_pair(moved, score));
      if (score > alpha || optimal == -1) {
        alpha = score;
        optimal = i;
      }
    }
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - start;
    if (elapsed.count() > 2000000) break;
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

std::pair<int64_t, int> playout(const grid::Grid& grid, int direction) {
  grid::Grid moved = grid.move(direction);
  auto zeros = moved.zero_tiles();
  int z = mt() % zeros.size();
  moved.table[zeros[z].first][zeros[z].second] = ((mt() % 2) >= 1) ? 2 : 4;
  int max_score = 0.0;
  int max_i = -1;
  for (int i = 0; i < 4; ++i) {
    if (moved.is_movable(i)) {
      int score = score::static_score_light(moved.move(i));
      if (score > max_score) {
        max_score = score;
        max_i = i;
      }
    }
  }
  if (max_i != -1) {
    auto result = playout(moved, max_i);
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
    auto result = playout(grid, play.direction);
    play.playout(result.first);
    playout_count += result.second;
  }
  int sum = grid.sum_tiles();
  for (int i = 0; i < 80 || playout_count < 5000; ++i) {
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
    auto result = playout(grid, selecetd_play.direction);
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
