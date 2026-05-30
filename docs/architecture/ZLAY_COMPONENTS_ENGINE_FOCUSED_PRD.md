# Zlay Components Architecture PRD

## Document Info

| Field | Value |
|---|---|
| Document | Zlay Components Architecture PRD |
| Scope | Component system, Flutter-like class model, engine-focused tooling components, folder naming revision |
| Target language | C++11 baseline, C++14/C++17/C++20 enhanced path |
| Target project | Zlay |
| Focus | Engine UI components only |
| Removed scope | IDE-specific UI, DAW-specific UI, broad app-suite scope |
| Main correction | `zlay/components/core` should become `zlay/components/runtime` to avoid ambiguity with `zlay/core` |
| Main analogy | WebKit-quality pipeline without WebKit dependency |

---

## 1. Product Direction

Zlay Components is the high-level class-based component layer for Zlay.

The goal is to make Zlay usable like a native C++ component framework:

```cpp
class MyPanel final : public zlay::StatelessComponent {
public:
    zlay::Widget build(zlay::BuildContext& ctx) const override {
        return zlay::Column({
            zlay::Text("Hello Zlay"),
            zlay::Button("Run")
        });
    }
};
```

Zlay should feel close to Flutter-style composition, but stay native:

```txt
Component
  ↓
Widget
  ↓
Element
  ↓
RenderObject
  ↓
Layout
  ↓
Canvas
  ↓
RenderCommandList
  ↓
RendererBackend
  ↓
GpuDriver
```

Zlay is not a WebKit wrapper.

Zlay should aim for:

```txt
WebKit-like rendering discipline
Flutter-like component ergonomics
Engine-style backend/driver architecture
Native C++ implementation
```

---

## 2. Core Strategy

Zlay should not try to become a browser.

Zlay should take the good pipeline ideas from browser engines:

```txt
Component Tree
  ↓
Style Resolution
  ↓
Layout
  ↓
Paint
  ↓
Display List / Render Commands
  ↓
Compositor / Renderer Backend
  ↓
GPU Driver
```

Equivalent comparison:

| WebKit / Browser | Zlay |
|---|---|
| HTML element | `ZLayComponent` |
| DOM tree | `ZLayElementTree` |
| CSS | `ZLayStyle`, `ZLayTheme`, `ZLayStyleSheet` |
| CSS cascade | `ZLayStyleResolver` |
| Layout tree | `ZLayRenderObjectTree` |
| Paint phase | `ZLayCanvas` |
| Display list | `ZLayRenderCommandList` |
| Compositor | `ZLayRendererBackend` |
| GPU backend | `ZLayDriver` |

Final positioning:

```txt
WebKit quality, not WebKit dependency.
Browser-like pipeline, not browser engine.
Zlay components, Zlay style, Zlay canvas, Zlay renderer, Zlay driver.
```

---

## 3. Scope Control

The component layer should focus on engine UI.

Do not create top-level domains for:

```txt
ide/
daw/
editor/
```

Those names expand the scope too much.

Instead use engine-native naming:

```txt
tooling/
viewport/
scene/
inspector/
assets/
graph/
timeline/
debug/
```

Meaning:

| Old naming | New naming | Reason |
|---|---|---|
| `editor/` | `tooling/` | More neutral; means engine tooling, not a full editor product. |
| `ide/` | Remove or fold into `tooling/code/` later | IDE is outside current scope. |
| `daw/` | Remove | DAW is outside current scope. |
| `engine/` | Split into `viewport/`, `scene/`, `inspector/`, `assets/` | More precise. |
| `node/` | `graph/` | General graph system for shader/material/behavior graphs. |
| `timeline/` | Keep | Means animation/keyframe/sequence timeline, not DAW. |

---

## 4. Folder Naming Revision

### 4.1 Current Ambiguity

The current tree has:

```txt
zlay/
├── core/
├── components/
├── drivers/
├── event/
├── lifecycle/
├── os/
├── parser/
├── renderer/
├── secure/
└── ui/
```

If components also contains:

```txt
zlay/components/core/
```

then the meaning becomes ambiguous:

```txt
zlay/core
zlay/components/core
```

Both say "core", but they mean different things.

---

## 5. Meaning of `zlay/core`

`zlay/core` should mean global Zlay foundation.

This folder is for low-level reusable foundation code.

Recommended contents:

```txt
zlay/core/
├── zlay_arena.c
├── zlay_arena.h
├── zlay_memory.c
├── zlay_memory.h
├── zlay_cache.c
├── zlay_cache.h
├── zlay_compute.h
├── zlay_hash.h
├── zlay_logger.h
└── zlay_base_types.h
```

Meaning:

```txt
memory
arena
allocator
cache
hash
logger
base types
low-level utilities
```

Definition:

```txt
zlay/core = global foundation for all Zlay subsystems
```

It should not mean component runtime.

---

## 6. Replace `components/core` with `components/runtime`

Instead of:

```txt
zlay/components/core/
```

Use:

```txt
zlay/components/runtime/
```

Because this folder is not global core. It is the runtime object model for components.

Recommended contents:

