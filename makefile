all: clean
	flex scanner.lex
	bison -d parser.ypp
	g++ -std=c++17 -o hw5 *.c *.cpp staff/*.cpp symbols/*.cpp
clean:
	rm -f lex.yy.c
	rm -f parser.tab.*pp
	rm -f hw5
.PHONY: all clean