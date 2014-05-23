#ifndef GLFWHCK_H
#define GLFWHCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "GLFW/glfw3.h"

typedef enum glfwhckEventType {
  GLFWHCK_EVENT_WINDOW_POSITION = 1 << 1,
  GLFWHCK_EVENT_WINDOW_RESIZE = 1 << 2,
  GLFWHCK_EVENT_WINDOW_CLOSE = 1 << 3,
  GLFWHCK_EVENT_WINDOW_REFRESH = 1 << 4,
  GLFWHCK_EVENT_WINDOW_FOCUS = 1 << 5,
  GLFWHCK_EVENT_WINDOW_ICONIFY = 1 << 6,
  GLFWHCK_EVENT_FRAMEBUFFER_RESIZE = 1 << 7,
  GLFWHCK_EVENT_MOUSE_BUTTON = 1 << 8,
  GLFWHCK_EVENT_MOUSE_POSITION = 1 << 9,
  GLFWHCK_EVENT_MOUSE_ENTER = 1 << 10,
  GLFWHCK_EVENT_MOUSE_SCROLL = 1 << 11,
  GLFWHCK_EVENT_KEYBOARD_KEY = 1 << 12,
  GLFWHCK_EVENT_KEYBOARD_CHAR = 1 << 13,
} glfwhckEventType;

#define GLFWHCK_EVENTS_WINDOW GLFWHCK_EVENT_WINDOW_POSITION | GLFWHCK_EVENT_WINDOW_RESIZE \
  | GLFWHCK_EVENT_WINDOW_CLOSE | GLFWHCK_EVENT_WINDOW_REFRESH | GLFWHCK_EVENT_WINDOW_FOCUS \
  | GLFWHCK_EVENT_WINDOW_ICONIFY

#define GLFWHCK_EVENTS_MOUSE GLFWHCK_EVENT_MOUSE_BUTTON | GLFWHCK_EVENT_MOUSE_POSITION \
  | GLFWHCK_EVENT_MOUSE_ENTER | GLFWHCK_EVENT_MOUSE_SCROLL

#define GLFWHCK_EVENTS_KEYBOARD GLFWHCK_EVENT_KEYBOARD_KEY | GLFWHCK_EVENT_KEYBOARD_CHAR

#define GLFWHCK_EVENTS_ALL GLFWHCK_EVENT_WINDOW_POSITION | GLFWHCK_EVENT_WINDOW_RESIZE \
    | GLFWHCK_EVENT_WINDOW_CLOSE | GLFWHCK_EVENT_WINDOW_REFRESH | GLFWHCK_EVENT_WINDOW_FOCUS \
    | GLFWHCK_EVENT_WINDOW_ICONIFY | GLFWHCK_EVENT_FRAMEBUFFER_RESIZE | GLFWHCK_EVENT_MOUSE_BUTTON \
    | GLFWHCK_EVENT_MOUSE_POSITION | GLFWHCK_EVENT_MOUSE_ENTER | GLFWHCK_EVENT_MOUSE_SCROLL \
    | GLFWHCK_EVENT_KEYBOARD_KEY | GLFWHCK_EVENT_KEYBOARD_CHAR

typedef struct _glfwhckEventQueue glfwhckEventQueue;

typedef struct glfwhckEvent
{
  glfwhckEventType type;
  union
  {
    struct
    {
      int error;
      char const* description;
    } error;
    struct
    {
      int x;
      int y;
    } windowPosition;
    struct
    {
      int width;
      int height;
    } windowResize;
    struct
    {
      // No event data
    } windowClose;
    struct
    {
      // No event data
    } windowRefresh;
    struct
    {
      int focused;
    } windowFocus;
    struct
    {
      int iconified;
    } windowIconify;
    struct
    {
      int width;
      int height;
    } framebufferResize;
    struct
    {
      int button;
      int action;
      int mods;
    } mouseButton;
    struct
    {
      double x;
      double y;
    } mousePosition;
    struct
    {
      int entered;
    } mouseEnter;
    struct
    {
      double xOffset;
      double yOffset;
    } mouseScroll;
    struct
    {
      int key;
      int scancode;
      int action;
      int mods;
    } keyboardKey;
    struct
    {
      unsigned int codepoint;
    } keyboardChar;
  };
} glfwhckEvent;

glfwhckEventQueue* glfwhckEventQueueNew(GLFWwindow* window, int const eventTypes);
void glfwhckEventQueueFree(glfwhckEventQueue* queue);
void glfwhckEventQueuePush(glfwhckEventQueue* queue, glfwhckEvent const* event);
glfwhckEvent const* glfwhckEventQueuePop(glfwhckEventQueue* queue);
int glfwhckEventQueueEmpty(glfwhckEventQueue* queue);

#ifdef __cplusplus
}
#endif

#endif
