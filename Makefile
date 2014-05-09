2048ai:2048ai.cpp grid.cpp grid.hpp score.cpp score.hpp search.hpp search.cpp random.hpp random.cpp
	clang++ 2048ai.cpp grid.cpp score.cpp search.cpp random.cpp --std=c++11 -o 2048ai -Wall -Wextra -Weffc++

vtrain:value_train.cpp grid.cpp grid.hpp score.cpp score.hpp random.cpp random.hpp
	clang++ value_train.cpp grid.cpp score.cpp random.cpp --std=c++11 -o vtrain -Wall -Wextra -Weffc++
