#ifndef ZLAY_COMPONENTS_RUNTIME_ZLAY_RECONCILER_HPP
#define ZLAY_COMPONENTS_RUNTIME_ZLAY_RECONCILER_HPP

#include <memory>

#include <zlay/components/runtime/zlay_element.hpp>

namespace zlay {

class Reconciler {
public:
  static std::unique_ptr<Element> reconcile(std::unique_ptr<Element> old_element,
                                            const Widget& widget,
                                            BuildContext& ctx) {
    if (!old_element) {
      std::unique_ptr<Element> mounted(new Element(widget));
      mounted->mount(ctx);
      return mounted;
    }

    if (old_element->canUpdate(widget)) {
      old_element->update(widget, ctx);
      return old_element;
    }

    old_element->unmount();
    std::unique_ptr<Element> mounted(new Element(widget));
    mounted->mount(ctx);
    return mounted;
  }
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_RUNTIME_ZLAY_RECONCILER_HPP
