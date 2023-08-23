all:
	clang++ main.cpp scanner/Scanner.cpp interpreter/InterpreterCalculator.cpp util/LiteralParser.cpp util/StringUtil.cpp util/TokenSubArrayFinder.cpp debug/DebugPrinter.cpp debug/ErrorPrinter.cpp interpreter/FunctionExtractor.cpp interpreter/Interpreter.cpp interpreter/InterpreterIO.cpp util/RandomGenerator.cpp -o lang.out

buildtest:
	clang++ test.cpp scanner/Scanner.cpp interpreter/InterpreterCalculator.cpp util/LiteralParser.cpp util/StringUtil.cpp util/TokenSubArrayFinder.cpp debug/DebugPrinter.cpp debug/ErrorPrinter.cpp interpreter/FunctionExtractor.cpp interpreter/Interpreter.cpp interpreter/InterpreterIO.cpp util/RandomGenerator.cpp -o test.out

run: all
	./lang.out program.txt

test: buildtest
	./test.out

clean:
	rm -f lang.out
	rm -f test.out
