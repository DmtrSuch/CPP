#include "serializer.hpp"
#include <gtest/gtest.h>

TEST(TestUtils, test_serializer_correct) {
  Data x{1, true, 2};

  std::stringstream stream;

  Serializer serializer(&stream);
  Error result = serializer.save(x);

  Data y{0, false, 0};

  Deserializer deserializer(&stream);
  const Error err = deserializer.load(y);

  ASSERT_EQ(x.a, y.a);
  ASSERT_EQ(x.b, y.b);
  ASSERT_EQ(x.c, y.c);
  ASSERT_EQ(result, Error::NoError);
  ASSERT_EQ(err, Error::NoError);
}

TEST(TestUtils, test_deserializer_correct) {

  std::stringstream stream;
  stream << "1" << ' ' << "true" << ' ' << "2";
  Data y{0, false, 0};

  Deserializer deserializer(&stream);
  const Error err = deserializer.load(y);

  ASSERT_EQ(1, y.a);
  ASSERT_EQ(true, y.b);
  ASSERT_EQ(2, y.c);
  ASSERT_EQ(err, Error::NoError);
}

TEST(TestUtils, test_deserializer_incorrect_uint) {

  std::stringstream stream;
  stream << "true" << ' ' << "1" << ' ' << "2";
  Data y{0, false, 0};

  Deserializer deserializer(&stream);
  const Error err = deserializer.load(y);

  ASSERT_EQ(err, Error::CorruptedArchive);
}

TEST(TestUtils, test_deserializer_incorrect_bool) {

  std::stringstream stream;
  stream << "1" << ' ' << "1" << ' ' << "2";
  Data y{0, false, 0};

  Deserializer deserializer(&stream);
  const Error err = deserializer.load(y);

  ASSERT_EQ(err, Error::CorruptedArchive);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
