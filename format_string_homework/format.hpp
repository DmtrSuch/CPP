#include <charconv>
#include <concepts>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>
concept Streamable = requires(std::ostream &os, const T &a) {
  { os << a } -> std::same_as<std::ostream &>;
};

struct Error {
  std::string message_;
  const char *fileName_;
  int line_;
  Error(const std::string &message, const char *fileName, int line)
      : message_(message), fileName_(fileName), line_(line) {}
  operator std::string() const {
    std::ostringstream oss;
    oss << "[Error] " << message_ << " (at " << fileName_ << ":" << line_
        << ")";
    return oss.str();
  }
};

template <typename T> std::string to_str(const T &value) {
  throw std::runtime_error(
      Error("Unsupported type for <<", __FILE__, __LINE__));
}

template <Streamable T> std::string to_str(const T &value) {
  std::ostringstream oss;
  try {
    oss << value;
  } catch (const std::exception &e) {
    std::ostringstream err;
    err << "to_str failed by type '" << typeid(T).name() << "': " << e.what();
    throw std::runtime_error(Error(err.str(), __FILE__, __LINE__));
  }

  if (!oss) {
    std::ostringstream err;
    err << "to_str failed to write value of type '" << typeid(T).name()
        << "' to stream";
    throw std::runtime_error(Error(err.str(), __FILE__, __LINE__));
  }

  return oss.str();
}

template <typename... Args> std::string format(Args &&...args) {
  std::vector<std::string> params;
  (params.push_back(to_str(std::forward<Args>(args))), ...);
  size_t length_params = params.size();
  if (length_params == 0) {
    throw std::runtime_error(Error("Zero params\n", __FILE__, __LINE__));
  }
  std::string result = "";
  size_t length_string = params[0].length();
  for (size_t i = 0; i < length_string; i++) {
    if (params[0][i] != '{' && params[0][i] != '}') {
      result += params[0][i];
      continue;
    }
    if (params[0][i] == '}') {
      throw std::runtime_error(
          Error("Incorrect '}': without init '{'\n", __FILE__, __LINE__));
    }
    i++;
    std::string str_number = "";
    while (i < length_string && params[0][i] != '}' && params[0][i] != '{') {
      if (!isdigit(params[0][i])) {
        throw std::runtime_error(
            Error("Conversion error\n", __FILE__, __LINE__));
      }
      str_number += params[0][i];
      i++;
    }
    if (params[0][i] == '{') {
      throw std::runtime_error(
          Error("Incorrect '{': stil open another '{'\n", __FILE__, __LINE__));
    }
    if (params[0][i] != '}') {
      throw std::runtime_error(Error(
          "Incorrect '}': does not exist final '}'\n", __FILE__, __LINE__));
    }
    size_t number;
    auto err = std::from_chars(str_number.data(),
                               str_number.data() + str_number.size(), number);
    if (err.ec != std::errc()) {
      throw std::runtime_error(Error("Conversion error\n", __FILE__, __LINE__));
    }
    number++;
    if (number < 1 || number >= length_params) {
      throw std::runtime_error(
          Error("Index does not exist\n", __FILE__, __LINE__));
    }
    result += params[number];
  }

  return result;
}
