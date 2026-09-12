#include "solution.h"
#include <gtest/gtest.h>

using namespace hftu;

TEST(StringMap, InsertFour) {
    StringMap sm;
    sm.insert("key1", 4, 42);
    sm.insert("key2", 4, 84);

    const uint32_t* value1 = sm.find("key1", 4);
    ASSERT_NE(value1, nullptr);
    EXPECT_EQ(*value1, 42);

    const uint32_t* value2 = sm.find("key2", 4);
    ASSERT_NE(value2, nullptr);
    EXPECT_EQ(*value2, 84);

    const uint32_t* value3 = sm.find("key3", 4);
    EXPECT_EQ(value3, nullptr);
}


TEST(StringMap, InsertSizeOne) {
    StringMap sm;
    sm.insert("a", 1, 42);
    sm.insert("b", 1, 84);

    const uint32_t* value1 = sm.find("a", 1);
    ASSERT_NE(value1, nullptr);
    EXPECT_EQ(*value1, 42);

    const uint32_t* value2 = sm.find("b", 1);
    ASSERT_NE(value2, nullptr);
    EXPECT_EQ(*value2, 84);

    const uint32_t* value3 = sm.find("c", 1);
    EXPECT_EQ(value3, nullptr);
}

TEST(StringMap, InsertSizeTwo) {
    StringMap sm;
    sm.insert("ab", 2, 42);
    sm.insert("cd", 2, 84);

    const uint32_t* value1 = sm.find("ab", 2);
    ASSERT_NE(value1, nullptr);
    EXPECT_EQ(*value1, 42);

    const uint32_t* value2 = sm.find("cd", 2);
    ASSERT_NE(value2, nullptr);
    EXPECT_EQ(*value2, 84);

    const uint32_t* value3 = sm.find("ef", 2);
    EXPECT_EQ(value3, nullptr);
}

TEST(StringMap, InsertSizeThree) {
    StringMap sm;
    sm.insert("abc", 3, 42);
    sm.insert("def", 3, 84);

    const uint32_t* value1 = sm.find("abc", 3);
    ASSERT_NE(value1, nullptr);
    EXPECT_EQ(*value1, 42);

    const uint32_t* value2 = sm.find("def", 3);
    ASSERT_NE(value2, nullptr);
    EXPECT_EQ(*value2, 84);

    const uint32_t* value3 = sm.find("ghi", 3);
    EXPECT_EQ(value3, nullptr);
}

TEST(StringMap, InsertSizeSixTeen) {
    StringMap sm;
    sm.insert("whatiztententenc", 16, 57);
    sm.insert("1234567890123456", 16, 108);

    const uint32_t* value1 = sm.find("whatiztententenc", 16);
    ASSERT_NE(value1, nullptr);
    EXPECT_EQ(*value1, 57);

    const uint32_t* value2 = sm.find("1234567890123456", 16);
    ASSERT_NE(value2, nullptr);
    EXPECT_EQ(*value2, 108);

    const uint32_t* value3 = sm.find("6543210987654321", 16);
    EXPECT_EQ(value3, nullptr);
}

TEST(StringMap, EmptyString){
    StringMap sm;
    sm.insert("", 0, 99);

    const uint32_t* value1 = sm.find("", 0);
    ASSERT_NE(value1, nullptr);
    EXPECT_EQ(*value1, 99);
}

TEST(StringMap, EscapeCharacters){
    StringMap sm;

    sm.insert("l\0a", 3, 123);
    const uint32_t* value1 = sm.find("l\0a", 3);
    ASSERT_NE(value1, nullptr);
    EXPECT_EQ(*value1, 123);
}

TEST(StringMap, SpecialCharacters){
    StringMap sm;

    sm.insert("!@#$%^&*()", 10, 456);
    const uint32_t* value1 = sm.find("!@#$%^&*()", 10);
    ASSERT_NE(value1, nullptr);
    EXPECT_EQ(*value1, 456);
}