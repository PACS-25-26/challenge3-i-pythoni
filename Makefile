CXX := mpic++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -fopenmp -Iinclude

COMMON_SOURCES := \
	src/Grid.cpp \
	src/RowDecomposition.cpp \
	src/Functions.cpp \
	src/JacobiSolver.cpp \
	src/VTKWriter.cpp \
	src/Error.cpp

COMMON_OBJECTS := $(COMMON_SOURCES:.cpp=.o)
PARALLEL_OBJECTS := $(COMMON_OBJECTS) src/main.o
SERIAL_OBJECTS := $(COMMON_OBJECTS) src/serial_main.o

TARGETS := laplace_solver laplace_serial

.PHONY: all clean parallel serial

all: $(TARGETS)

parallel: laplace_solver

serial: laplace_serial

laplace_solver: $(PARALLEL_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(PARALLEL_OBJECTS)

laplace_serial: $(SERIAL_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(SERIAL_OBJECTS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGETS)
