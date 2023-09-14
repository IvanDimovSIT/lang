all:
	clang++ -flto -O3 -std=c++14 -pthread -march=native -mtune=native -o lang.out\
	 main/main.cpp\
	 main/REPL.cpp\
	 scanner/Scanner.cpp\
	 interpreter/InterpreterCalculator.cpp\
	 util/LiteralParser.cpp\
	 util/StringUtil.cpp\
	 util/TokenSubArrayFinder.cpp\
	 debug/DebugPrinter.cpp\
	 debug/ErrorPrinter.cpp\
	 interpreter/FunctionExtractor.cpp\
	 interpreter/Interpreter.cpp\
	 interpreter/InterpreterIO.cpp\
	 util/RandomGenerator.cpp

buildtest:
	clang++ -g -std=c++14 -pthread -o test.out\
	 main/test.cpp\
	 scanner/Scanner.cpp\
	 interpreter/InterpreterCalculator.cpp\
	 util/LiteralParser.cpp\
	 util/StringUtil.cpp\
	 util/TokenSubArrayFinder.cpp\
	 debug/DebugPrinter.cpp\
	 debug/ErrorPrinter.cpp\
	 interpreter/FunctionExtractor.cpp\
	 interpreter/Interpreter.cpp\
	 interpreter/InterpreterIO.cpp\
	 util/RandomGenerator.cpp

run: all
	./lang.out

test: buildtest
	./test.out

clean:
	rm -f lang.out
	rm -f test.out