```txt
zlay/components/runtime/
├── zlay_component.hpp
├── zlay_stateless_component.hpp
├── zlay_stateful_component.hpp
├── zlay_widget.hpp
├── zlay_element.hpp
├── zlay_state.hpp
├── zlay_build_context.hpp
├── zlay_key.hpp
├── zlay_reconciler.hpp
├── zlay_render_object.hpp
├── zlay_component_registry.hpp
└── zlay_component_ref.hpp
```

Definition:

```txt
zlay/components/runtime = runtime system for Flutter-like components
```

It contains:

```txt
Component
Widget
Element
State
BuildContext
Key
Reconciler
RenderObject bridge
Component registry
```

---

## 7. Replace `components/lifecycle` with `components/hooks`

The project already has:

```txt
zlay/lifecycle/
```

That global lifecycle folder is for:

```txt
autosave
history
journal
recovery
snapshot
session
```

So avoid:

```txt
zlay/components/lifecycle/
```

Use:

```txt
zlay/components/hooks/
```

Recommended contents:

```txt
zlay/components/hooks/
├── zlay_mount.hpp
├── zlay_unmount.hpp
├── zlay_update.hpp
├── zlay_effect.hpp
├── zlay_dirty.hpp
└── zlay_rebuild.hpp
```

Meaning:

```txt
component mount/unmount/update/effect lifecycle
```

---

## 8. Final Top-Level Meaning

```txt
zlay/
├── core/                 # global foundation
├── lifecycle/            # app/session lifecycle
├── renderer/             # render abstraction
├── drivers/              # low-level GPU/API drivers
├── backends/             # adapters
├── os/                   # OS abstraction
├── parser/               # parser/DSL
├── event/                # global event system
├── secure/               # hash/security utilities
├── ui/                   # legacy/simple UI widgets; can migrate later
└── components/           # Flutter-like class component layer
```

---

## 9. Final Components Folder Structure

```txt
zlay/
└── components/
    ├── runtime/
    │   ├── zlay_component.hpp
    │   ├── zlay_stateless_component.hpp
    │   ├── zlay_stateful_component.hpp
    │   ├── zlay_widget.hpp
    │   ├── zlay_element.hpp
    │   ├── zlay_state.hpp
    │   ├── zlay_build_context.hpp
    │   ├── zlay_key.hpp
    │   ├── zlay_reconciler.hpp
    │   ├── zlay_render_object.hpp
    │   └── zlay_component_registry.hpp
    │
    ├── meta/
    │   ├── zlay_type_id.hpp
    │   ├── zlay_type_list.hpp
    │   ├── zlay_type_traits.hpp
    │   ├── zlay_detection.hpp
    │   ├── zlay_concepts.hpp
    │   ├── zlay_policy.hpp
    │   ├── zlay_crtp.hpp
    │   ├── zlay_variant.hpp
    │   ├── zlay_any_widget.hpp
    │   └── zlay_function_ref.hpp
    │
    ├── hooks/
    │   ├── zlay_mount.hpp
    │   ├── zlay_unmount.hpp
    │   ├── zlay_update.hpp
    │   ├── zlay_effect.hpp
    │   ├── zlay_dirty.hpp
    │   └── zlay_rebuild.hpp
    │
    ├── state/
    │   ├── zlay_signal.hpp
    │   ├── zlay_notifier.hpp
    │   ├── zlay_value_notifier.hpp
    │   ├── zlay_state_store.hpp
    │   ├── zlay_provider.hpp
    │   ├── zlay_context_provider.hpp
    │   └── zlay_selector.hpp
    │
    ├── style/
    │   ├── zlay_style.hpp
    │   ├── zlay_style_sheet.hpp
    │   ├── zlay_style_resolver.hpp
    │   ├── zlay_pseudo_state.hpp
    │   ├── zlay_variant.hpp
    │   └── zlay_style_token.hpp
    │
    ├── theme/
    │   ├── zlay_theme.hpp
    │   ├── zlay_theme_data.hpp
    │   ├── zlay_theme_provider.hpp
    │   ├── zlay_color_scheme.hpp
    │   ├── zlay_typography.hpp
    │   ├── zlay_spacing_scale.hpp
    │   ├── zlay_radius_scale.hpp
    │   └── zlay_shadow_scale.hpp
    │
    ├── layout/
    ├── visual/
    ├── text/
    ├── input/
    ├── form/
    ├── menu/
    ├── overlay/
    ├── navigation/
    ├── scroll/
    ├── data/
    ├── painting/
    ├── animation/
    ├── interaction/
    ├── shortcut/
    ├── accessibility/
    │
    ├── tooling/
    │   ├── dock/
    │   ├── panels/
    │   ├── toolbar/
    │   ├── command/
    │   ├── console/
    │   └── profiler/
    │
    ├── viewport/
    │   ├── zlay_viewport.hpp
    │   ├── zlay_viewport_overlay.hpp
    │   ├── zlay_gizmo.hpp
    │   ├── zlay_grid_overlay.hpp
    │   └── zlay_camera_panel.hpp
    │
    ├── scene/
    │   ├── zlay_scene_tree.hpp
    │   ├── zlay_scene_outliner.hpp
    │   ├── zlay_hierarchy_view.hpp
    │   └── zlay_entity_row.hpp
    │
    ├── inspector/
    │   ├── zlay_inspector.hpp
    │   ├── zlay_property_grid.hpp
    │   ├── zlay_property_row.hpp
    │   ├── zlay_component_inspector.hpp
    │   └── zlay_transform_panel.hpp
    │
    ├── assets/
    │   ├── zlay_asset_browser.hpp
    │   ├── zlay_asset_grid.hpp
    │   ├── zlay_asset_tree.hpp
    │   ├── zlay_asset_preview.hpp
    │   └── zlay_file_drop_zone.hpp
    │
    ├── graph/
    │   ├── zlay_node_graph.hpp
    │   ├── zlay_node.hpp
    │   ├── zlay_node_port.hpp
    │   ├── zlay_node_connection.hpp
    │   ├── zlay_material_graph.hpp
    │   └── zlay_shader_graph.hpp
    │
    ├── timeline/
    │   ├── zlay_timeline.hpp
    │   ├── zlay_timeline_ruler.hpp
    │   ├── zlay_keyframe_track.hpp
    │   ├── zlay_animation_track.hpp
    │   └── zlay_curve_editor.hpp
    │
    └── debug/
        ├── zlay_debug_overlay.hpp
        ├── zlay_layout_inspector.hpp
        ├── zlay_render_stats.hpp
        ├── zlay_bounds_debugger.hpp
        └── zlay_frame_graph_view.hpp
```

