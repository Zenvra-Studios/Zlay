#ifndef ZLAY_COMPONENTS_RUNTIME_ZLAY_COMPONENT_HPP
#define ZLAY_COMPONENTS_RUNTIME_ZLAY_COMPONENT_HPP

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#include <zlay/components/runtime/zlay_state.hpp>

namespace zlay {

class Component {
public:
  virtual ~Component() {}
};

class StatelessComponent : public Component {
public:
  virtual Widget build(BuildContext& ctx) const = 0;
};

class StatefulComponent : public Component {
public:
  virtual std::unique_ptr<StateBase> createState() = 0;
};

namespace detail {

template <class StoredComponent>
inline Widget componentWidgetImpl(const std::shared_ptr<StoredComponent>& instance,
                                  const Key& key,
                                  std::true_type) {
  typename Widget::StateFactory factory = [instance]() -> std::unique_ptr<StateBase> {
    StatefulComponent* stateful = instance.get();
    std::unique_ptr<StateBase> state = stateful->createState();
    ComponentState<StoredComponent>* typed =
      dynamic_cast<ComponentState<StoredComponent>*>(state.get());
    if (typed) {
      typed->bindComponent(instance.get());
    }
    return state;
  };

  return Widget::stateful(type_id<StoredComponent>(), "Component", key, factory);
}

template <class StoredComponent>
inline Widget componentWidgetImpl(const std::shared_ptr<StoredComponent>& instance,
                                  const Key& key,
                                  std::false_type) {
  static_assert(std::is_base_of<StatelessComponent, StoredComponent>::value,
                "zlay::ComponentWidget expects StatelessComponent or StatefulComponent");

  typename Widget::StatelessBuilder builder = [instance](BuildContext& ctx) -> Widget {
    const StatelessComponent* stateless = instance.get();
    return stateless->build(ctx);
  };

  return Widget::stateless(type_id<StoredComponent>(), "Component", key, builder);
}

} // namespace detail

template <class ComponentT>
inline Widget ComponentWidget(ComponentT component, const Key& key = Key()) {
  typedef typename std::decay<ComponentT>::type StoredComponent;
  static_assert(std::is_base_of<Component, StoredComponent>::value,
                "zlay::ComponentWidget expects a zlay::Component-derived type");

  std::shared_ptr<StoredComponent> instance(
    new StoredComponent(std::move(component)));

  return detail::componentWidgetImpl<StoredComponent>(
    instance, key, typename std::is_base_of<StatefulComponent, StoredComponent>::type());
}

template <class ComponentT, class... Args>
inline Widget MakeComponent(Args&&... args) {
  return ComponentWidget(ComponentT(std::forward<Args>(args)...));
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_RUNTIME_ZLAY_COMPONENT_HPP
