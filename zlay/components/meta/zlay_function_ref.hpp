#ifndef ZLAY_COMPONENTS_META_ZLAY_FUNCTION_REF_HPP
#define ZLAY_COMPONENTS_META_ZLAY_FUNCTION_REF_HPP

#include <utility>

namespace zlay {

template <class>
class FunctionRef;

template <class R, class... Args>
class FunctionRef<R(Args...)> {
public:
  FunctionRef() : object_(0), callback_(0) {}

  template <class Fn>
  FunctionRef(Fn& fn)
      : object_(static_cast<void*>(&fn)),
        callback_(&FunctionRef::call<Fn>) {}

  R operator()(Args... args) const {
    return callback_(object_, std::forward<Args>(args)...);
  }

  explicit operator bool() const { return callback_ != 0; }

private:
  template <class Fn>
  static R call(void* object, Args... args) {
    return (*static_cast<Fn*>(object))(std::forward<Args>(args)...);
  }

  void* object_;
  R (*callback_)(void*, Args...);
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_META_ZLAY_FUNCTION_REF_HPP
