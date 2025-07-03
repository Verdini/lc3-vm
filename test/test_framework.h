#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT_TRUE(message, test)   \
  do {                               \
    if (!(test)) {                   \
      printf("FAIL: %s\n", message); \
      return message;                \
    }                                \
    return NULL;                     \
  } while (0)

#define ASSERT_INT_EQUAL(message, expected, actual)                     \
  do {                                                                  \
    if ((expected) != (actual)) {                                       \
      printf("FAIL: %s\nExpected: %d, Actual: %d\n", message, expected, \
             actual);                                                   \
      return message;                                                   \
    }                                                                   \
    return NULL;                                                        \
  } while (0)

#define ASSERT_UINT16_EQUAL(message, expected, actual)                \
  do {                                                                \
    if ((expected) != (actual)) {                                     \
      printf("FAIL: %s\nExpected: 0x%04X, Actual: 0x%04X\n", message, \
             (uint16_t)(expected), (uint16_t)(actual));               \
      return message;                                                 \
    }                                                                 \
    return NULL;                                                      \
  } while (0)

#define ASSERT_MEM_EQUAL(message, expected, actual, size)          \
  do {                                                             \
    if (memcmp((expected), (actual), (size)) != 0) {               \
      printf("FAIL: %s\nMemory contents do not match\n", message); \
      return message;                                              \
    }                                                              \
    return NULL;                                                   \
  } while (0)

#define RUN_TEST(test_fn)             \
  do {                                \
    char *message = test_fn();        \
    tests_run++;                      \
    if (message) {                    \
      tests_failed++;                 \
    } else {                          \
      tests_passed++;                 \
      printf("PASS: %s\n", #test_fn); \
    }                                 \
  } while (0)

#define REPORT_TESTS()                          \
  do {                                          \
    printf("\n----- TEST RESULTS -----\n");     \
    printf("Tests run: %d\n", tests_run);       \
    printf("Tests passed: %d\n", tests_passed); \
    printf("Tests failed: %d\n", tests_failed); \
    printf("------------------------\n");       \
    return tests_failed;                        \
  } while (0)

#endif /* TEST_FRAMEWORK_H */