---

## 10. Component Runtime Classes

### 10.1 Base Classes

| Class | Role |
|---|---|
| `zlay::Component` | Base class for user-authored components. |
| `zlay::StatelessComponent` | Component with no mutable internal state. |
| `zlay::StatefulComponent` | Component that creates a state object. |
| `zlay::StateBase` | Base class for mutable state. |
| `zlay::ComponentState<T>` | Typed state for a component. |
| `zlay::Widget` | Lightweight immutable UI description. |
| `zlay::Element` | Mounted runtime instance. |
| `zlay::RenderObject` | Layout/paint object. |
| `zlay::BuildContext` | Access to theme, state, services, parent tree. |
| `zlay::Key` | Stable identity used by reconciliation. |

### 10.2 Difference Between Widget, Component, Element, RenderObject

| Layer | Lifetime | Mutable? | Purpose |
|---|---:|---:|---|
| `Widget` | Short | No | Configuration object. |
| `Component` | Medium | Usually no | User-authored class object. |
| `State` | Long | Yes | Mutable behavior. |
| `Element` | Long | Yes | Mounted runtime node. |
| `RenderObject` | Long | Yes | Layout and paint node. |

---

## 11. Public API Examples

### 11.1 Stateless Component

```cpp
class MainPanel final : public zlay::StatelessComponent {
public:
    zlay::Widget build(zlay::BuildContext& ctx) const override {
        return zlay::Column({
            zlay::Text("Engine Tool"),
            zlay::Button("Run")
        });
    }
};
```

### 11.2 Stateful Component

```cpp
class Counter final : public zlay::StatefulComponent {
public:
    class State final : public zlay::ComponentState<Counter> {
        int value = 0;

    public:
        zlay::Widget build(zlay::BuildContext& ctx) override {
            return zlay::Column({
                zlay::Text("Counter: " + std::to_string(value)),
                zlay::Button({
                    .text = "Increment",
                    .onClick = [this] {
                        setState([&] {
                            value++;
                        });
                    }
                })
            });
        }
    };

    std::unique_ptr<zlay::StateBase> createState() override {
        return std::make_unique<State>();
    }
};
```

### 11.3 Engine Tooling UI

```cpp
class EngineWorkbench final : public zlay::StatelessComponent {
public:
    zlay::Widget build(zlay::BuildContext& ctx) const override {
        return zlay::DockSpace({
            zlay::DockTab("Viewport", zlay::Viewport()),
            zlay::DockTab("Scene", zlay::SceneOutliner()),
            zlay::DockTab("Inspector", zlay::Inspector()),
            zlay::DockTab("Assets", zlay::AssetBrowser()),
            zlay::DockTab("Console", zlay::ConsolePanel())
        });
    }
};
```

---

## 12. Lifecycle Model

### 12.1 Stateless Lifecycle

```txt
construct
  ↓
build
  ↓
diff/reconcile
  ↓
layout
  ↓
paint
  ↓
destroy
```

### 12.2 Stateful Lifecycle

```txt
construct component
  ↓
createState
  ↓
state.init
  ↓
state.build
  ↓
state.didUpdate
  ↓
state.setState
  ↓
rebuild
  ↓
state.dispose
```

### 12.3 State API

```cpp
class StateBase {
public:
    virtual ~StateBase() = default;

    virtual void initState() {}
    virtual void didMount() {}
    virtual void didUpdate() {}
    virtual void beforeUnmount() {}
    virtual void dispose() {}

    virtual Widget build(BuildContext& ctx) = 0;

protected:
    template<class Fn>
    void setState(Fn&& fn);
};
```

---

## 13. WebKit-Like Pipeline in Zlay

Zlay should use the same discipline as a rendering engine:

