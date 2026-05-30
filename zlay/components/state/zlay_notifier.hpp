#ifndef ZLAY_COMPONENTS_STATE_ZLAY_NOTIFIER_HPP
#define ZLAY_COMPONENTS_STATE_ZLAY_NOTIFIER_HPP

#include <functional>
#include <vector>

namespace zlay {

class Notifier {
public:
  typedef std::function<void()> Listener;

  void notify() {
    for (std::size_t i = 0; i < listeners_.size(); ++i) {
      listeners_[i]();
    }
  }

  void subscribe(Listener listener) {
    listeners_.push_back(listener);
  }

private:
  std::vector<Listener> listeners_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_STATE_ZLAY_NOTIFIER_HPP
