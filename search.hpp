#include "grid.hpp"
#include "score.hpp"

int search(const table_t& table);

int alpha_beta_player(const table_t& table, int alpha, int beta, int depth);
int alpha_beta_host(table_t&& table, int alpha, int beta, int depth);
int alpha_beta_search(const table_t& table);
int Monte_Carlo_search(const table_t& table);