```txt
1. Build
   Component.build()

2. Reconcile
   Widget tree compared with old Element tree

3. Style Resolve
   Theme + Style + PseudoState resolved

4. Layout
   Constraint → Size → Position

5. Paint
   RenderObject.paint(Canvas)

6. Record
   Canvas produces RenderCommandList

7. Layer
   Commands grouped into layers

8. Composite
   Layers composited

9. Backend Render
   OpenGL/Vulkan/WebGPU/raylib/sokol

10. Present
   Swapchain/window present
```

This gives Zlay the structure of a serious UI rendering engine without depending on WebKit.

---

## 14. Reconciliation Algorithm

### 14.1 Basic Rules

```txt
if old element is null:
    mount new widget

else if old.type_id == new.type_id and old.key == new.key:
    update existing element

else:
    unmount old element
    mount new widget
```

### 14.2 Pseudocode

```cpp
Element* reconcile(Element* oldElement, const Widget& newWidget, Slot slot) {
    if (!oldElement) {
        return mount(newWidget, slot);
    }

    if (oldElement->type_id() == newWidget.type_id()
        && oldElement->key() == newWidget.key()) {
        oldElement->update(newWidget);
        return oldElement;
    }

    unmount(oldElement);
    return mount(newWidget, slot);
}
```

### 14.3 Child Diff

```txt
1. Match children by key.
2. If no key, match by index and type.
3. Update matching elements.
4. Remove old unmatched elements.
5. Mount new unmatched widgets.
```

Future optimization:

```txt
- keyed map
- longest increasing subsequence for reorder
- dirty subtree rebuilding
- component-level memoization
```

---

## 15. Layout Algorithm

### 15.1 Constraint Model

```cpp
struct BoxConstraints {
    float min_width;
    float max_width;
    float min_height;
    float max_height;
};
```

### 15.2 Size Model

```cpp
struct Size {
    float width;
    float height;
};
```

### 15.3 Render Object API

```cpp
class RenderObject {
public:
    virtual ~RenderObject() = default;

    virtual void layout(const BoxConstraints& constraints) = 0;
    virtual void paint(Canvas& canvas) = 0;

    Size size() const;
    Offset offset() const;
};
```

### 15.4 Flex Layout

For `Row` and `Column`:

```txt
1. Measure non-flex children.
2. Sum fixed size.
3. Compute remaining size.
4. Divide remaining size among flex children.
5. Layout flex children with assigned constraints.
6. Position children by main axis and cross axis alignment.
```

### 15.5 Stack Layout

```txt
1. Layout non-positioned children.
2. Compute stack size.
3. Layout positioned children.
4. Assign offsets.
```

### 15.6 Scroll Layout

```txt
1. Layout child with unbounded main axis.
2. Store content size.
3. Compute viewport size.
4. Apply scroll offset.
5. Clip viewport.
6. Paint visible region.
```

### 15.7 Virtual List Layout

```txt
1. Estimate item height.
2. Compute visible index range from scroll offset.
3. Build/mount only visible children.
4. Recycle item elements when possible.
5. Maintain stable keys for visible range.
```

---

## 16. Event System

### 16.1 Event Flow

```txt
platform event
  ↓
Zlay input system
  ↓
hit testing
  ↓
capture phase
  ↓
target component
  ↓
bubble phase
  ↓
state update
  ↓
dirty/rebuild
```

### 16.2 Event Types

| Event | Use |
|---|---|
| Pointer move | Hover, drag, viewport interaction |
| Pointer down/up | Button, slider, graph, canvas |
| Pointer wheel | Scroll, zoom |
| Key down/up | Shortcut, text field |
| Text input | Editable text |
| Focus | Form/input/tooling |
| Drag/drop | Asset browser, docking |
| Resize | Split panel, floating window |
| IME | Future text input |

### 16.3 Hit Testing

```txt
1. Start from root render object.
2. Visit children in reverse paint order.
3. Check bounds.
4. If hit, push hit target.
5. Dispatch event to deepest target.
```

---

## 17. Metaprogramming Plan

Zlay Components should use C++ metaprogramming to keep the class model type-safe and scalable.

### 17.1 Required Techniques

| Technique | C++ Version | Use |
|---|---:|---|
| Variadic templates | C++11 | Children lists, builder DSL |
| Perfect forwarding | C++11 | Efficient props/component construction |
| `std::enable_if` | C++11 | Conditional overloads |
| Type traits | C++11 | Detect component/widget/render object types |
| CRTP | C++11 | Static internal components |
| Tag dispatch | C++11 | Select behavior by category |
| Tuple expansion | C++11/14 | Static children processing |
| Detection idiom | C++11/17 | Detect `build()`, `paint()`, props fields |
| `constexpr` | C++11/14/17/20 | Type IDs, flags, static strings |
| `std::integer_sequence` | C++14 | Tuple iteration |
| `if constexpr` | C++17 | Compile-time branching |
| Fold expressions | C++17 | Process variadic children |
| `std::variant` | C++17 | Value-based widget union |
| `std::optional` | C++17 | Optional props |
| `std::any` | C++17 | Type-erased state/services |
| Inline variables | C++17 | Global type tokens |
| Concepts | C++20 | Clean component constraints |
| `requires` | C++20 | Type-safe builder APIs |
| `consteval` | C++20 | Compile-time type/name hashing |

---

## 18. Type Traits

### 18.1 Basic Component Trait

