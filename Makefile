2048ai:2048ai.cpp grid.cpp grid.hpp score.cpp score.hpp search.hpp search.cpp
	clang++ 2048ai.cpp grid.cpp score.cpp search.cpp random.cpp --std=c++11 -o 2048ai -Wall -Wextra -Weffc++
