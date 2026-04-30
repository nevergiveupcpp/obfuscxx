// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2025-2026 nevergiveupcpp

// Copyright 2025-2026 nevergiveupcpp
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NGU_OBFUSCXX_METRICS_UNITTEST_H
#define NGU_OBFUSCXX_METRICS_UNITTEST_H

#include <array>

#include <gtest/gtest.h>
#include <obfuscxx/obfuscxx.h>

using namespace ngu;

TEST(ObfuscxxTest, IntegerValue) {
    obfxx(int) value{100};
    EXPECT_EQ(value.get(), 100);

    value = 50;
    EXPECT_EQ(value.get(), 50);
}

TEST(ObfuscxxTest, FloatValue) {
    obfxx(float) const value{1.5f};
    EXPECT_FLOAT_EQ(value.get(), 1.5f);
}

TEST(ObfuscxxTest, ArrayIteration) {
    obfxx(int[4]) const array{1, 2, 3, 4};
    constexpr int expected[] = {1, 2, 3, 4};

    for (int i{}; auto val : array) {
        EXPECT_EQ(val, expected[i++]);
    }
}

TEST(ObfuscxxTest, ToString) {
    constexpr auto narrow_str = obfxxo("frost on the morning grass");
    EXPECT_STREQ(narrow_str.to_string(), "frost on the morning grass");

    constexpr auto wide_str = obfxxo(L"frost on the morning grass");
    EXPECT_STREQ(wide_str.to_string(), L"frost on the morning grass");
}

TEST(ObfuscxxTest, PointerValue) {
    obfxx(int*) pointer{};
    pointer = new int{101};

    EXPECT_NE(pointer.get(), nullptr);
    EXPECT_EQ(*pointer.get(), 101);

    delete pointer.get();
}

TEST(ObfuscxxTest, ComparisonOperators) {
    obfxx(int) a{100};
    obfxx(int) b{100};
    obfxx(int) c{50};

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(c < a);
    EXPECT_TRUE(a > c);
    EXPECT_TRUE(a >= b);
    EXPECT_TRUE(c <= a);
}

TEST(ObfuscxxTest, ArithmeticOperators) {
    obfxx(int) a{10};
    obfxx(int) b{5};

    EXPECT_EQ(a + b, 15);
    EXPECT_EQ(a - b, 5);
    EXPECT_EQ(a * b, 50);
    EXPECT_EQ(a / b, 2);

    a += b;
    EXPECT_EQ(a.get(), 15);

    a -= b;
    EXPECT_EQ(a.get(), 10);
}

TEST(ObfuscxxTest, ObfuscationLevels) {
    obfxx(int, obf_level::low) const low{50};
    obfxx(int, obf_level::medium) const medium{50};
    obfxx(int, obf_level::high) const high{50};

    EXPECT_EQ(low.get(), 50);
    EXPECT_EQ(medium.get(), 50);
    EXPECT_EQ(high.get(), 50);
}

TEST(ObfuscxxTest, IntegerEdgeCases) {
    obfxx(int) const max_int{INT_MAX};
    obfxx(int) const min_int{INT_MIN};
    obfxx(int) const zero{0};
    obfxx(int) const negative{-12345};

    EXPECT_EQ(max_int.get(), INT_MAX);
    EXPECT_EQ(min_int.get(), INT_MIN);
    EXPECT_EQ(zero.get(), 0);
    EXPECT_EQ(negative.get(), -12345);
}

TEST(ObfuscxxTest, FloatEdgeCases) {
    obfxx(float) const zero{0.0f};
    obfxx(float) const negative{-3.14f};
    obfxx(float) const small{0.0001f};
    obfxx(float) const large{123456.789f};

    EXPECT_FLOAT_EQ(zero.get(), 0.0f);
    EXPECT_FLOAT_EQ(negative.get(), -3.14f);
    EXPECT_FLOAT_EQ(small.get(), 0.0001f);
    EXPECT_FLOAT_EQ(large.get(), 123456.789f);
}

TEST(ObfuscxxTest, ArrayOperators) {
    obfxx(int[5]) const array{10, 20, 30, 40, 50};

    EXPECT_EQ(array[0], 10);
    EXPECT_EQ(array[2], 30);
    EXPECT_EQ(array[4], 50);
    EXPECT_EQ(array.get(1), 20);
    EXPECT_EQ(array.get(3), 40);
    EXPECT_EQ(array.size(), 5);
}

TEST(ObfuscxxTest, ArraySet) {
    obfxx(int[3]) array{1, 2, 3};

    array.set(100, 0);
    array.set(200, 1);
    array.set(300, 2);

    EXPECT_EQ(array[0], 100);
    EXPECT_EQ(array[1], 200);
    EXPECT_EQ(array[2], 300);
}

TEST(ObfuscxxTest, ArrayCopyTo) {
    obfxx(int[5]) const array{1, 2, 3, 4, 5};
    int buffer[5]{};

    array.copy_to(buffer, array.size());

    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(buffer[i], i + 1);
    }
}

TEST(ObfuscxxTest, ToArray) {
    obfxx(int[4]) const array{0, 1, 2, 3};
    auto const plain_array = array.to_array();

    std::array<int, 4> buffer{};
    memcpy(buffer.data(), plain_array.get(), plain_array.size_bytes());

    EXPECT_EQ(buffer[0], 0);
    EXPECT_EQ(buffer[1], 1);
    EXPECT_EQ(buffer[2], 2);
    EXPECT_EQ(buffer[3], 3);
}

