#ifndef ZLAY_COMPONENTS_RUNTIME_ZLAY_BUILD_CONTEXT_HPP
#define ZLAY_COMPONENTS_RUNTIME_ZLAY_BUILD_CONTEXT_HPP

#include <zlay.h>

namespace zlay {

class Element;

class BuildContext {
public:
  BuildContext()
      : element_(0), theme_(0), c_context_(0), user_data_(0) {}

  BuildContext(Element* element, const ZLay_Theme* theme = 0,
               ZLay_Context* c_context = 0, void* user_data = 0)
      : element_(element),
        theme_(theme),
        c_context_(c_context),
        user_data_(user_data) {}

  Element* element() const { return element_; }
  const ZLay_Theme* theme() const { return theme_; }
  ZLay_Context* c_context() const { return c_context_; }
  void* user_data() const { return user_data_; }

  BuildContext child(Element* element) const {
    return BuildContext(element, theme_, c_context_, user_data_);
  }

private:
  Element* element_;
  const ZLay_Theme* theme_;
  ZLay_Context* c_context_;
  void* user_data_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_RUNTIME_ZLAY_BUILD_CONTEXT_HPP
