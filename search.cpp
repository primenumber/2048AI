#include "search.hpp"
#include <cassert>
#include <utility>

int alpha_beta_host(const table_t& table, int alpha, int beta, int depth) {
  if (depth == 0) return static_score(table);
  for (int i = 0; i < 4; ++i) {
    table_t moved = move(table, i);
    int score = alpha_beta_player(std::move(moved), alpha, beta, depth - 1);
    if (score > alpha) {
      alpha = score;
      if (alpha >= beta) {
        return beta;
      }
    }
  }
  return alpha;
}

int alpha_beta_player(table_t&& table, int alpha, int beta, int depth) {
  if (depth == 0) return static_score(table);
  auto zeros = zero_list(table);
  int numbers[2] = {2, 4};
  for (auto tile : zeros) {
    for (int i = 0; i < 2; ++i) {
      table[tile.first][tile.second] = numbers[i];
      int score = alpha_beta_host(table, alpha, beta, depth - 1);
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
  int kDepth = 5;
  for (int i = 0; i < 4; ++i) {
    if (!movable(table, i)) continue;
    table_t moved = move(table, i);
    int score = alpha_beta_player(std::move(moved), alpha, beta, kDepth - 1);
    if (score > alpha) {
      alpha = score;
      optimal = i;
    }
  }
  assert(optimal != -1);
  return optimal;
}

int search(const table_t& table) {
  return alpha_beta_search(table);
}
