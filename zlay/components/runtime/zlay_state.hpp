#ifndef ZLAY_COMPONENTS_RUNTIME_ZLAY_STATE_HPP
#define ZLAY_COMPONENTS_RUNTIME_ZLAY_STATE_HPP

#include <functional>

#include <zlay/components/runtime/zlay_build_context.hpp>
#include <zlay/components/runtime/zlay_widget.hpp>

namespace zlay {

class Element;

class StateBase {
public:
  StateBase() : element_(0), dirty_(false) {}
  virtual ~StateBase() {}

  virtual void initState() {}
  virtual void didMount() {}
  virtual void didUpdate() {}
  virtual void beforeUnmount() {}
  virtual void dispose() {}

  virtual Widget build(BuildContext& ctx) = 0;

  Element* element() const { return element_; }
  bool dirty() const { return dirty_; }
  void clearDirty() { dirty_ = false; }

  void attach(Element* element, std::function<void()> mark_dirty) {
    element_ = element;
    mark_dirty_ = mark_dirty;
  }

  void detach() {
    element_ = 0;
    mark_dirty_ = std::function<void()>();
  }

protected:
  template <class Fn>
  void setState(Fn&& fn) {
    fn();
    dirty_ = true;
    if (mark_dirty_) {
      mark_dirty_();
    }
  }

private:
  Element* element_;
  bool dirty_;
  std::function<void()> mark_dirty_;
};

template <class ComponentT>
class ComponentState : public StateBase {
public:
  ComponentState() : component_(0) {}

  const ComponentT* component() const { return component_; }
  void bindComponent(const ComponentT* component) { component_ = component; }

private:
  const ComponentT* component_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_RUNTIME_ZLAY_STATE_HPP
