#include <map>
#include "grid.hpp"
#include "random.hpp"
#include "score.hpp"

namespace ai2048 {
namespace search {


int search(const grid::Grid& grid);

std::array<int, 4> simple_search(const grid::Grid& grid);

} // namespace search
} // namespace ai2048
