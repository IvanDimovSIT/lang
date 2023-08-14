all:
	clang++ main.cpp scanner/Scanner.cpp interpreter/InterpreterCalculator.cpp util/LiteralParser.cpp util/StringUtil.cpp util/TokenSubArrayFinder.cpp debug/DebugPrinter.cpp debug/ErrorPrinter.cpp interpreter/FunctionExtractor.cpp interpreter/Interpreter.cpp -o lang.out

buildtest:
	clang++ test.cpp scanner/Scanner.cpp interpreter/InterpreterCalculator.cpp util/LiteralParser.cpp util/StringUtil.cpp util/TokenSubArrayFinder.cpp debug/DebugPrinter.cpp debug/ErrorPrinter.cpp interpreter/FunctionExtractor.cpp interpreter/Interpreter.cpp  -o test.out

run: all
	./lang.out

test: buildtest
	./test.out

clean:
	rm -f lang.out
	rm -f test.out
