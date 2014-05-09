#include <map>
#include "grid.hpp"
#include "random.hpp"

namespace ai2048 {
namespace search {

struct Value {
  Value(int v, int a, int b) : value(v), alpha(a), beta(b) {};
  int value;
  int alpha;
  int beta;
};

int search(const grid::Grid& grid);
int Monte_Carlo_search(const grid::Grid& grid);

} // namespace search
} // namespace ai2048
