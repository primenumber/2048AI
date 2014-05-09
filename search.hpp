#include <map>
#include "grid.hpp"
#include "random.hpp"
#include "score.hpp"

namespace ai2048 {
namespace search {

int search(const grid::Grid& grid);

int alpha_beta_host(grid::Grid grid, std::map<grid::Grid, int>& scores, int alpha, int beta, int depth);
int alpha_beta_search(const grid::Grid& grid);
int Monte_Carlo_search(const grid::Grid& grid);

} // namespace search
} // namespace ai2048
