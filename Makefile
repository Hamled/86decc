##
# 86decc
#
# @file
# @version 0.1

CC=clang
CFLAGS=-g -Wall -Wextra -Werror -fPIC -fstack-protector
OUTPUTS=86decc

86decc: 86decc.c

build: 86decc

.PHONY: clean
clean:
	@rm -f $(OUTPUTS)

.PHONY: all
all: build

# end