```cpp
template<class T>
struct is_component {
    static constexpr bool value =
        std::is_base_of<zlay::Component, T>::value;
};
```

### 18.2 Detection Idiom

```cpp
template<class, class = void>
struct has_build : std::false_type {};

template<class T>
struct has_build<T, void_t<decltype(std::declval<T>().build(
    std::declval<zlay::BuildContext&>()
))>> : std::true_type {};
```

### 18.3 C++20 Concept

```cpp
template<class T>
concept ComponentLike = requires(T t, zlay::BuildContext& ctx) {
    { t.build(ctx) } -> std::same_as<zlay::Widget>;
};
```

---

## 19. Type ID System

Zlay should not rely only on RTTI.

### 19.1 Runtime Type ID

```cpp
using TypeId = uint64_t;

template<class T>
TypeId type_id() {
    static const TypeId id = hash_type_name(__PRETTY_FUNCTION__);
    return id;
}
```

### 19.2 C++20 Consteval Type ID

```cpp
template<class T>
consteval uint64_t static_type_id() {
    return fnv1a(__PRETTY_FUNCTION__);
}
```

### 19.3 Type ID Use Cases

```txt
widget reconciliation
component registry
serialization
debug inspector
hot reload future
style/type mapping
```

---

## 20. Type Erasure

Zlay needs type erasure to store different widgets in one tree.

### 20.1 Any Widget

```cpp
class AnyWidget {
public:
    template<class T>
    AnyWidget(T widget);

    TypeId type_id() const;
    const Key& key() const;

    Element* mount(BuildContext& ctx) const;

private:
    struct Concept;
    template<class T>
    struct Model;

    std::shared_ptr<const Concept> self;
};
```

### 20.2 Why

A `Column` may contain:

```txt
Text
Button
Slider
CustomComponent
```

All must fit into:

```cpp
std::vector<Widget>
```

---

## 21. Props System

### 21.1 Plain Struct Props

```cpp
struct ButtonProps {
    std::string text;
    std::function<void()> onClick;
    ButtonVariant variant = ButtonVariant::Default;
    bool disabled = false;
};
```

### 21.2 Builder Props for C++11

```cpp
class ButtonPropsBuilder {
public:
    ButtonPropsBuilder& text(std::string value);
    ButtonPropsBuilder& onClick(std::function<void()> fn);
    ButtonPropsBuilder& disabled(bool value = true);

    ButtonProps build() const;
};
```

### 21.3 C++20 Designated Initializer Style

```cpp
zlay::Button({
    .text = "Save",
    .variant = zlay::ButtonVariant::Primary,
    .onClick = [] {}
});
```

---

## 22. State Management

### 22.1 Local State

```cpp
setState([&] {
    counter++;
});
```

### 22.2 Signal State

```cpp
zlay::Signal<int> counter = 0;
```

### 22.3 Value Notifier

```cpp
zlay::ValueNotifier<bool> enabled(true);
```

### 22.4 Provider

```cpp
zlay::Provider<AppState>({
    .value = appState,
    .child = MyApp()
});
```

### 22.5 Selector

```cpp
zlay::Selector<AppState, int>({
    .select = [](const AppState& s) { return s.counter; },
    .builder = [](int counter) { return zlay::Text(counter); }
});
```

---

## 23. Style System

Zlay style system replaces CSS for native components.

### 23.1 Pseudo States

```txt
normal
hover
active
focus
disabled
selected
dragging
error
```

### 23.2 Style Resolution

```cpp
Style resolved = resolver.resolve({
    .variant = "primary",
    .state = PseudoState::Hover | PseudoState::Focus
});
```

### 23.3 Theme Access

```cpp
auto& theme = ctx.theme();
auto color = theme.colors.primary;
```

---

## 24. Component Categories

| Package | Focus |
|---|---|
| `runtime` | Component, Widget, Element, State, Key, BuildContext |
| `meta` | Type traits, type id, detection, type erasure, concepts |
| `hooks` | Mount, unmount, update, dirty, rebuild, effects |
| `state` | Signal, provider, notifier, store |
| `style` | Style object, resolver, pseudo state, variants |
| `theme` | Theme data, color scheme, typography, spacing |
| `layout` | Container, Row, Column, Stack, Grid, Dock, Split |
| `visual` | Box, Surface, Panel, Card, Divider |
| `text` | Text, RichText, EditableText, TextLayout |
| `input` | Button, TextField, Checkbox, Slider, Dropdown |
| `form` | Form, Validator, FormState |
| `menu` | MenuBar, Menu, MenuItem, ContextMenu |
| `overlay` | Dialog, Tooltip, Popup, Toast |
| `navigation` | AppBar, Toolbar, Tabs, Navigator |
| `scroll` | ScrollView, ListView, VirtualList |
| `data` | TreeView, TableView, PropertyGrid |
| `painting` | CustomPaint, CanvasView, Shape, Clip, Transform |
| `animation` | Tween, Animator, Transitions |
| `interaction` | Gesture, Hover, Drag, Drop, Resize |
| `shortcut` | Shortcuts, Commands, Actions |
| `accessibility` | Semantics, roles, accessibility tree |
| `tooling` | Docking, panels, command palette, console, profiler |
| `viewport` | Viewport, overlay, gizmo, camera panel |
| `scene` | Scene tree, outliner, hierarchy, entity row |
| `inspector` | Inspector, property grid, transform panel |
| `assets` | Asset browser, preview, tree, file drop |
| `graph` | Node graph, material graph, shader graph |
| `timeline` | Animation/keyframe/sequence timeline |
| `debug` | Layout inspector, bounds debugger, render stats |

