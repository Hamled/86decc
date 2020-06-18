##
# 86decc
#
# @file
# @version 0.1

CC=clang
CFLAGS= -lm -g -Wall -Wextra -Werror -fPIC -fstack-protector
OUTPUTS=86decc 86decc-test

86decc: main.c debug.c 86decc.c
86decc-test: test.c 86decc.c
	$(CC) $(CFLAGS) -o $@ $^

build: 86decc
test: 86decc-test
	./86decc-test

.PHONY: clean
clean:
	@rm -f $(OUTPUTS)

.PHONY: all
all: build test

# end
