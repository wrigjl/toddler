
ALL=a.out tracer

all: $(ALL)

a.out: main.cc
	$(CXX) -O -g main.cc -o $@

tracer: tracer.c
	$(CC) -O -g tracer.c -o $@

