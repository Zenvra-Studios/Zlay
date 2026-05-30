#ifndef ZLAY_COMPONENTS_GRAPH_ZLAY_NODE_HPP
#define ZLAY_COMPONENTS_GRAPH_ZLAY_NODE_HPP

#include <zlay/components/text/zlay_text.hpp>

namespace zlay {

inline Widget Node(const std::string& name) {
  return Text(name);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_GRAPH_ZLAY_NODE_HPP
