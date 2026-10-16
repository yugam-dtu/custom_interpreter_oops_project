CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = interpreter
SRC = main.cpp

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) demo.hb

test: $(TARGET)
	@echo "=== Running Baseline Test ==="
	@printf "Tester\n" | ./$(TARGET) demo.hb
	@echo "\n=== Running Example 01: Hello ==="
	@./$(TARGET) examples/01_hello.hb
	@echo "\n=== Running Example 02: Math Expressions ==="
	@./$(TARGET) examples/02_math_expressions.hb
	@echo "\n=== Running Example 03: Conditionals ==="
	@./$(TARGET) examples/03_conditionals.hb
	@echo "\nAll tests completed successfully!"

clean:
	rm -f $(TARGET) bro *.o
