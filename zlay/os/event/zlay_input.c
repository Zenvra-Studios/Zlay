#include <os/event/zlay_input.h>

#include <string.h>

void ZLay_InputBeginFrame(ZLay_InputState* input) {
  if (input == NULL) {
    return;
  }

  input->mouse_dx = 0.0f;
  input->mouse_dy = 0.0f;
  input->wheel_x = 0.0f;
  input->wheel_y = 0.0f;
  input->text_input_count = 0u;
  memset(input->mouse_pressed, 0, sizeof(input->mouse_pressed));
  memset(input->mouse_released, 0, sizeof(input->mouse_released));
  memset(input->key_pressed, 0, sizeof(input->key_pressed));
  memset(input->key_released, 0, sizeof(input->key_released));
}

void ZLay_InputProcessEvent(ZLay_InputState* input, const ZLay_Event* event) {
  if (input == NULL || event == NULL) {
    return;
  }

  switch (event->type) {
    case ZLAY_EVENT_MOUSE_MOVE:
      input->mouse_x = event->mouse_move.x;
      input->mouse_y = event->mouse_move.y;
      input->mouse_dx += event->mouse_move.dx;
      input->mouse_dy += event->mouse_move.dy;
      break;

    case ZLAY_EVENT_MOUSE_BUTTON_DOWN:
      if (event->mouse_button.button >= 0 && event->mouse_button.button < 8) {
        input->mouse_down[event->mouse_button.button] = true;
        input->mouse_pressed[event->mouse_button.button] = true;
      }
      input->mouse_x = event->mouse_button.x;
      input->mouse_y = event->mouse_button.y;
      break;

    case ZLAY_EVENT_MOUSE_BUTTON_UP:
      if (event->mouse_button.button >= 0 && event->mouse_button.button < 8) {
        input->mouse_down[event->mouse_button.button] = false;
        input->mouse_released[event->mouse_button.button] = true;
      }
      input->mouse_x = event->mouse_button.x;
      input->mouse_y = event->mouse_button.y;
      break;

    case ZLAY_EVENT_MOUSE_WHEEL:
      input->wheel_x += event->wheel.x;
      input->wheel_y += event->wheel.y;
      break;

    case ZLAY_EVENT_KEY_DOWN:
      if (event->key.key >= 0 && event->key.key < 512) {
        input->key_down[event->key.key] = true;
        input->key_pressed[event->key.key] = true;
      }
      input->modifiers = event->key.modifiers;
      break;

    case ZLAY_EVENT_KEY_UP:
      if (event->key.key >= 0 && event->key.key < 512) {
        input->key_down[event->key.key] = false;
        input->key_released[event->key.key] = true;
      }
      input->modifiers = event->key.modifiers;
      break;

    case ZLAY_EVENT_TEXT_INPUT:
      if (input->text_input_count < 32u) {
        input->text_input[input->text_input_count++] = event->text.codepoint;
      }
      break;

    default:
      break;
  }
}

void ZLay_InputEndFrame(ZLay_InputState* input) {
  (void)input;
}
