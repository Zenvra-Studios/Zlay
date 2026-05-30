#include <cstdio>
#include <memory>

#include <zlay/components.hpp>

class EngineWorkbench final : public zlay::StatelessComponent {
public:
  zlay::Widget build(zlay::BuildContext&) const override {
    return zlay::DockSpace({
      zlay::DockTab("Viewport", zlay::Viewport()),
      zlay::DockTab("Scene", zlay::SceneOutliner()),
      zlay::DockTab("Inspector", zlay::Inspector()),
      zlay::DockTab("Assets", zlay::AssetBrowser()),
      zlay::DockTab("Console", zlay::ConsolePanel())
    });
  }
};

class Counter final : public zlay::StatefulComponent {
public:
  class State final : public zlay::ComponentState<Counter> {
  public:
    State() : value_(0) {}

    zlay::Widget build(zlay::BuildContext&) override {
      zlay::ButtonProps increment;
      increment.text = "Increment";
      increment.variant = zlay::ButtonVariant::Primary;
      increment.onClick = [this]() {
        setState([this]() { ++value_; });
      };

      return zlay::Column({
        zlay::Text(std::string("Counter: ") + std::to_string(value_)),
        zlay::Button(increment)
      });
    }

  private:
    int value_;
  };

  std::unique_ptr<zlay::StateBase> createState() override {
    return std::unique_ptr<zlay::StateBase>(new State());
  }
};

int main() {
  zlay::Widget root = zlay::Column({
    zlay::Text("ZLay C++ Components"),
    zlay::ComponentWidget(EngineWorkbench()),
    zlay::ComponentWidget(Counter())
  });

  zlay::BuildContext context;
  std::unique_ptr<zlay::Element> tree =
    zlay::Reconciler::reconcile(std::unique_ptr<zlay::Element>(), root, context);
  tree->layout(zlay::BoxConstraints::loose(zlay::Size(1280.0f, 720.0f)));

  zlay::RenderObject* render = tree->renderObject();
  zlay::Size size = render ? render->size() : zlay::Size();
  std::printf("components example: root %.1fx%.1f\n", size.width, size.height);
  return render ? 0 : 1;
}
