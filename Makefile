NAME := _stitch
SRC_DIR := src
SRC = $(wildcard $(SRC_DIR)/*.cpp)
INC = $(wildcard $(SRC_DIR)/*.hpp)

CXX := g++
CXX_FLAGS := -std=c++17 -Wall -fPIC
GTEST_FLAGS = -DGTEST -lgtest
PY_FLAGS := \
	-DPY -O3 -shared \
	`python3-config --includes --ldflags` \
	`python3 -m pybind11 --includes`


all: py

py: $(SRC) $(INC)
	@pip install pybind11
	$(CXX) $(CXX_FLAGS) $(PY_FLAGS) $(SRC) -o $(NAME).so

gtest: $(SRC) $(INC)
	$(CXX) $(CXX_FLAGS) $(GTEST_FLAGS) $(SRC) -o $(NAME)
	./$(NAME)

.PHONY: clean
clean:
	rm -rf $(NAME) $(NAME)*.so build __pycache__ .pytest_cache
