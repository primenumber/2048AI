#include "search.hpp"
#include <cassert>
#include <cmath>
#include <utility>
#include <vector>

int alpha_beta_player(const table_t& table, int alpha, int beta, int depth) {
  if (depth == 0) return static_score_light(table);
  for (int i = 0; i < 4; ++i) {
    table_t moved = move(table, i);
    int score = alpha_beta_host(std::move(moved), alpha, beta, depth - 1);
    if (score > alpha) {
      alpha = score;
      if (alpha >= beta) {
        return beta;
      }
    }
  }
  return alpha;
}

int alpha_beta_host(table_t&& table, int alpha, int beta, int depth) {
  if (depth == 0) return static_score_light(table);
  auto zeros = zero_list(table);
  int numbers[2] = {2, 4};
  for (auto tile : zeros) {
    for (int i = 0; i < 2; ++i) {
      table[tile.first][tile.second] = numbers[i];
      int score = alpha_beta_player(table, alpha, beta, depth - 1);
      if (score < beta) {
        beta = score;
        if (alpha >= beta) {
          return alpha;
        }
      }
    }
    table[tile.first][tile.second] = 0;
  }
  return beta;
}


int alpha_beta_search(const table_t& table) {
  int alpha = -1000000000;
  int beta = 1000000000;
  int optimal = -1;
  int kDepth = 7;
  for (int i = 0; i < 4; ++i) {
    if (!movable(table, i)) continue;
    table_t moved = move(table, i);
    int score = alpha_beta_host(std::move(moved), alpha, beta, kDepth - 1);
    if (score > alpha) {
      alpha = score;
      optimal = i;
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

std::pair<int64_t, int> playout(const table_t& table, int direction) {
  table_t moved = move(table, direction);
  auto zeros = zero_list(moved);
  int z = rand() % zeros.size();
  moved[zeros[z].first][zeros[z].second] = ((rand() % 2) >= 1) ? 2 : 4;
  int max_score = 0.0;
  int max_i = -1;
  for (int i = 0; i < 4; ++i) {
    if (movable(moved, i)) {
      int score = -static_score_light(move(moved, i));
      if (score > max_score) {
        max_score = score;
        max_i = i;
      }
    }
  }
  if (max_i != -1) {
    auto result = playout(moved, max_i);
    return std::make_pair(result.first, result.second + 1);
  } else {
    return std::make_pair(sq_sum_grid(moved), 1);
  }
}

int Monte_Carlo_search(const table_t& table) {
  std::vector<Play> movable_list;
  for (int i = 0; i < 4; ++i)
    if (movable(table, i))
      movable_list.emplace_back(i);
  if (movable_list.size() == 1) return movable_list.front().direction;
  int playout_count = 0;
  for (auto& play : movable_list) {
    auto result = playout(table, play.direction);
    play.playout(result.first);
    playout_count += result.second;
  }
  int sum = sum_grid(table);
  for (int i = 0; i < 80 || playout_count < 10000; ++i) {
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
    auto result = playout(table, selecetd_play.direction);
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
  return max_average_score_direction;
}

int search(const table_t& table) {
  // return Monte_Carlo_search(table);
  return alpha_beta_search(table);
}
