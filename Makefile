NAME := _stitch
SRC_DIR := src
TEST_DIR := test
SRC = $(wildcard $(SRC_DIR)/*.cpp)
INC = $(wildcard $(SRC_DIR)/*.hpp)
TEST = $(wildcard $(TEST_DIR)/*.cpp)

CXX := g++
CXX_FLAGS := -std=c++17 -Wall -fPIC
GTEST_FLAGS = -DGTEST -lgtest
PY_FLAGS := \
	-DPY -O3 -shared \
	`python3-config --includes --ldflags` \
	`python3 -m pybind11 --includes`


all: py

py: $(SRC) $(INC)
	$(CXX) $(SRC) -o $(NAME).so $(CXX_FLAGS) $(PY_FLAGS)

gtest: $(SRC) $(INC) $(TEST)
	$(CXX) $(SRC) $(TEST) -o $(NAME) $(CXX_FLAGS) $(GTEST_FLAGS)
	./$(NAME)

.PHONY: clean
clean:
	rm -rf $(NAME) $(NAME)*.so build __pycache__ .pytest_cache
