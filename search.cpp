#include "search.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include <chrono>
#include <unordered_map>

extern std::mt19937 mt;

namespace ai2048 {
namespace search {

std::unordered_map<grid::Grid, double> grids;

int alpha_beta_host(const grid::Grid, std::map<grid::Grid, Value>&, int, int, int);

int alpha_beta_player(const grid::Grid& grid,
    std::map<grid::Grid, Value>& scores,
    int alpha, int beta, int depth) {
  for (int j = 0; j < 4; ++j) {
    if (!grid.is_movable(j)) continue;
    grid::Grid next_grid = grid.move(j);
    auto score_itr = scores.find(next_grid);
    if (score_itr == end(scores)) {
      const int score = alpha_beta_host(next_grid, scores, alpha, beta, depth - 1);
      if (beta == score) {
        scores.insert(std::make_pair(next_grid, Value(beta, Value::kValueMax)));
        return beta;
      } else if (score == alpha) {
        scores.insert(std::make_pair(next_grid, Value(Value::kValueMin, alpha)));
      } else {
        scores.insert(std::make_pair(next_grid, Value(score, score)));
        alpha = score;
      } 
    } else {
      Value& svalue = score_itr->second;
      if (svalue.value_max <= alpha) continue;
      if (svalue.value_min >= beta) return beta;
      alpha = std::max(alpha, svalue.value_min);
      const int score = alpha_beta_host(next_grid, scores,
          alpha, std::min(beta, svalue.value_max), depth - 1);
      if (beta == score) {
        svalue.value_min = beta;
        return beta;
      } else if (score == alpha) {
        svalue.value_max = alpha;
      } else {
        alpha = svalue.value_min = svalue.value_max = score;
      }
    }
    if (alpha >= beta) return beta;
  }
  return alpha;
}

int alpha_beta_host(grid::Grid grid,
    std::map<grid::Grid, Value>& scores,
    int alpha, int beta, int depth) {
  if (depth == 0) return score::static_score_light(grid);
  auto zeros = grid.zero_tiles();
  int numbers[2] = {1, 2};
  for (auto tile : zeros) {
    for (int i = 0; i < 2; ++i) {
      grid[tile.first][tile.second] = numbers[i];
      int score = alpha_beta_player(grid, scores, alpha, beta, depth);
      if (score < beta) {
        beta = score;
        if (alpha >= beta) {
          return alpha;
        }
      }
    }
    grid[tile.first][tile.second] = 0;
  }
  return beta;
}

std::array<int, 4> alpha_beta_search(const grid::Grid& grid) {
  int optimal = -1;
  int now_score = score::static_score(grid);
  auto movable = grid.movable_directions();
  std::array<int, 4> scores = {{0, 0, 0, 0}};
  for (int kDepth = 3; kDepth < 40; ++kDepth) {
    std::map<grid::Grid, Value> scores_map;
    auto start = std::chrono::system_clock::now();
    int alpha = -1000000000;
    int beta = 1000000000;
    int score_sum = 0;
    for (grid::Direction dir : movable) {
      grid::Grid moved = grid.move(dir);
      alpha_beta_host(std::move(moved), scores_map, now_score, now_score + 1, kDepth - 1);
      int score = alpha_beta_host(std::move(moved), scores_map, alpha, beta, kDepth - 1);
      if (score > alpha || optimal == -1) {
        alpha = score;
        optimal = dir;
        scores[dir] = score;
      } else {
        scores[dir] = std::min(alpha - 1, score);
      }
      score_sum += score;
    }
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - start;
    if (elapsed.count() > 50000) {
      std::cout << "{\"type\":\"debug\",\"score\":" << kDepth << "}" << std::endl;
      break;
    }
    now_score = score_sum / movable.size();
  }
  assert(optimal != -1);
  return scores;
}

double great_search_host(grid::Grid grid, int depth);

double great_search_player(const grid::Grid& grid, int depth) {
  auto itr = grids.find(grid);
  if (itr != std::end(grids)) {
    return itr->second;
  }
  if (depth == 0) {
    double score = score::static_score_light(grid);
    grids.insert(std::make_pair(grid, score));
    return score;
  } else {
    auto movables = grid.movable_directions();
    if (movables.empty()) return score::static_score_light(grid) - 10000000;
    double max_score = -1000000000;
    for (auto direction : movables) {
      grid::Grid moved = grid.move(direction);
      max_score = std::max(max_score, great_search_host(moved, depth - 1));
    }
    grids.insert(std::make_pair(grid, max_score));
    return max_score;
  }
}

double great_search_host(grid::Grid grid, int depth) {
  auto zeros = grid.zero_tiles();
  double score = 0.0;
  int size = zeros.size();
  for (auto tile : zeros) {
    grid[tile.first][tile.second] = 1;
    score += (1.0 / size) * 0.9 * great_search_player(grid, depth);
    grid[tile.first][tile.second] = 2;
    score += (1.0 / size) * 0.1 * great_search_player(grid, depth);
    grid[tile.first][tile.second] = 0;
  }
  return score;
}

std::array<double, 4> great_search(const grid::Grid& grid) {
  auto movables = grid.movable_directions();
  std::array<double, 4> scores;
  for (int depth = 2; depth <= 20; ++depth) {
    grids.clear();
    auto start = std::chrono::high_resolution_clock::now();
    std::fill(std::begin(scores), std::end(scores), -1000000000.0);
    for (auto direction : movables) {
      grid::Grid moved = grid.move(direction);
      scores[direction] = great_search_host(moved, depth);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    if (elapsed.count() > 0.003) {
      std::cerr << "depth: " << depth + 1 << std::endl;
      break;
    }
  }
  return scores;
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
  moved[zeros[z].first][zeros[z].second] = ((mt() % 2) >= 1) ? 1 : 2;
  auto movable = moved.movable_directions();
  if (!movable.empty()) {
    int dir = movable[mt() % movable.size()];
    auto result = playout(moved, dir, depth - 1);
    return std::make_pair(result.first + 1, result.second + 1);
  } else {
    return std::make_pair(0, 1);
  }
}

std::array<double, 4> Monte_Carlo_search(const grid::Grid& grid) {
  std::vector<Play> movable_list;
  for (int i = 0; i < 4; ++i)
    if (grid.is_movable(i))
      movable_list.emplace_back(i);
  int playout_count = 0;
  for (auto& play : movable_list) {
    auto result = playout(grid, play.direction, 100);
    play.playout(result.first);
    playout_count += result.second;
  }
  int sum = 100;
  for (int i = 0; i < 2000 || playout_count < 100000; ++i) {
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
  std::array<double, 4> scores = {{0.0, 0.0, 0.0, 0.0}};
  for (Play& play : movable_list) {
    double average = (double)play.score_sum / play.search_num;
    scores[play.direction] = average;
  }
  return scores;
}

int calc_ideal_score(grid::Grid grid) {
  std::array<int, 16> sorted_score = {
    {1,1,1,1,1,1,3,3,10,10,10,35,40,40,50,80}
  };
  std::sort(std::begin(grid.tiles), std::end(grid.tiles));
  return std::inner_product(std::begin(grid.tiles), std::end(grid.tiles),
      std::begin(sorted_score), 0, std::plus<int>(), [](int g, int s) { return s << g; });
}

int combination_search(const grid::Grid& grid) {
  using std::begin;
  using std::end;
  using std::max_element;
  auto simple_search_scores = simple_search(grid);
  int ideal_score = calc_ideal_score(grid);
  int simple_search_max = *max_element(begin(simple_search_scores), end(simple_search_scores));
  std::cout << "{\"type\":\"debug\",\"score\":" << ((double)ideal_score / simple_search_max) << "}" << std::endl;
  if (simple_search_max * 2 < ideal_score) {
    auto Monte_Carlo_scores = Monte_Carlo_search(grid);
    return max_element(begin(Monte_Carlo_scores), end(Monte_Carlo_scores)) - begin(Monte_Carlo_scores);
  } else {
    return max_element(begin(simple_search_scores), end(simple_search_scores)) - begin(simple_search_scores);
  }
}

int search(const grid::Grid& grid) {
  using std::begin;
  using std::end;
  using std::max_element;
  // return Monte_Carlo_search(grid);
  // return alpha_beta_search(grid);
  // return simple_search(grid);
  // return combination_search(grid);
  auto search_scores = great_search(grid);
  return max_element(begin(search_scores), end(search_scores)) - begin(search_scores);
}

} // namespace search
} // namespace ai2048
