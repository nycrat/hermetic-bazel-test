#include "main/greeting.h"

#include <gtest/gtest.h>

TEST(GreetingTest, GreetsWorldByDefault) {
  EXPECT_EQ("Hello world!", get_greet("world"));
}

TEST(GreetingTest, GreetsNamedPerson) {
  EXPECT_EQ("Hello avah!", get_greet("avah"));
}

TEST(GreetingTest, HandlesEmptyName) {
  EXPECT_EQ("Hello !", get_greet(""));
}
