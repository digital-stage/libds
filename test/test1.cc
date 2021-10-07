#include <gtest/gtest.h>

int testFunc()
{
  return 6;
}

TEST(FirstTestCase, SomeStupidTestFoo) {
  ASSERT_EQ(6, testFunc());
}