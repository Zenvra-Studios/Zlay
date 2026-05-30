#ifndef ZLAY_COMPONENTS_ASSETS_ZLAY_ASSET_BROWSER_HPP
#define ZLAY_COMPONENTS_ASSETS_ZLAY_ASSET_BROWSER_HPP

#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget AssetBrowser() {
  std::vector<Widget> body;
  body.push_back(Text("Textures"));
  body.push_back(Text("Materials"));
  body.push_back(Text("Meshes"));
  return ToolingPanel("Assets", body);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_ASSETS_ZLAY_ASSET_BROWSER_HPP