---

## 25. Component List

### 25.1 Runtime

| Component/Class | Purpose |
|---|---|
| `ZLayComponent` | Base object semua komponen |
| `ZLayStatelessComponent` | Komponen tanpa state |
| `ZLayStatefulComponent` | Komponen dengan state |
| `ZLayWidget` | Handle immutable |
| `ZLayElement` | Mounted node |
| `ZLayState` | Mutable state |
| `ZLayBuildContext` | Context build |
| `ZLayKey` | Stable identity |
| `ZLayReconciler` | Tree diff/update |
| `ZLayComponentRegistry` | Registry component |

### 25.2 Layout

| Component | Purpose |
|---|---|
| `ZLayContainer` | Container dasar |
| `ZLayRow` | Horizontal layout |
| `ZLayColumn` | Vertical layout |
| `ZLayStack` | Layered layout |
| `ZLayPositioned` | Child positioned |
| `ZLayAlign` | Alignment |
| `ZLayCenter` | Center child |
| `ZLayPadding` | Padding |
| `ZLayMargin` | Margin |
| `ZLaySizedBox` | Fixed size |
| `ZLayFlexible` | Flexible child |
| `ZLayExpanded` | Expanded child |
| `ZLaySpacer` | Spacer |
| `ZLayWrap` | Wrap layout |
| `ZLayGrid` | Grid layout |
| `ZLaySplitView` | Split panes |
| `ZLayDockLayout` | Dockable layout |
| `ZLayOverlayLayout` | Overlay positioning |

### 25.3 Visual

| Component | Purpose |
|---|---|
| `ZLayBox` | Box primitive |
| `ZLaySurface` | Surface/panel dasar |
| `ZLayPanel` | Panel umum |
| `ZLayCard` | Card UI |
| `ZLayDivider` | Divider |
| `ZLayShadowBox` | Shadow surface |
| `ZLayGradientBox` | Gradient surface |
| `ZLayGlassPanel` | Glass/blur panel |

### 25.4 Text

| Component | Purpose |
|---|---|
| `ZLayText` | Text basic |
| `ZLayRichText` | Rich text |
| `ZLayTextSpan` | Text span |
| `ZLaySelectableText` | Selectable text |
| `ZLayEditableText` | Editable text |
| `ZLayTextSelection` | Selection model |
| `ZLayTextCursor` | Cursor/caret |
| `ZLayTextLayout` | Text layout |
| `ZLayFontHandle` | Font reference |

### 25.5 Input

| Component | Purpose |
|---|---|
| `ZLayButton` | Button |
| `ZLayIconButton` | Icon button |
| `ZLayTextButton` | Text button |
| `ZLayCheckbox` | Checkbox |
| `ZLayRadio` | Radio |
| `ZLaySwitch` | Switch |
| `ZLaySlider` | Slider |
| `ZLayRangeSlider` | Range slider |
| `ZLayTextField` | Single-line input |
| `ZLayTextArea` | Multi-line input |
| `ZLayNumberInput` | Number input |
| `ZLayComboBox` | Combo box |
| `ZLayDropdown` | Dropdown |
| `ZLaySearchInput` | Search input |
| `ZLayColorPicker` | Color picker |
| `ZLayHotkeyInput` | Shortcut input |

### 25.6 Engine Tooling

| Domain | Component | Purpose |
|---|---|---|
| Tooling | `ZLayDockSpace` | Main docking area |
| Tooling | `ZLayDockNode` | Dock node |
| Tooling | `ZLayDockTab` | Dock tab |
| Tooling | `ZLayCommandPalette` | Command palette |
| Tooling | `ZLayConsolePanel` | Console/log panel |
| Tooling | `ZLayProfilerPanel` | Profiler UI |
| Viewport | `ZLayViewport` | Render viewport |
| Viewport | `ZLayViewportOverlay` | 2D overlay |
| Viewport | `ZLayGizmo` | Transform gizmo |
| Viewport | `ZLayGridOverlay` | Viewport grid |
| Viewport | `ZLayCameraPanel` | Camera controls |
| Scene | `ZLaySceneTree` | Scene tree |
| Scene | `ZLaySceneOutliner` | Outliner |
| Scene | `ZLayHierarchyView` | Hierarchy |
| Scene | `ZLayEntityRow` | Entity row |
| Inspector | `ZLayInspector` | Inspector root |
| Inspector | `ZLayPropertyGrid` | Property grid |
| Inspector | `ZLayPropertyRow` | Property row |
| Inspector | `ZLayComponentInspector` | Component inspector |
| Inspector | `ZLayTransformPanel` | Transform fields |
| Assets | `ZLayAssetBrowser` | Asset browser |
| Assets | `ZLayAssetGrid` | Asset grid |
| Assets | `ZLayAssetTree` | Asset tree |
| Assets | `ZLayAssetPreview` | Asset preview |
| Assets | `ZLayFileDropZone` | File drop target |
| Graph | `ZLayNodeGraph` | Node graph |
| Graph | `ZLayNode` | Node |
| Graph | `ZLayNodePort` | Node port |
| Graph | `ZLayNodeConnection` | Node connection |
| Graph | `ZLayMaterialGraph` | Material graph |
| Graph | `ZLayShaderGraph` | Shader graph |
| Timeline | `ZLayTimeline` | Timeline root |
| Timeline | `ZLayTimelineRuler` | Ruler |
| Timeline | `ZLayKeyframeTrack` | Keyframes |
| Timeline | `ZLayAnimationTrack` | Animation track |
| Timeline | `ZLayCurveEditor` | Curve editor |
| Debug | `ZLayDebugOverlay` | Debug overlay |
| Debug | `ZLayLayoutInspector` | Layout inspector |
| Debug | `ZLayRenderStats` | Render stats |
| Debug | `ZLayBoundsDebugger` | Bounds debug |
| Debug | `ZLayFrameGraphView` | Frame graph UI |

