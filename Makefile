CXX				= clang++
CXXFLAGS	= -O2 -march=native -mtune=native --std=c++17 -Wall -Wextra -Weffc++
LIBOBJS			= grid.o score.o search.o random.o game.o gamedag.o simpleSearch.o tableSearch.o
OBJS			= 2048ai.o $(LIBOBJS)
CALCOBJS	= calc.o $(LIBOBJS)
TARGET		= 2048ai
CALC			= calcer

all: $(TARGET)

$(TARGET):$(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) -o $(TARGET)

$(CALC):$(CALCOBJS)
	$(CXX) $(CALCOBJS) $(CXXFLAGS) -o $(CALC)

vtrain:value_train.cpp grid.cpp grid.hpp score.cpp score.hpp random.cpp random.hpp
	clang++ value_train.cpp grid.cpp score.cpp random.cpp --std=c++11 -o vtrain -Wall -Wextra -Weffc++

clean:
	-rm -f 2048ai $(OBJS)

.cpp.o:
	$(CXX) -c $< $(CXXFLAGS)

2048ai.o: gamedag.hpp game.hpp grid.hpp score.hpp search.hpp random.hpp
grid.o: grid.hpp
score.o: score.hpp grid.hpp
search.o: search.hpp score.hpp grid.hpp random.hpp
random.o: random.hpp
game.o: game.hpp grid.hpp
gamedag.o: gamedag.hpp game.hpp grid.hpp
calc.o: grid.hpp
simpleSearch.o: search.hpp grid.hpp
tableSearch.o: search.hpp grid.hpp
