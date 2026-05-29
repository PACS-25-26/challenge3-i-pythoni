CXX := mpic++
TEST_CXX ?= g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -fopenmp -Iinclude

COMMON_SOURCES := \
	src/Grid.cpp \
	src/RowDecomposition.cpp \
	src/Functions.cpp \
	src/LocalSolver.cpp \
	src/JacobiSolver.cpp \
	src/VTKWriter.cpp \
	src/Error.cpp

COMMON_OBJECTS := $(COMMON_SOURCES:.cpp=.o)
PARALLEL_OBJECTS := $(COMMON_OBJECTS) src/ParallelJacobiSolver.o src/main.o
SERIAL_OBJECTS := $(COMMON_OBJECTS) src/serial_main.o

MPI_TEST_SOURCES := test/test_parallel_solver.cpp
TEST_SOURCES := $(filter-out $(MPI_TEST_SOURCES), $(wildcard test/test_*.cpp))
TEST_TARGETS := $(TEST_SOURCES:.cpp=) $(MPI_TEST_SOURCES:.cpp=)

TARGETS := laplace_solver laplace_serial

.PHONY: all clean parallel serial test

all: $(TARGETS)

parallel: laplace_solver

serial: laplace_serial

test: $(TEST_TARGETS)
	@for exec in $(TEST_TARGETS); do \
		echo -n "Executing $$exec... "; \
		if [ "$$exec" = "test/test_parallel_solver" ]; then \
			mpirun -np 1 ./$$exec && echo "SUCCESS" || { echo "FAILED"; exit 1; }; \
		else \
			./$$exec && echo "SUCCESS" || { echo "FAILED"; exit 1; }; \
		fi; \
	done

laplace_solver: $(PARALLEL_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(PARALLEL_OBJECTS)

laplace_serial: $(SERIAL_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(SERIAL_OBJECTS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test/%: test/%.cpp $(COMMON_SOURCES)
	$(TEST_CXX) $(CXXFLAGS) -o $@ $< $(COMMON_SOURCES)

test/test_parallel_solver: test/test_parallel_solver.cpp $(COMMON_SOURCES) src/ParallelJacobiSolver.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(COMMON_SOURCES) src/ParallelJacobiSolver.cpp

clean:
	rm -f src/*.o $(TARGETS) $(TEST_TARGETS)
