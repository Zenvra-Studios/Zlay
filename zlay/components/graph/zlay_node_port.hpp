#ifndef ZLAY_COMPONENTS_GRAPH_ZLAY_NODE_PORT_HPP
#define ZLAY_COMPONENTS_GRAPH_ZLAY_NODE_PORT_HPP

#include <zlay/components/text/zlay_text.hpp>

namespace zlay {

inline Widget NodePort(const std::string& name) {
  return Text(name);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_GRAPH_ZLAY_NODE_PORT_HPP