---

## 26. Required Algorithms

| Algorithm | Use Case |
|---|---|
| Tree reconciliation | Update element tree efficiently |
| Keyed diff | Preserve state across reorder |
| Dirty flag propagation | Rebuild only changed subtrees |
| Constraint layout | Compute size from constraints |
| Flex layout | Row/Column |
| Stack layout | Overlay and positioned children |
| Scroll virtualization | Large lists/timelines/asset browser |
| Hit testing | Pointer event dispatch |
| Capture/bubble events | Interaction routing |
| Focus traversal | Keyboard navigation |
| Shortcut matching | Command system |
| Style resolution | Variant + pseudo state |
| Theme inheritance | Propagate theme down tree |
| Command pattern | Undo/redo/tool actions |
| Observer pattern | Signals/notifiers |
| Visitor pattern | Tree traversal/debug inspector |
| Type erasure | Store heterogeneous widgets |
| Policy-based design | Custom layout/paint/event behavior |
| CRTP dispatch | Static internal components |
| Tuple iteration | Static children and compile-time UI |
| Variant visitation | Value widget union |
| Object pool | Element/render object reuse |
| Arena allocation | Fast frame/temp allocation |
| Small buffer optimization | Avoid heap for small callbacks/children |
| Intrusive list | Efficient tree nodes |
| Topological ordering | Dependency graph for state/effects |
| LRU cache | Font glyph/image/style cache |
| Spatial indexing | Hit testing in graph/viewport |
| R-tree/grid index | Picking and node graph performance |
| Longest increasing subsequence | Optimized keyed reorder |

---

## 27. Memory Strategy

| Allocation | Strategy |
|---|---|
| Frame temporary | Arena allocator |
| Widget config | Value object or small heap |
| Element tree | Pool allocator |
| Render objects | Pool allocator |
| Text layout | Cache + arena |
| Events | Ring buffer |
| Commands | Vector or arena list |
| Styles | Interned tokens / shared immutable data |

Ownership rules:

```txt
Widget owns configuration.
Element owns mounted lifecycle.
State owns mutable component data.
RenderObject owns layout/paint state.
Context references services, theme, and tree.
```

---

## 28. Build Configuration

### 28.1 CMake Options

```cmake
option(ZLAY_ENABLE_COMPONENTS "Enable Zlay C++ component layer" ON)
option(ZLAY_ENABLE_COMPONENTS_META "Enable metaprogramming utilities" ON)
option(ZLAY_ENABLE_COMPONENTS_TOOLING "Enable engine tooling components" ON)
option(ZLAY_ENABLE_COMPONENTS_VIEWPORT "Enable viewport components" ON)
option(ZLAY_ENABLE_COMPONENTS_SCENE "Enable scene components" ON)
option(ZLAY_ENABLE_COMPONENTS_INSPECTOR "Enable inspector components" ON)
option(ZLAY_ENABLE_COMPONENTS_ASSETS "Enable asset components" ON)
option(ZLAY_ENABLE_COMPONENTS_GRAPH "Enable graph components" ON)
option(ZLAY_ENABLE_COMPONENTS_TIMELINE "Enable timeline components" ON)

set(ZLAY_COMPONENTS_CPP_STANDARD "17" CACHE STRING "C++ standard for components")
```

### 28.2 C++ Standard Profiles

| Profile | Standard | Description |
|---|---:|---|
| Legacy | C++11 | Basic inheritance, type traits, variadic templates |
| Stable | C++14 | Better constexpr, integer_sequence |
| Recommended | C++17 | variant, optional, any, if constexpr, fold expressions |
| Modern | C++20 | concepts, requires, consteval, designated initialization |

---

## 29. Header Aggregation

Create:

```txt
include/zlay/components.hpp
```

Stable public components:

```cpp
#pragma once

#include <zlay/components/runtime/zlay_component.hpp>
#include <zlay/components/runtime/zlay_widget.hpp>
#include <zlay/components/runtime/zlay_build_context.hpp>

#include <zlay/components/layout/zlay_container.hpp>
#include <zlay/components/layout/zlay_row.hpp>
#include <zlay/components/layout/zlay_column.hpp>
#include <zlay/components/layout/zlay_stack.hpp>

#include <zlay/components/text/zlay_text.hpp>
#include <zlay/components/input/zlay_button.hpp>
#include <zlay/components/input/zlay_text_field.hpp>
#include <zlay/components/visual/zlay_panel.hpp>
```

