#ifndef ZLAY_COMPONENTS_GRAPH_ZLAY_NODE_GRAPH_HPP
#define ZLAY_COMPONENTS_GRAPH_ZLAY_NODE_GRAPH_HPP

#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget NodeGraph() {
  std::vector<Widget> body;
  body.push_back(Text("Node A -> Node B"));
  return ToolingPanel("Graph", body);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_GRAPH_ZLAY_NODE_GRAPH_HPP
