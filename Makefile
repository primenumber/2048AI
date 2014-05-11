CXXC	 		= clang++
CXXFLAGS	= -O3 --std=c++11 -Wall -Wextra -Weffc++
OBJS			= 2048ai.o grid.o random.o search.o
TARGET		= 2048ai

all: $(TARGET)

$(TARGET):$(OBJS)
	$(CXXC) $(OBJS) $(CXXFLAGS) -o $(TARGET)

clean:
	-rm -f 2048ai $(OBJS)

.cpp.o:
	$(CXXC) -c $< $(CXXFLAGS)

2048ai.o: grid.hpp random.hpp
grid.o: grid.hpp
random.o: random.hpp
search.o: search.hpp grid.hpp random.hpp