Engine tooling modules can be separated:

```txt
<zlay/components_tooling.hpp>
<zlay/components_viewport.hpp>
<zlay/components_scene.hpp>
<zlay/components_inspector.hpp>
<zlay/components_assets.hpp>
<zlay/components_graph.hpp>
<zlay/components_timeline.hpp>
```

---

## 30. Implementation Phases

### Phase 1: Runtime Class System

| Task | Output |
|---|---|
| Add `Component` | User can inherit class |
| Add `StatelessComponent` | User can override `build` |
| Add `StatefulComponent` | User can create state |
| Add `Widget` handle | Heterogeneous tree storage |
| Add `Element` | Mounted runtime instance |
| Add `BuildContext` | Access theme/services |
| Add `Key` | Reconciliation identity |

### Phase 2: Basic Components

| Task | Output |
|---|---|
| Add `Text` | Basic text |
| Add `Container` | Basic layout/visual box |
| Add `Row`/`Column` | Flex layout |
| Add `Button` | Click interaction |
| Add `ScrollView` | Basic scroll |
| Add `CanvasView` | Custom painting |

### Phase 3: State and Style

| Task | Output |
|---|---|
| Add `setState` | Stateful rebuild |
| Add `Signal` | Reactive state |
| Add `Theme` | Global styling |
| Add `StyleResolver` | Hover/focus/active states |
| Add `Shortcuts` | Keyboard actions |

### Phase 4: Engine Tooling Components

| Task | Output |
|---|---|
| Add `DockSpace` | Engine panels |
| Add `SplitView` | Resizable areas |
| Add `Inspector` | Property editing |
| Add `SceneOutliner` | Scene hierarchy |
| Add `AssetBrowser` | Asset UI |
| Add `ConsolePanel` | Logs |
| Add `Viewport` | Render viewport wrapper |
| Add `Graph` | Material/shader graph |
| Add `Timeline` | Animation/keyframe timeline |

---

## 31. AI Agent Implementation Prompt

```txt
Implement Zlay C++ Components as a Flutter-like class object layer focused only on engine UI.

Rules:

1. Components must live under:
   zlay/components/

2. Do not create top-level:
   zlay/components/ide/
   zlay/components/daw/
   zlay/components/editor/

3. Use engine-focused folders:
   tooling/
   viewport/
   scene/
   inspector/
   assets/
   graph/
   timeline/
   debug/

4. Do not use zlay/components/core.
   Use:
   zlay/components/runtime/

5. Do not use zlay/components/lifecycle.
   Use:
   zlay/components/hooks/

6. Keep zlay/core for global foundation:
   memory, arena, cache, hash, logger, base types.

7. Keep zlay/lifecycle for app/session lifecycle:
   autosave, history, journal, recovery, snapshot, session.

8. The public API should allow:
   class MyComponent : public zlay::StatelessComponent
   class MyComponent : public zlay::StatefulComponent

9. Add core runtime types:
   - zlay::Widget
   - zlay::Component
   - zlay::StatelessComponent
   - zlay::StatefulComponent
   - zlay::StateBase
   - zlay::ComponentState<T>
   - zlay::Element
   - zlay::RenderObject
   - zlay::BuildContext
   - zlay::Key

10. Implement reconciliation:
   - compare type_id
   - compare key
   - update matching element
   - unmount/mount non-matching element

11. Use C++11-compatible base implementation.

12. Add C++17/C++20 enhanced paths when available:
   - std::optional
   - std::variant
   - if constexpr
   - fold expressions
   - concepts
   - requires
   - consteval type id

13. Do not expose OpenGL, Vulkan, GLFW, ImGui, raylib, sokol, or Clay types in component public headers.

14. Components should build render/layout objects, not call GPU APIs.

15. Components may use Canvas through:
   zlay/renderer/canvas/

16. Start implementation with:
   - Component
   - StatelessComponent
   - StatefulComponent
   - Widget
   - Element
   - BuildContext
   - Text
   - Container
   - Row
   - Column
   - Button
   - CanvasView

17. Keep ABI-sensitive plugin boundary separate from C++ inheritance layer.
```

---

## 32. Final Rules

```txt
zlay/core
= global low-level foundation

zlay/components/runtime
= component object model

zlay/lifecycle
= app/session lifecycle

zlay/components/hooks
= component lifecycle hooks

tooling
= engine tools, not generic editor product

timeline
= animation/keyframe/sequence timeline, not DAW

graph
= material/shader/behavior graph, not broad node editor product

viewport/scene/inspector/assets
= engine-focused component domains
```

Final architecture rule:

```txt
Component = user-facing class object.
Widget = lightweight immutable configuration.
Element = mounted runtime instance.
RenderObject = layout and paint object.
Canvas = drawing abstraction.
RendererBackend = translates render commands.
GpuDriver = low-level GPU/API handler.
```

Zlay Components should feel easy like Flutter-style UI composition, but internally remain native, renderer-agnostic, backend-clean, and focused on engine UI.
