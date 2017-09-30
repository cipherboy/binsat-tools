CC ?= gcc
CFLAGS ?= -pedantic -std=c99 -Werror -Wall -Wextra -fdiagnostics-color=always -O3

all: build test

build: bin2sat sat2bin bin2bcsoln

clean:
	rm -f bin2sat bin2bcsoln sat2bin

bin2sat: bin2sat.c
	$(CC) bin2sat.c -o bin2sat $(CFLAGS)

sat2bin: sat2bin.c
	$(CC) sat2bin.c -o sat2bin $(CFLAGS)

bin2bcsoln: bin2bcsoln.c
	$(CC) bin2bcsoln.c -o bin2bcsoln $(CFLAGS)

test:
	mkdir -p .tmp
	grep '^v' tests/problem-sat.out  | sed 's/v //g' | grep -io '[-0-9]*' | sort -n > .tmp/problem-sat.out1
	./sat2bin < tests/problem-sat.out > .tmp/problem-sat.bin || true
	./bin2sat < .tmp/problem-sat.bin | sed 's/v //g' | grep -io '[-0-9]*' | sort -n > .tmp/problem-sat.out2
	diff .tmp/problem-sat.out1 .tmp/problem-sat.out2
