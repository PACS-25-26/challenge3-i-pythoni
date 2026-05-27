CXX := mpic++
TEST_CXX ?= g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -fopenmp -Iinclude

COMMON_SOURCES := \
	src/Grid.cpp \
	src/RowDecomposition.cpp \
	src/Functions.cpp \
	src/JacobiSolver.cpp \
	src/VTKWriter.cpp \
	src/Error.cpp

COMMON_OBJECTS := $(COMMON_SOURCES:.cpp=.o)
PARALLEL_OBJECTS := $(COMMON_OBJECTS) src/ParallelJacobiSolver.o src/main.o
SERIAL_OBJECTS := $(COMMON_OBJECTS) src/serial_main.o

TEST_SOURCES := $(wildcard test/test_*.cpp)
TEST_TARGETS := $(TEST_SOURCES:.cpp=)

TARGETS := laplace_solver laplace_serial

.PHONY: all clean parallel serial test

all: $(TARGETS)

parallel: laplace_solver

serial: laplace_serial

test: $(TEST_TARGETS)
	@for exec in $(TEST_TARGETS); do \
		echo -n "Executing $$exec... "; \
		./$$exec && echo "SUCCESS" || { echo "FAILED"; exit 1; }; \
	done

laplace_solver: $(PARALLEL_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(PARALLEL_OBJECTS)

laplace_serial: $(SERIAL_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(SERIAL_OBJECTS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test/%: test/%.cpp $(COMMON_SOURCES)
	$(TEST_CXX) $(CXXFLAGS) -o $@ $< $(COMMON_SOURCES)

clean:
	rm -f src/*.o $(TARGETS) $(TEST_TARGETS)
