#ifndef ZLAY_COMPONENTS_ASSETS_ZLAY_ASSET_PREVIEW_HPP
#define ZLAY_COMPONENTS_ASSETS_ZLAY_ASSET_PREVIEW_HPP

#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget AssetPreview() {
  std::vector<Widget> body;
  body.push_back(Text("No asset selected"));
  return ToolingPanel("Preview", body);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_ASSETS_ZLAY_ASSET_PREVIEW_HPP
