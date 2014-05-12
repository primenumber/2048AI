#include "search.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include <chrono>

extern std::mt19937 mt;

namespace ai2048 {
namespace search {

int divided_score(const grid::Grid& grid) {
  int sum = grid.sum_tiles();
  int max_patial_sum = 0;
  for (int i = 0; i < 2; ++i) {
    int partial_sum = 0;
    for (int j = 0; j < 4; ++j)
      partial_sum += grid.table[i*3][j];
    max_patial_sum = std::max(max_patial_sum, partial_sum);
    partial_sum = 0;
    for (int j = 0; j < 4; ++j)
      partial_sum += grid.table[j][i*3];
    max_patial_sum = std::max(max_patial_sum, partial_sum);
  }
  if (max_patial_sum == sum) return max_patial_sum;
  return max_patial_sum / (sum - max_patial_sum);
}

struct Play {
 public:
  Play(const int direction_)
      : direction(direction_), score_sum(0), search_num(0) {}
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
  moved.table[zeros[z].first][zeros[z].second] = ((mt() % 10) >= 1) ? 2 : 4;
  auto movable = moved.movable_directions();
  if (!movable.empty()) {
    int dir = movable[mt() % movable.size()];
    auto result = playout(moved, dir);
    return std::make_pair(result.first + 1, result.second + 1);
  } else {
    return std::make_pair(0, 1);
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
  int sum = 10;
  for (int i = 0; i < 2000 || playout_count < 100000; ++i) {
    double max_ucb1 = 0.0;
    int max_ucb1_play = -1;
    for (int j = 0; j < movable_list.size(); ++j) {
      Play& play = movable_list[j];
      int n = play.search_num;
      double ucb1 = (double)play.score_sum / n
          + std::sqrt(sum * 2.0 * std::log(i + movable_list.size()) / n);
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
    double average = (double)play.score_sum / play.search_num + divided_score(grid.move(play.direction));
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
}

} // namespace search
} // namespace ai2048
