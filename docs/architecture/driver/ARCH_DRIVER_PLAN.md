# ZLay Driver Architecture Plan

This plan merges the infrastructure direction from `ARCH2.md` with the lower-level rendering details from `ARCH1.md`.

## Direction

The driver layer is the bridge between ZLay render commands and platform GPU backends. It should stay independent from widget/layout logic and only receive `ZLay_RenderCommandList` plus frame/surface state.

## Layers

1. `ZLay_Surface`
   Owns native window/display handles, drawable size, DPI scale, and resize validity.

2. `ZLay_PresentConfig`
   Owns vsync, low-latency mode, present mode, and frames-in-flight selection.

3. `ZLay_FrameScheduler`
   Rotates frame resources and tracks the monotonic frame index.

4. `ZLay_DirtyState`
   Tracks style/layout/text/paint/surface/GPU invalidation so future renderers can skip unnecessary uploads.

5. `ZLay_Batch`
   Converts stable render commands into generic vertex/index buffers. This is the shared path before backend-specific GPU upload.

6. `ZLay_Driver`
   Orchestrates create, resize, begin frame, submit, end frame, present, stats, and backend API dispatch.

7. Backend adapters
   OpenGL/Vulkan expose `ZLay_DriverAPI` hooks and consume the shared `ZLay_DriverFramePacket`. Real GL/Vk native calls can grow behind those hooks without changing the public driver facade.

8. `ZLay_DriverContext`
   Stores backend capabilities, native handles, and context generation. This is the driver-owned context layer from `ARCH2.md`.

9. `ZLay_Swapchain`
   Owns image count, format, present mode, acquire/release/present flow, and resize generation.

10. `ZLay_PipelineCache`
   Tracks rect/text/image pipeline readiness. Rect is available first; text/image become valid when atlas/texture support lands.

## Frame Flow

```text
input/update
  -> layout builds ZLay_RenderCommandList
  -> ZLay_DriverBeginFrame
  -> acquire swapchain image
  -> ZLay_BatchBuildFromCommands
  -> upload vertices/indices into persistent ring buffer
  -> backend submit
  -> backend end frame
  -> backend present
  -> dirty flags clear
```

## Technical Rules

- Driver code must not depend on parser or widget declarations.
- Backend-specific code should not rebuild layout; it consumes the already-built command list or shared batch.
- Surface resize marks `surface`, `layout`, `paint`, and `gpu_resources` dirty.
- Low-latency mode defaults to one frame in flight; normal mode defaults to two.
- Batching stays stable by command order and carries `z_index` per vertex for future sorting/pipeline work.
- Driver submit uploads batch memory once into the per-frame ring buffer before backend submission.
- Backend adapters must read `ZLay_DriverFramePacket`, not rebuild layout or command geometry.
- Text is counted separately now; glyph atlas and font batching should become a dedicated text batch path later.
- Stats must be queryable after a frame: commands, draw calls, primitive counts, vertex/index counts, overflow, dirty state, and frame index.

## Growth Path

1. Replace frame-packet recording with real OpenGL buffer mapping, shader bind, viewport/scissor, and `glDrawElements`.
2. Replace Vulkan packet recording with real instance/device/swapchain, command buffer, sync, and indexed draw submission.
3. Split text into glyph atlas, font cache, and text batch primitive.
4. Add clip/scissor stack emission instead of only counting clip commands.
5. Add backend capability query for MSAA, preferred present mode, max texture size, and shader language.
6. Add optional software backend using the same `ZLay_Batch` output.