TEST(ObfuscxxTest, ArrayAssignment) {
    obfxx(int[3]) array{1, 2, 3};

    array = {10, 20, 30};

    EXPECT_EQ(array[0], 10);
    EXPECT_EQ(array[1], 20);
    EXPECT_EQ(array[2], 30);
}

TEST(ObfuscxxTest, DataIsEncrypted) {
    obfxx(int) const value{50};

    auto const raw_data = reinterpret_cast<const uint64_t*>(&value);
    volatile uint64_t encrypted = *raw_data;

    EXPECT_NE(encrypted, 50);
    EXPECT_EQ(value.get(), 50);
}

TEST(ObfuscxxTest, PointerOperators) {
    obfxx(int*) pointer{};
    pointer = new int{999};

    EXPECT_NE(pointer.get(), nullptr);
    EXPECT_EQ(*pointer.get(), 999);

    *pointer.get() = 111;
    EXPECT_EQ(*pointer.get(), 111);

    delete pointer.get();
}

TEST(ObfuscxxTest, EmptyString) {
    constexpr auto str = obfxxo("");
    EXPECT_STREQ(str.to_string(), "");
}

TEST(ObfuscxxTest, LongString) {
    constexpr auto str = obfxxo(
        "The river ran calmly past the old wooden bridge, where a few fishermen waited in the morning "
        "light, watching the wind move through the tall grass on the far bank while a heron stood "
        "still among the reeds."
    );

    EXPECT_STREQ(
        str.to_string(),
        "The river ran calmly past the old wooden bridge, where a few fishermen waited in the morning "
        "light, watching the wind move through the tall grass on the far bank while a heron stood "
        "still among the reeds."
    );
}

TEST(ObfuscxxTest, IteratorOperations) {
    obfxx(int[5]) const array{1, 2, 3, 4, 5};

    auto it = array.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);

    EXPECT_NE(it, array.end());

    int count{};
    for (auto iter = array.begin(); iter != array.end(); ++iter) {
        count++;
    }
    EXPECT_EQ(count, 5);
}

TEST(ObfuscxxTest, DifferentTypes) {
    obfxx(uint64_t) const uint64_value{0xFFFFFFFFFFFFFFFF};
    obfxx(int8_t) const int8_value{-127};
    obfxx(double) const double_value{3.141592653589793};

    EXPECT_EQ(uint64_value.get(), 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(int8_value.get(), -127);
    EXPECT_DOUBLE_EQ(double_value.get(), 3.141592653589793);
}

TEST(ObfuscxxTest, MultipleAssignments) {
    obfxx(int) value{10};

    value = 20;
    EXPECT_EQ(value.get(), 20);
    value = 30;
    EXPECT_EQ(value.get(), 30);
    value = 40;
    EXPECT_EQ(value.get(), 40);
}

TEST(ObfuscxxTest, ArrayEquality) {
    obfxx(int[3]) const a{1, 2, 3};
    obfxx(int[3]) const b{1, 2, 3};
    obfxx(int[3]) const c{1, 2, 4};

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
}

TEST(ObfuscxxTest, ImplicitConversion) {
    obfxx(int) const value{42};

    int x = value;
    EXPECT_EQ(x, 42);

    int result = value + 10;
    EXPECT_EQ(result, 52);
}

TEST(ObfuscxxTest, InlineMacroExpressions) {
    EXPECT_STREQ(obfxxs("river by the old bridge"), "river by the old bridge");
    EXPECT_STREQ(obfxxs(L"river by the old bridge"), L"river by the old bridge");
    EXPECT_STREQ(obfxxs("wind across the meadow", obf_level::medium), "wind across the meadow");
    EXPECT_STREQ(obfxxs(L"wind across the meadow", obf_level::medium), L"wind across the meadow");
    EXPECT_STREQ(obfxxs("birds on a wooden fence", obf_level::high), "birds on a wooden fence");
    EXPECT_STREQ(obfxxs(L"birds on a wooden fence", obf_level::high), L"birds on a wooden fence");

    EXPECT_EQ(obfxxv(52), 52);
    EXPECT_EQ(obfxxv(52, obf_level::medium), 52);
    EXPECT_EQ(obfxxv(52, obf_level::high), 52);

    EXPECT_FLOAT_EQ(obfxxv(3.14f), 3.14f);
    EXPECT_FLOAT_EQ(obfxxv(3.14f, obf_level::medium), 3.14f);
    EXPECT_FLOAT_EQ(obfxxv(-3.14f, obf_level::high), -3.14f);
}

#if defined(__clang__) || defined(__GNUC__)
TEST(ObfuscxxTest, UserDefinedLiterals) {
    EXPECT_STREQ("river by the old bridge"_obf, "river by the old bridge");
    EXPECT_STREQ(L"river by the old bridge"_obf, L"river by the old bridge");

    EXPECT_STREQ("wind across the meadow"_obfm, "wind across the meadow");
    EXPECT_STREQ(L"wind across the meadow"_obfm, L"wind across the meadow");

    EXPECT_STREQ("birds on a wooden fence"_obfh, "birds on a wooden fence");
    EXPECT_STREQ(L"birds on a wooden fence"_obfh, L"birds on a wooden fence");
}
#endif

#endif // NGU_OBFUSCXX_METRICS_UNIT_TEST_H