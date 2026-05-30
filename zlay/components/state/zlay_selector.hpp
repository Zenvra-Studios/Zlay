#ifndef ZLAY_COMPONENTS_STATE_ZLAY_SELECTOR_HPP
#define ZLAY_COMPONENTS_STATE_ZLAY_SELECTOR_HPP

#include <functional>

#include <zlay/components/runtime/zlay_widget.hpp>

namespace zlay {

template <class Source, class Selected>
struct SelectorProps {
  Source value;
  std::function<Selected(const Source&)> select;
  std::function<Widget(const Selected&)> builder;
  Key key;
};

template <class Source, class Selected>
inline Widget Selector(const SelectorProps<Source, Selected>& props) {
  return Widget::stateless(type_id<SelectorProps<Source, Selected> >(), "Selector",
                           props.key, [props](BuildContext&) {
                             return props.builder(props.select(props.value));
                           });
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_STATE_ZLAY_SELECTOR_HPP
