#ifndef ZLAY_COMPONENTS_RUNTIME_ZLAY_COMPONENT_REGISTRY_HPP
#define ZLAY_COMPONENTS_RUNTIME_ZLAY_COMPONENT_REGISTRY_HPP

#include <functional>
#include <map>
#include <string>

#include <zlay/components/runtime/zlay_widget.hpp>

namespace zlay {

class ComponentRegistry {
public:
  typedef std::function<Widget()> Factory;

  void registerFactory(const std::string& name, Factory factory) {
    factories_[name] = factory;
  }

  bool contains(const std::string& name) const {
    return factories_.find(name) != factories_.end();
  }

  Widget create(const std::string& name) const {
    std::map<std::string, Factory>::const_iterator it = factories_.find(name);
    return it == factories_.end() ? Widget() : it->second();
  }

private:
  std::map<std::string, Factory> factories_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_RUNTIME_ZLAY_COMPONENT_REGISTRY_HPP
