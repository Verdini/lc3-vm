# LC-3 VM Test Suite

This directory contains the test suite for the LC-3 Virtual Machine and Assembler. The test suite uses a simple C-based testing framework.

## Test Structure

- `test_framework.h`: Contains macros and utilities for writing and running tests
- `test/vm_tests.h`: VM test implementations
- `test/asm_tests.h`: Assembler test implementations
- `test.c`: Main test runner that includes all test modules

## Writing Tests

To write a new test, follow these steps:

1. Create a new test function in the appropriate test file or create a new test file
2. Each test function should return a `char*` (NULL for success, error message for failure)
3. Use the assertion macros from `test_framework.h` to validate your tests
4. Add your test function to the main() function using the RUN_TEST macro

Example:

```c
// In a test module file (e.g., tests/my_tests.h):
static char *test_my_new_feature(void) {
    // Initialize test data

    // Call the function being tested

    // Assert expected results
    ASSERT_INT_EQUAL("My error message", expected_value, actual_value);

    return NULL; // Return NULL for success
}

// Add the test to the test runner function
void run_my_tests(void) {
    printf("Running My Tests...\n\n");
    RUN_TEST(test_my_new_feature);
    // Add more tests here
}

// In test.c:
#include "test/my_tests.h"

int main(void) {
    // Other test modules...
    run_my_tests();
    // Other test modules...

    REPORT_TESTS();
}
```

## Running Tests

To run all tests:

```
make test
```

## Test Coverage

Currently, the test suite covers:

- VM initialization
- VM memory operations
- Basic assembler functionality

Future improvements:

- Add tests for each opcode implementation
- Add tests for program loading
- Add tests for error handling
- Add more comprehensive assembler tests
