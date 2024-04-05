import re
import shutil
import os

TEST_SKELETON  = "test/.test_skeleton.c"
TEST_IMPL      = "test/.test_impl.c"
TEST_INTERFACE = "test/hashtable_test.h"
SUITE_TRIGGER  = "SUITE"

def extract_function_name(line):
    pattern = r'bool\s+(\w+)\s*\(\s*\);'
    match = re.match(pattern, line)
    if match:
        return match.group(1)
    else:
        return None

def copy_and_rename_file(original_filename, new_filename):
    current_directory = os.getcwd()
    original_path = os.path.join(current_directory, original_filename)
    new_path = os.path.join(current_directory, new_filename)
    shutil.copy(original_path, new_path)

def append_line_to_file(filename, line):
    with open(filename, "a") as file:
        file.write(line + "\n")

def log_suite_start(suite):
    append_line_to_file(TEST_IMPL, '\tlog_suite_start("{}");'.format(suite))

def add_test(name, suite):
    append_line_to_file(TEST_IMPL, '\trun_test_and_print("{}", "{}", {}(), &fail, &tot);'.format(name, suite, name))

copy_and_rename_file(TEST_SKELETON, TEST_IMPL)
append_line_to_file(TEST_IMPL, "int main(int argc, char** argv)\n{\n\tint fail = 0, tot = 0;")

with open(TEST_INTERFACE) as f:
    lines = f.readlines()
    cur_suite = "general"
    for line in lines:
        if SUITE_TRIGGER in line:
            suite = line.strip().split('=')[1].strip()
            cur_suite = suite
            log_suite_start(cur_suite)
        else:
            func = extract_function_name(line.strip())
            if func is None: continue
            add_test(func, cur_suite)

append_line_to_file(TEST_IMPL, "\tprint_cumulative_stats(fail, tot);\n}")