#ifndef ZLAY_COMPONENTS_STATE_ZLAY_SIGNAL_HPP
#define ZLAY_COMPONENTS_STATE_ZLAY_SIGNAL_HPP

#include <functional>
#include <vector>

namespace zlay {

template <class T>
class Signal {
public:
  typedef std::function<void(const T&)> Listener;

  Signal() : value_() {}
  Signal(const T& value) : value_(value) {}

  const T& get() const { return value_; }
  operator const T&() const { return value_; }

  void set(const T& value) {
    value_ = value;
    for (std::size_t i = 0; i < listeners_.size(); ++i) {
      listeners_[i](value_);
    }
  }

  Signal& operator=(const T& value) {
    set(value);
    return *this;
  }

  void subscribe(Listener listener) {
    listeners_.push_back(listener);
  }

private:
  T value_;
  std::vector<Listener> listeners_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_STATE_ZLAY_SIGNAL_HPP
