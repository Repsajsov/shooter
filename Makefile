CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++23 $(shell pkg-config nlohmann_json)
TARGET = game

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean: 
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
