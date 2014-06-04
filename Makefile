CXXC	 		= clang++
CXXFLAGS	= -O3 --std=c++11 -Wall -Wextra -Weffc++
OBJS			= 2048ai.o grid.o score.o search.o random.o game.o simpleSearch.o
TARGET		= 2048ai

all: $(TARGET)

$(TARGET):$(OBJS)
	$(CXXC) $(OBJS) $(CXXFLAGS) -o $(TARGET)

clean:
	-rm -f 2048ai $(OBJS)

.cpp.o:
	$(CXXC) -c $< $(CXXFLAGS)

2048ai.o: game.hpp grid.hpp score.hpp search.hpp random.hpp
grid.o: grid.hpp
score.o: score.hpp grid.hpp
search.o: search.hpp score.hpp grid.hpp random.hpp
random.o: random.hpp
game.o: game.hpp grid.hpp
