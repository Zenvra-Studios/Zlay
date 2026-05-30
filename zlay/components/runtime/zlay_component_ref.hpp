#ifndef ZLAY_COMPONENTS_RUNTIME_ZLAY_COMPONENT_REF_HPP
#define ZLAY_COMPONENTS_RUNTIME_ZLAY_COMPONENT_REF_HPP

#include <memory>

namespace zlay {

template <class T>
class ComponentRef {
public:
  ComponentRef() {}
  explicit ComponentRef(const std::shared_ptr<T>& value) : value_(value) {}

  std::shared_ptr<T> lock() const { return value_.lock(); }
  bool expired() const { return value_.expired(); }

private:
  std::weak_ptr<T> value_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_RUNTIME_ZLAY_COMPONENT_REF_HPP
