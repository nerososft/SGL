//
// Created by neo on 2025/2/9.
//

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <unistd.h>

enum EventType {
  EVENT_KEY_DOWN,
  EVENT_KEY_UP,
  EVENT_MOUSE_DOWN,
  EVENT_MOUSE_UP,
  EVENT_MOUSE_MOVE,
};

struct Event {
  EventType type;

  union {
    struct {
      uint32_t key = 0;
    } key;

    struct {
      uint32_t x = 0;
      uint32_t y = 0;
    } cursor;

    struct {
      uint32_t key = 0;
    } mouse;
  } data;
};

class IEventHandler {
public:
  IEventHandler() = default;

  virtual ~IEventHandler() = default;

  virtual void OnKeyUp(uint32_t key) = 0;

  virtual void OnKeyDown(uint32_t key) = 0;

  virtual void OnMouseDown(uint32_t key) = 0;

  virtual void OnMouseUp(uint32_t key) = 0;

  virtual void OnMouseMove(uint32_t x, uint32_t y) = 0;
};

#endif // EVENTHANDLER_H
