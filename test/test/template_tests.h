#ifndef TEMPLATE_TESTS_H
#define TEMPLATE_TESTS_H

#include <stdio.h>

#include "../test_framework.h"
// Include any other needed headers

// Test function template
static char *test_example(void) {
  // Setup test data

  // Call the function being tested

  // Assert expected results
  ASSERT_TRUE("Example test passes", 1);

  return NULL;
}

// Add more test functions here

// Run all tests in this module
void run_template_tests(void) {
  printf("Running Template Tests...\n\n");
  RUN_TEST(test_example);
  // Add more test calls here
}

#endif /* TEMPLATE_TESTS_H */
