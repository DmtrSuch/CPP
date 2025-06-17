#include <sstream>
#include <string>

enum class Error { NoError, CorruptedArchive };

struct Data {
  uint64_t a;
  bool b;
  uint64_t c;

  template <class Serializer> Error serialize(Serializer &serializer) {
    return serializer(a, b, c);
  }

  template <class Deserializer> Error deserialize(Deserializer &deserializer) {
    return deserializer(a, b, c);
  }
};

class Serializer {
  std::ostream *out_;
  static constexpr char Separator = ' ';

public:
  explicit Serializer(std::ostream *out) : out_(out) {}

  template <class T> Error save(T &object) { return object.serialize(*this); }

  template <class... ArgsT> Error operator()(ArgsT &...args) {
    return process(args...);
  }

private:
  Error process(bool &arg) {
    *out_ << (arg ? "true" : "false") << Separator;
    return Error::NoError;
  }

  Error process(uint64_t &arg) {
    *out_ << arg << Separator;
    return Error::NoError;
  }

  template <class T, class... ArgsT> Error process(T &val, ArgsT &&...args) {
    if (Error err = process(val); err != Error::NoError)
      return err;
    return process(std::forward<ArgsT>(args)...);
  }

  Error process() { return Error::NoError; }
};

class Deserializer {
  std::istream *in_;
  static constexpr char Separator = ' ';

public:
  explicit Deserializer(std::istream *in) : in_(in) {}

  template <class T> Error load(T &object) { return object.deserialize(*this); }

  template <class... ArgsT> Error operator()(ArgsT &...args) {
    return process(args...);
  }

private:
  Error process(bool &arg) {
    std::string buf;
    *in_ >> buf;
    if (buf == "true")
      arg = true;
    else if (buf == "false")
      arg = false;
    else
      return Error::CorruptedArchive;
    return Error::NoError;
  }

  Error process(uint64_t &arg) {
    std::string buf;
    *in_ >> buf;
    try {
      size_t pos;
      arg = std::stoull(buf, &pos);
      if (pos != buf.size())
        return Error::CorruptedArchive;
    } catch (...) {
      return Error::CorruptedArchive;
    }
    return Error::NoError;
  }

  template <class T, class... ArgsT> Error process(T &val, ArgsT &&...args) {
    if (Error err = process(val); err != Error::NoError)
      return err;
    return process(std::forward<ArgsT>(args)...);
  }

  Error process() { return Error::NoError; }
};
