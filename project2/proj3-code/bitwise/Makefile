CC = gcc
LIBS = -lm

.PHONY: all test clean test-setup zip

all: btest_server btest_driver fshow ishow

btest_driver: btest_driver.c bits_impl.h bits.s
	$(CC) -Wall -Werror -g -o $@ btest_driver.c bits.s

btest_server: btest.h bits_test.h btest.c decl.c tests.c
	gcc -O -Wall -m32 -lm -o $@ btest.c decl.c tests.c

fshow: fshow.c
	gcc -Wall -m32 -o $@ $^

ishow: ishow.c
	gcc -Wall -m32 -o $@ $^

test-setup:
	@chmod u+x cc_check check_bitwise

test: test-setup all
	./check_bitwise

clean:
	rm -f *.o btest_driver btest_server fshow ishow *~

zip:
	@echo "Error: Do not run make zip in the bitwise subdirectory. Run it in the project's top-level directory."
