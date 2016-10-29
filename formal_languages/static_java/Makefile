BISON=bison
FLEX=flex
CXX=g++

all: static_java

static_java.tab.c static_java.tab.h: static_java.y
	$(BISON) -d $<

lex.yy.c: static_java.l
	$(FLEX) $<

static_java: lex.yy.c static_java.tab.c static_java_main.cpp static_java_run.cpp
	$(CXX) $^ -o $@

clean:
	rm -f static_java.tab.c static_java.tab.h lex.yy.c static_java
