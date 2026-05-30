#ifndef ZLAY_COMPONENTS_RUNTIME_ZLAY_WIDGET_HPP
#define ZLAY_COMPONENTS_RUNTIME_ZLAY_WIDGET_HPP

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <zlay/components/meta/zlay_type_id.hpp>
#include <zlay/components/runtime/zlay_build_context.hpp>
#include <zlay/components/runtime/zlay_key.hpp>
#include <zlay/components/runtime/zlay_render_object.hpp>

namespace zlay {

class StateBase;

namespace detail {

template <class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace detail

enum class WidgetKind {
  Empty,
  Render,
  StatelessComponent,
  StatefulComponent
};

class Widget {
public:
  typedef std::function<std::unique_ptr<RenderObject>(BuildContext&)> RenderFactory;
  typedef std::function<void(RenderObject&, BuildContext&)> RenderUpdater;
  typedef std::function<Widget(BuildContext&)> StatelessBuilder;
  typedef std::function<std::unique_ptr<StateBase>()> StateFactory;

  Widget() {}

  bool empty() const { return !impl_; }
  explicit operator bool() const { return !empty(); }

  WidgetKind kind() const {
    return impl_ ? impl_->kind : WidgetKind::Empty;
  }

  TypeId typeId() const {
    return impl_ ? impl_->type_id : 0u;
  }

  const Key& key() const {
    static const Key empty_key;
    return impl_ ? impl_->key : empty_key;
  }

  const std::string& debugName() const {
    static const std::string empty_name;
    return impl_ ? impl_->debug_name : empty_name;
  }

  const std::vector<Widget>& children() const {
    static const std::vector<Widget> empty_children;
    return impl_ ? impl_->children : empty_children;
  }

  std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) const {
    if (impl_ && impl_->render_factory) {
      return impl_->render_factory(ctx);
    }
    return std::unique_ptr<RenderObject>();
  }

  void updateRenderObject(RenderObject& object, BuildContext& ctx) const {
    if (impl_ && impl_->render_updater) {
      impl_->render_updater(object, ctx);
    }
  }

  Widget build(BuildContext& ctx) const {
    if (impl_ && impl_->stateless_builder) {
      return impl_->stateless_builder(ctx);
    }
    return Widget();
  }

  std::unique_ptr<StateBase> createState() const {
    if (impl_ && impl_->state_factory) {
      return impl_->state_factory();
    }
    return std::unique_ptr<StateBase>();
  }

  static Widget render(TypeId type_id, const std::string& debug_name,
                       const Key& key,
                       const std::vector<Widget>& children,
                       RenderFactory factory,
                       RenderUpdater updater = RenderUpdater()) {
    std::shared_ptr<Impl> impl(new Impl());
    impl->kind = WidgetKind::Render;
    impl->type_id = type_id;
    impl->key = key;
    impl->debug_name = debug_name;
    impl->children = children;
    impl->render_factory = factory;
    impl->render_updater = updater;
    return Widget(impl);
  }

  static Widget stateless(TypeId type_id, const std::string& debug_name,
                          const Key& key, StatelessBuilder builder) {
    std::shared_ptr<Impl> impl(new Impl());
    impl->kind = WidgetKind::StatelessComponent;
    impl->type_id = type_id;
    impl->key = key;
    impl->debug_name = debug_name;
    impl->stateless_builder = builder;
    return Widget(impl);
  }

  static Widget stateful(TypeId type_id, const std::string& debug_name,
                         const Key& key, StateFactory state_factory) {
    std::shared_ptr<Impl> impl(new Impl());
    impl->kind = WidgetKind::StatefulComponent;
    impl->type_id = type_id;
    impl->key = key;
    impl->debug_name = debug_name;
    impl->state_factory = state_factory;
    return Widget(impl);
  }

private:
  struct Impl {
    Impl() : kind(WidgetKind::Empty), type_id(0u) {}

    WidgetKind kind;
    TypeId type_id;
    Key key;
    std::string debug_name;
    std::vector<Widget> children;
    RenderFactory render_factory;
    RenderUpdater render_updater;
    StatelessBuilder stateless_builder;
    StateFactory state_factory;
  };

  explicit Widget(std::shared_ptr<const Impl> impl) : impl_(impl) {}

  std::shared_ptr<const Impl> impl_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_RUNTIME_ZLAY_WIDGET_HPP
