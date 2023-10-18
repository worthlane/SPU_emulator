CXX = g++-13
EXECUTABLE = asm
CXXFLAGS =  -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
			-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts       \
			-Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal  \
			-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline          \
			-Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked            \
			-Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo  \
			-Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn                \
			-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default      \
			-Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast           \
			-Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing   \
			-Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation    \
			-fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192         \
			-Wstack-usage=8192 -fPIE -Werror=vla
BUILD_DIR = ../build/bin
OBJECTS_DIR = ../build/asm
COMMON_DIR = ../common
SOURCES = main.cpp assembler.cpp
OUT_SOURCES = ../common/log_funcs.cpp ../common/errors.cpp ../common/input_and_output.cpp ../common/asm_input_and_output.cpp
OBJECTS = $(SOURCES:%.cpp=$(OBJECTS_DIR)/%.o)
OUT_OBJECTS = $(OUT_SOURCES:$(COMMON_DIR)/%.cpp=$(OBJECTS_DIR)/%.o)

.PHONY: all
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(OUT_OBJECTS)
	$(CXX) $^ -o $@ $(CXXFLAGS)

$(OBJECTS_DIR)/%.o: $(COMMON_DIR)/%.cpp
	$(CXX)  -c $^ -o $@ $(CXXFLAGS)

$(OBJECTS_DIR)/%.o: %.cpp
	$(CXX)  -c $^ -o $@ $(CXXFLAGS)

.PHONY: clean makedirs test

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS_DIR)/*.o *.o
	rm -rf *.log

makedirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJECTS_DIR)

test:
	$(CXX) $(CXXFLAGS) $(SOURCES)
