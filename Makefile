NAME := _stitch
SRC_DIR := src
TEST_DIR := test
SRC = $(wildcard $(SRC_DIR)/*.cpp)
INC = $(wildcard $(SRC_DIR)/*.hpp)
TEST = $(wildcard $(TEST_DIR)/*.cpp)
TEST_INC = $(wildcard $(TEST_DIR)/*.hpp)

CXX := g++
CXX_FLAGS := -std=c++17 -Wall -fPIC
GTEST_FLAGS = -DGTEST -lgtest
PY_FLAGS := \
	-DPY -O3 -shared \
	`python3-config --includes --ldflags` \
	`python3 -m pybind11 --includes`


all: $(NAME).so $(NAME).pyi

$(NAME).so: $(SRC) $(INC)
	$(CXX) $(SRC) -o $@ $(CXX_FLAGS) $(PY_FLAGS)

$(NAME).pyi: $(NAME).so
	stubgen -m $(NAME) --include-docstrings -o ./

pytest: $(NAME)_test.py $(NAME)_pytest.so $(NAME)_pytest.pyi
	python3 -m pytest -v -s

$(NAME)_pytest.so: $(SRC) $(INC) $(TEST) $(TEST_INC)
	$(CXX) $(SRC) $(TEST) -o $@ $(CXX_FLAGS) $(GTEST_FLAGS) $(PY_FLAGS)

$(NAME)_pytest.pyi: $(NAME)_pytest.so
	stubgen -m $(NAME)_pytest --include-docstrings -o ./

gtest: $(NAME)
	./$(NAME)

$(NAME): $(SRC) $(INC) $(TEST) $(TEST_INC)
	$(CXX) $(SRC) $(TEST) -o $@ $(CXX_FLAGS) $(GTEST_FLAGS)

.PHONY: clean
clean:
	rm -rf $(NAME) $(NAME).so $(NAME).pyi $(NAME)_pytest.so $(NAME)_pytest.pyi
	rm -rf __pycache__ .pytest_cache
