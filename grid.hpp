#include <array>
#include <utility>
#include <vector>

using table_t = std::array<std::array<int, 4>, 4>;

table_t rotate(table_t table, int direction);
bool movable(const table_t& table, int direction);
table_t move(const table_t& table, int direction);
std::vector<std::pair<int, int>> zero_list(const table_t& table);
int sum_grid(const table_t& table);
