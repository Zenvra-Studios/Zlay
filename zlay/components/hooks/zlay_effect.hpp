#ifndef ZLAY_COMPONENTS_HOOKS_ZLAY_EFFECT_HPP
#define ZLAY_COMPONENTS_HOOKS_ZLAY_EFFECT_HPP

#include <functional>

namespace zlay {

class Effect {
public:
  Effect() {}
  explicit Effect(std::function<void()> run) : run_(run) {}

  void run() const {
    if (run_) {
      run_();
    }
  }

private:
  std::function<void()> run_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_HOOKS_ZLAY_EFFECT_HPP
