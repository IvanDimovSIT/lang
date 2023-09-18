ifeq ($(OS),Windows_NT)
	EXE := .exe
else
	EXE := .out
endif

CXX := clang++
CXXFLAGS := -flto -O3 -std=c++14 -pthread -march=native -mtune=native
DEBUG_CXXFLAGS := -g -std=c++14 -pthread

SRC := \
	scanner/Preprocessor.cpp \
	scanner/Scanner.cpp \
	token/OperatorArguments.cpp \
	token/TokenSyntax.cpp \
	interpreter/InterpreterCalculator.cpp \
	util/FileReader.cpp \
	util/LiteralParser.cpp \
	util/StringUtil.cpp \
	util/TokenSubArrayFinder.cpp \
	debug/DebugPrinter.cpp \
	debug/ErrorPrinter.cpp \
	debug/REPLPreprocessorErrorPrinter.cpp \
	interpreter/FunctionExtractor.cpp \
	interpreter/Interpreter.cpp \
	interpreter/InterpreterIO.cpp \
	util/RandomGenerator.cpp \
	main/REPL.cpp

LANG_SRC := main/main.cpp $(SRC)
TEST_SRC := main/test.cpp $(SRC)

LANG_EXE := lang$(EXE)
TEST_EXE := test$(EXE)

all: $(LANG_EXE)

buildtest: $(TEST_EXE)

$(LANG_EXE): $(LANG_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(LANG_SRC)

$(TEST_EXE): $(TEST_SRC)
	$(CXX) $(DEBUG_CXXFLAGS) -o $@ $(TEST_SRC)

run: all
	./$(LANG_EXE)

test: buildtest
	./$(TEST_EXE)

clean:
	rm -f $(LANG_EXE) $(TEST_EXE)
