test: FORCE
	@python gen_tests.py
	@gcc -o test_exe test/hashtable_test.c test/.test_impl.c hashtable.c
	@./test_exe
	@rm -rf test/.test_impl.c test_exe

debug: FORCE
	@python gen_tests.py
	@gcc -g -o test_exe test/hashtable_test.c test/.test_impl.c hashtable.c
	@rm -rf test/.test_impl.c

demo: benchmark/hashtable_demo.cpp hashtable.c
	g++ -O3 benchmark/hashtable_demo.cpp hashtable.c -o benchmark/hashtable_demo
	@echo "usage: ./hashtable_demo <string length> <num strings (2^input)> <start from default size>"
	@echo "example: ./hashtable_demo 32 15 true -- 2^15 strings of length 32 in a hashtable starting at default size"

FORCE: ;