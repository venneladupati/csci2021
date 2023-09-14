# CSCI 2021 lab Makefile
AN=lab01

# -Wno-comment: disable warnings for multi-line comments, present in some tests
CFLAGS = -Wall -Wno-comment -Werror -g
CC = gcc $(CFLAGS)
SHELL = /bin/bash
CWD = $(shell pwd | sed 's/.*\///g')

.PHONY: clean clean-tests test test-quiz test-code test-setup zip

all: hello_c list_main

hello_c: hello_c.c
	$(CC) -o $@ $^

list_main: list_main.o list_funcs.o list.h
	$(CC) -o $@ $^

list_main.o: list_main.c list.h
	$(CC) -c $<

list_funcs.o: list_funcs.c list.h
	$(CC) -c $<

# cleaning target to remove compiled programs/objects
clean:
	rm -f hello_c list_main *.o

clean-tests:
	rm -rf test_results

help:
	@echo 'Typical usage is:'
	@echo '  > make                          # build all programs'
	@echo '  > make clean                    # remove all compiled items'
	@echo '  > make zip                      # create a zip file for submission'
	@echo '  > make test                     # run all tests'
	@echo '  > make test-quiz                # run tests that verify quiz answers'
	@echo '  > make test-code                # run tests that verify code is correct'

# 'make zip' to create zip file for submission
zip: clean clean-tests
	rm -f $(AN)-code.zip
	cd .. && zip "$(CWD)/$(AN)-code.zip" -r "$(CWD)" -x "$(CWD)/test_cases/*" "$(CWD)/testius"
	@echo Zip created in $(AN)-code.zip
	@if (( $$(stat -c '%s' $(AN)-code.zip) > 10*(2**20) )); then echo "WARNING: $(AN)-code.zip seems REALLY big, check there are no abnormally large test files"; du -h $(AN)-code.zip; fi
	@if (( $$(unzip -t $(AN)-code.zip | wc -l) > 256 )); then echo "WARNING: $(AN)-code.zip has 256 or more files in it which may cause submission problems"; fi

# testing targets below
ifdef testnum
test: test-setup hello_c list_main
	./testius test_cases/tests.json -v -n "$(testnum)"
else
test: test-setup hello_c list_main
	./testius test_cases/tests.json -v
endif

test-quiz: test-setup QUESTIONS.txt
	./testius test_cases/tests.json -v -n 1

test-code: test-setup hello_c list_main
	./testius test_cases/tests.json -v -n 2-6

test-setup:
	@chmod u+x ./testius
	@chmod u+x test_cases/resources/socrates
