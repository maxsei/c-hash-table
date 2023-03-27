.PHONY:test
test:
	gcc -o test libhashtable.c -lcheck -D TESTING && ./test
	rm test
