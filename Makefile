CXX = g++
CXXFLAGS = -std=c++2b -Wall -g -pthread # Añadido -pthread
LDFLAGS = -pthread # Añadido -pthread

TARGET = main
SRCS = main.cpp \
	   demos/ListsDemo.cpp \
	   demos/DemoBinaryTree.cpp \
	   demos/DemoAVL.cpp \
	   demos/BTreeDemo.cpp \
	   demos/stdmapDemo.cpp \
	   demos/DigitalTrieDemo.cpp \
	   demos/HashPoissonDemo.cpp \
	   demos/DijkstraDemo.cpp \
	   demos/LRUCacheDemo.cpp
# 	   DemoVector.cpp
# macros.cpp vector.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

demos/%.o: demos/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean