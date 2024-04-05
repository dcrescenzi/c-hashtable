#include "../hashtable.h"
#include "hashtable_test.h"

const char* RED = "\e[1;31m";
const char* GREEN = "\e[1;32m";
const char* WHITE = "\e[1;37m";
const char* RESET = "\x1b[0m";

void log_suite_start(const char* name)
{
    fprintf(stdout, "%sRUNNING SUITE : %s%s\n", WHITE, RESET, name);
}

void run_test_and_print(const char* name, const char* group, bool pass, int* fail, int* tot)
{
    //stats
    if(!pass) (*fail)++;
    (*tot)++;

    fprintf(stdout, "  %s[%s]%s %s.%s\n", pass ? GREEN : RED, pass ? "PASS" : "FAIL", RESET, group, name);
}

void print_cumulative_stats(int fail, int tot)
{
    fprintf(stdout, "[%d/%d] passed\n", tot-fail, tot);
    fprintf(stdout, "%s%s%s\n", fail == 0 ? GREEN : RED, fail == 0 ? "all tests passed successfully" : "not all tests passed", RESET);
}

