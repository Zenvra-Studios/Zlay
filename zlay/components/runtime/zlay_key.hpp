#ifndef ZLAY_COMPONENTS_RUNTIME_ZLAY_KEY_HPP
#define ZLAY_COMPONENTS_RUNTIME_ZLAY_KEY_HPP

#include <sstream>
#include <string>

namespace zlay {

class Key {
public:
  Key() : has_value_(false) {}
  Key(const char* value) : value_(value ? value : ""), has_value_(value != 0) {}
  Key(const std::string& value) : value_(value), has_value_(true) {}

  static Key none() { return Key(); }

  static Key index(unsigned long value) {
    std::ostringstream out;
    out << "#" << value;
    return Key(out.str());
  }

  bool empty() const { return !has_value_; }
  explicit operator bool() const { return has_value_; }
  const std::string& value() const { return value_; }

  friend bool operator==(const Key& lhs, const Key& rhs) {
    if (!lhs.has_value_ && !rhs.has_value_) {
      return true;
    }
    return lhs.has_value_ == rhs.has_value_ && lhs.value_ == rhs.value_;
  }

  friend bool operator!=(const Key& lhs, const Key& rhs) {
    return !(lhs == rhs);
  }

private:
  std::string value_;
  bool has_value_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_RUNTIME_ZLAY_KEY_HPP
