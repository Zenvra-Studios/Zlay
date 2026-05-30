#ifndef ZLAY_COMPONENTS_RUNTIME_ZLAY_ELEMENT_HPP
#define ZLAY_COMPONENTS_RUNTIME_ZLAY_ELEMENT_HPP

#include <memory>
#include <vector>

#include <zlay/components/runtime/zlay_state.hpp>

namespace zlay {

class Element {
public:
  explicit Element(const Widget& widget, Element* parent = 0)
      : widget_(widget), parent_(parent), mounted_(false), dirty_(false) {}

  ~Element() { unmount(); }

  const Widget& widget() const { return widget_; }
  Element* parent() const { return parent_; }
  bool mounted() const { return mounted_; }
  bool dirty() const { return dirty_; }

  TypeId typeId() const { return widget_.typeId(); }
  const Key& key() const { return widget_.key(); }

  bool canUpdate(const Widget& next) const {
    return widget_.typeId() == next.typeId() && widget_.key() == next.key();
  }

  void mount(BuildContext& inherited_context) {
    if (mounted_) {
      return;
    }
    mounted_ = true;
    BuildContext ctx = inherited_context.child(this);
    mountByKind(ctx);
  }

  void update(const Widget& next, BuildContext& inherited_context) {
    widget_ = next;
    BuildContext ctx = inherited_context.child(this);

    if (widget_.kind() == WidgetKind::Render) {
      if (!render_object_) {
        render_object_ = widget_.createRenderObject(ctx);
      } else {
        widget_.updateRenderObject(*render_object_, ctx);
      }
      reconcileChildren(widget_.children(), ctx);
      dirty_ = false;
      return;
    }

    if (widget_.kind() == WidgetKind::StatelessComponent) {
      Widget built = widget_.build(ctx);
      reconcileSingleChild(built, ctx);
      dirty_ = false;
      return;
    }

    if (widget_.kind() == WidgetKind::StatefulComponent) {
      if (!state_) {
        state_ = widget_.createState();
        if (state_) {
          state_->attach(this, [this]() { markDirty(); });
          state_->initState();
          state_->didMount();
        }
      } else {
        state_->didUpdate();
      }
      Widget built = state_ ? state_->build(ctx) : Widget();
      if (state_) {
        state_->clearDirty();
      }
      reconcileSingleChild(built, ctx);
      dirty_ = false;
      return;
    }

    children_.clear();
    render_object_.reset();
    dirty_ = false;
  }

  void rebuild(BuildContext& inherited_context) {
    if (dirty_) {
      update(widget_, inherited_context);
    }
    for (std::size_t i = 0; i < children_.size(); ++i) {
      if (children_[i]) {
        children_[i]->rebuild(inherited_context);
      }
    }
  }

  void unmount() {
    if (!mounted_ && children_.empty() && !render_object_ && !state_) {
      return;
    }

    if (state_) {
      state_->beforeUnmount();
    }
    for (std::size_t i = 0; i < children_.size(); ++i) {
      if (children_[i]) {
        children_[i]->unmount();
      }
    }
    children_.clear();
    render_object_.reset();
    if (state_) {
      state_->dispose();
      state_->detach();
      state_.reset();
    }
    mounted_ = false;
    dirty_ = false;
  }

  void markDirty() {
    dirty_ = true;
    if (parent_) {
      parent_->markDirty();
    }
  }

  RenderObject* renderObject() const {
    if (render_object_) {
      return render_object_.get();
    }
    if (!children_.empty() && children_[0]) {
      return children_[0]->renderObject();
    }
    return 0;
  }

  void layout(const BoxConstraints& constraints) {
    RenderObject* object = renderObject();
    if (object) {
      object->layout(constraints);
    }
  }

  void paint(ZLay_Canvas* canvas) {
    RenderObject* object = renderObject();
    if (object) {
      object->paint(canvas);
    }
  }

  std::vector<std::unique_ptr<Element> >& children() { return children_; }
  const std::vector<std::unique_ptr<Element> >& children() const { return children_; }

private:
  void mountByKind(BuildContext& ctx) {
    if (widget_.kind() == WidgetKind::Render) {
      render_object_ = widget_.createRenderObject(ctx);
      reconcileChildren(widget_.children(), ctx);
      return;
    }
    if (widget_.kind() == WidgetKind::StatelessComponent) {
      reconcileSingleChild(widget_.build(ctx), ctx);
      return;
    }
    if (widget_.kind() == WidgetKind::StatefulComponent) {
      state_ = widget_.createState();
      if (state_) {
        state_->attach(this, [this]() { markDirty(); });
        state_->initState();
        state_->didMount();
        Widget built = state_->build(ctx);
        state_->clearDirty();
        reconcileSingleChild(built, ctx);
      }
    }
  }

  void reconcileSingleChild(const Widget& child, BuildContext& ctx) {
    std::vector<Widget> children;
    if (child) {
      children.push_back(child);
    }
    reconcileChildren(children, ctx);
  }

  void reconcileChildren(const std::vector<Widget>& next_widgets, BuildContext& ctx) {
    std::vector<std::unique_ptr<Element> > old;
    old.swap(children_);
    std::vector<bool> used(old.size(), false);

    for (std::size_t i = 0; i < next_widgets.size(); ++i) {
      const Widget& child_widget = next_widgets[i];
      int match = -1;

      if (child_widget.key()) {
        for (std::size_t j = 0; j < old.size(); ++j) {
          if (!used[j] && old[j] && old[j]->key() == child_widget.key() &&
              old[j]->typeId() == child_widget.typeId()) {
            match = static_cast<int>(j);
            break;
          }
        }
      } else if (i < old.size() && !used[i] && old[i] &&
                 old[i]->canUpdate(child_widget)) {
        match = static_cast<int>(i);
      } else {
        for (std::size_t j = 0; j < old.size(); ++j) {
          if (!used[j] && old[j] && !old[j]->key() &&
              old[j]->typeId() == child_widget.typeId()) {
            match = static_cast<int>(j);
            break;
          }
        }
      }

      if (match >= 0) {
        used[static_cast<std::size_t>(match)] = true;
        std::unique_ptr<Element> element =
          std::move(old[static_cast<std::size_t>(match)]);
        element->parent_ = this;
        element->update(child_widget, ctx);
        children_.push_back(std::move(element));
      } else {
        std::unique_ptr<Element> element(new Element(child_widget, this));
        element->mount(ctx);
        children_.push_back(std::move(element));
      }
    }

    for (std::size_t i = 0; i < old.size(); ++i) {
      if (!used[i] && old[i]) {
        old[i]->unmount();
      }
    }

    syncRenderChildren();
  }

  void syncRenderChildren() {
    if (!render_object_) {
      return;
    }
    std::vector<RenderObject*> render_children;
    for (std::size_t i = 0; i < children_.size(); ++i) {
      if (children_[i]) {
        render_children.push_back(children_[i]->renderObject());
      }
    }
    render_object_->setChildren(render_children);
  }

  Widget widget_;
  Element* parent_;
  bool mounted_;
  bool dirty_;
  std::unique_ptr<RenderObject> render_object_;
  std::unique_ptr<StateBase> state_;
  std::vector<std::unique_ptr<Element> > children_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_RUNTIME_ZLAY_ELEMENT_HPP
