#include "format.hpp"
#include <gtest/gtest.h>

struct CustomIncorrectClass {
  std::string message_;
  CustomIncorrectClass(const std::string &message) : message_(message) {}
  operator std::string() const {
    std::ostringstream oss;
    oss << message_;
    return oss.str();
  }
};

struct CustomCorrectClass {
  std::string message_;
  CustomCorrectClass(const std::string &message) : message_(message) {}
  operator std::string() const {
    std::ostringstream oss;
    oss << message_;
    return oss.str();
  }
};

inline std::ostream &operator<<(std::ostream &os, const CustomCorrectClass &c) {
  return os << static_cast<std::string>(c);
}

TEST(TestUtils, test_result_with_correct_custom_class) {
  auto text = format("{1}+{1}+{2} != {0}", 2, "one", CustomCorrectClass("5"));
  ASSERT_EQ(text, "one+one+5 != 2");
}

TEST(TestUtils, test_result_one) {
  auto text = format("{1}+{1}+{2} != {0}", 2, "one", 5);
  ASSERT_EQ(text, "one+one+5 != 2");
}

TEST(TestUtils, test_result_two) {
  auto text = format("{1}+{1} = {0}", 2, "one");
  ASSERT_EQ(text, "one+one = 2");
}

TEST(TestUtils, test_result_incorrect_custom_class) {
  try {
    auto text =
        format("{1}+{1}+{2} != {0}", 2, "one", CustomIncorrectClass("5"));
    FAIL() << "[Error] Unsupported type for << (at format.hpp:31)";
  } catch (const std::runtime_error &err) {
    EXPECT_STREQ("[Error] Unsupported type for << (at format.hpp:31)",
                 err.what());
  }
}

TEST(TestUtils, test_result_zero_params) {
  try {
    auto text = format();
    FAIL() << "[Error] Zero params\n (at format.hpp:59)";
  } catch (const std::runtime_error &err) {
    EXPECT_STREQ("[Error] Zero params\n (at format.hpp:59)", err.what());
  }
}

TEST(TestUtils, test_result_incorrect_left_brace) {
  try {
    auto text = format("{1}+{{1} = {0}", 2, "one");
    FAIL()
        << "[Error] Incorrect '{': stil open another '{'\n (at format.hpp:84)";
  } catch (const std::runtime_error &err) {
    EXPECT_STREQ(
        "[Error] Incorrect '{': stil open another '{'\n (at format.hpp:84)",
        err.what());
  }
}

TEST(TestUtils, test_result_incorrect_right_brace) {
  try {
    auto text = format("{1}+{1}} = {0}", 2, "one");
    FAIL() << "[Error] Incorrect '}': without init '{'\n (at format.hpp:70)";
  } catch (const std::runtime_error &err) {
    EXPECT_STREQ("[Error] Incorrect '}': without init '{'\n (at format.hpp:70)",
                 err.what());
  }
}

TEST(TestUtils, test_result_incorrect_final_right_brace) {
  try {
    auto text = format("{1}+{1} = {0", 2, "one");
    FAIL() << "[Error] Incorrect '}': does not exist final '}'\n (at "
              "format.hpp:88)";
  } catch (const std::runtime_error &err) {
    EXPECT_STREQ(
        "[Error] Incorrect '}': does not exist final '}'\n (at format.hpp:88)",
        err.what());
  }
}

TEST(TestUtils, test_result_incorrect_index) {
  try {
    auto text = format("{1}+{4} = {0}", 2, "one");
    FAIL() << "[Error] Index does not exist\n (at format.hpp:99)";
  } catch (const std::runtime_error &err) {
    EXPECT_STREQ("[Error] Index does not exist\n (at format.hpp:99)",
                 err.what());
  }
}

TEST(TestUtils, test_result_incorrect_conversation) {
  try {
    auto text = format("{1}+{1a} = {0}", 2, "one");
    FAIL() << "[Error] Conversion error\n (at format.hpp:77)";
  } catch (const std::runtime_error &err) {
    EXPECT_STREQ("[Error] Conversion error\n (at format.hpp:77)", err.what());
  }
}

TEST(TestUtils, test_result_empty_brace) {
  try {
    auto text = format("{1}+{} = {0}", 2, "one");
    FAIL() << "[Error] Conversion error\n (at format.hpp:94)";
  } catch (const std::runtime_error &err) {
    EXPECT_STREQ("[Error] Conversion error\n (at format.hpp:94)", err.what());
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
