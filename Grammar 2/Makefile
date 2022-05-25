output: lex.yy.c y.tab.c y.tab.h parser.h Compiler.c linker
	gcc Compiler.c lex.yy.c y.tab.c -o compiler
	./compiler<$(input)
	./linker
	cp output.xsm B190010CS.xsm
	rm prelinker.xsm
	rm output.xsm

lex.yy.c: lexer.l
	lex lexer.l

y.tab.c: parser.y
	yacc -d parser.y

linker: Linker.c
	gcc Linker.c -o linker

clean:
	rm *.xsm
	rm y.tab.c
	rm lex.yy.c
	rm y.tab.h
	rm compiler
	rm linker



