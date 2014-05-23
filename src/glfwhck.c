#include "glfwhck.h"
#include <stdlib.h>
#include <memory.h>

typedef struct _glfwhckEventQueue
{
  glfwhckEvent* buffer;
  unsigned int bufferSize;
  unsigned int bufferCapacity;
  unsigned int bufferPos;
} _glfwhckEventQueue;

static void glfwhckEventQueueWindowPosCb(GLFWwindow* window, int x, int y);
static void glfwhckEventQueueWindowSizeCb(GLFWwindow* window, int width, int height);
static void glfwhckEventQueueWindowCloseCb(GLFWwindow* window);
static void glfwhckEventQueueWindowRefreshCb(GLFWwindow* window);
static void glfwhckEventQueueWindowFocusCb(GLFWwindow* window, int focused);
static void glfwhckEventQueueWindowIconifyCb(GLFWwindow* window, int iconified);
static void glfwhckEventQueueFramebufferSizeCb(GLFWwindow* window, int width, int height);
static void glfwhckEventQueueMouseButtonCb(GLFWwindow* window, int button, int action, int mods);
static void glfwhckEventQueueMousePosCb(GLFWwindow* window, double x, double y);
static void glfwhckEventQueueMouseEnterCb(GLFWwindow* window, int entered);
static void glfwhckEventQueueMouseScrollCb(GLFWwindow* window, double xOffset, double yOffset);
static void glfwhckEventQueueKeyboardKeyCb(GLFWwindow* window, int key, int scancode, int action, int mods);
static void glfwhckEventQueueKeyboardCharCb(GLFWwindow* window, unsigned int codepoint);
static void glfwhckEventQueueSetWindowCallbacks(GLFWwindow* window, int const eventTypes);

glfwhckEventQueue* glfwhckEventQueueNew(GLFWwindow* window, int const eventTypes)
{
  glfwhckEventQueue* queue = calloc(1, sizeof(glfwhckEventQueue));
  queue->bufferSize = 0;
  queue->bufferCapacity = 32;
  queue->bufferPos = 0;
  queue->buffer = calloc(queue->bufferCapacity, sizeof(glfwhckEvent));

  glfwSetWindowUserPointer(window, queue);
  glfwhckEventQueueSetWindowCallbacks(window, eventTypes);

  return queue;
}

void glfwhckEventQueueFree(glfwhckEventQueue* queue)
{
  free(queue->buffer);
  free(queue);
}

void glfwhckEventQueuePush(glfwhckEventQueue* queue, glfwhckEvent const* event)
{
  if(queue->bufferSize >= queue->bufferCapacity)
  {
    unsigned int const oldCapacity = queue->bufferCapacity;
    unsigned int const oldPos = queue->bufferPos;
    glfwhckEvent const* oldBuffer = queue->buffer;

    queue->bufferCapacity = 2 * queue->bufferCapacity;
    queue->buffer = calloc(queue->bufferCapacity, sizeof(glfwhckEvent));
    queue->bufferPos = 0;

    memcpy(queue->buffer, oldBuffer + oldPos * sizeof(glfwhckEvent),
           (oldCapacity - oldPos) * sizeof(glfwhckEvent));

    if (oldPos != 0)
    {
      memcpy(queue->buffer + oldPos * sizeof(glfwhckEvent), oldBuffer,
             oldPos * sizeof(glfwhckEvent));
    }
  }

  unsigned int const index = (queue->bufferPos + queue->bufferSize) % queue->bufferCapacity;
  queue->buffer[index] = *event;
  queue->bufferSize += 1;
}

glfwhckEvent const* glfwhckEventQueuePop(glfwhckEventQueue* queue)
{
  unsigned int const oldPos = queue->bufferPos;
  queue->bufferPos = (queue->bufferPos + 1) % queue->bufferCapacity;
  queue->bufferSize -= 1;
  return &queue->buffer[oldPos];
}

int glfwhckEventQueueEmpty(glfwhckEventQueue* queue)
{
  return queue->bufferSize == 0 ? GL_TRUE : GL_FALSE;
}

static void glfwhckEventQueueWindowPosCb(GLFWwindow* window, int x, int y)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_WINDOW_POSITION;
  event.windowPosition.x = x;
  event.windowPosition.y = y;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueWindowSizeCb(GLFWwindow* window, int width, int height)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_WINDOW_RESIZE;
  event.windowResize.width = width;
  event.windowResize.height = height;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueWindowCloseCb(GLFWwindow* window)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_WINDOW_CLOSE;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueWindowRefreshCb(GLFWwindow* window)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_WINDOW_REFRESH;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueWindowFocusCb(GLFWwindow* window, int focused)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_WINDOW_FOCUS;
  event.windowFocus.focused = focused;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueWindowIconifyCb(GLFWwindow* window, int iconified)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_WINDOW_ICONIFY;
  event.windowIconify.iconified = iconified;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueFramebufferSizeCb(GLFWwindow* window, int width, int height)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_FRAMEBUFFER_RESIZE;
  event.framebufferResize.width = width;
  event.framebufferResize.height = height;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueMouseButtonCb(GLFWwindow* window, int button, int action, int mods)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_MOUSE_BUTTON;
  event.mouseButton.button = button;
  event.mouseButton.action = action;
  event.mouseButton.mods = mods;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueMousePosCb(GLFWwindow* window, double x, double y)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_MOUSE_POSITION;
  event.mousePosition.x = x;
  event.mousePosition.y = y;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueMouseEnterCb(GLFWwindow* window, int entered)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_MOUSE_ENTER;
  event.mouseEnter.entered = entered;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueMouseScrollCb(GLFWwindow* window, double xOffset, double yOffset)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_MOUSE_SCROLL;
  event.mouseScroll.xOffset = xOffset;
  event.mouseScroll.yOffset = yOffset;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueKeyboardKeyCb(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_KEYBOARD_KEY;
  event.keyboardKey.key = key;
  event.keyboardKey.scancode = scancode;
  event.keyboardKey.action = action;
  event.keyboardKey.mods = mods;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueKeyboardCharCb(GLFWwindow* window, unsigned int codepoint)
{
  glfwhckEventQueue* queue = (glfwhckEventQueue*) glfwGetWindowUserPointer(window);
  glfwhckEvent event;
  event.type = GLFWHCK_EVENT_KEYBOARD_CHAR;
  event.keyboardChar.codepoint = codepoint;
  glfwhckEventQueuePush(queue, &event);
}

static void glfwhckEventQueueSetWindowCallbacks(GLFWwindow* window, int const eventTypes)
{
  if(eventTypes & GLFWHCK_EVENT_WINDOW_POSITION)
  {
    glfwSetWindowPosCallback(window, glfwhckEventQueueWindowPosCb);
  }
  if(eventTypes & GLFWHCK_EVENT_WINDOW_RESIZE)
  {
    glfwSetWindowSizeCallback(window, glfwhckEventQueueWindowSizeCb);
  }
  if(eventTypes & GLFWHCK_EVENT_WINDOW_CLOSE)
  {
    glfwSetWindowCloseCallback(window, glfwhckEventQueueWindowCloseCb);
  }
  if(eventTypes & GLFWHCK_EVENT_WINDOW_REFRESH)
  {
    glfwSetWindowRefreshCallback(window, glfwhckEventQueueWindowRefreshCb);
  }
  if(eventTypes & GLFWHCK_EVENT_WINDOW_FOCUS)
  {
    glfwSetWindowFocusCallback(window, glfwhckEventQueueWindowFocusCb);
  }
  if(eventTypes & GLFWHCK_EVENT_WINDOW_ICONIFY)
  {
    glfwSetWindowIconifyCallback(window, glfwhckEventQueueWindowIconifyCb);
  }
  if(eventTypes & GLFWHCK_EVENT_FRAMEBUFFER_RESIZE)
  {
    glfwSetFramebufferSizeCallback(window, glfwhckEventQueueFramebufferSizeCb);
  }
  if(eventTypes & GLFWHCK_EVENT_MOUSE_BUTTON)
  {
    glfwSetMouseButtonCallback(window, glfwhckEventQueueMouseButtonCb);
  }
  if(eventTypes & GLFWHCK_EVENT_MOUSE_POSITION)
  {
    glfwSetCursorPosCallback(window, glfwhckEventQueueMousePosCb);
  }
  if(eventTypes & GLFWHCK_EVENT_MOUSE_ENTER)
  {
    glfwSetCursorEnterCallback(window, glfwhckEventQueueMouseEnterCb);
  }
  if(eventTypes & GLFWHCK_EVENT_MOUSE_SCROLL)
  {
    glfwSetScrollCallback(window, glfwhckEventQueueMouseScrollCb);
  }
  if(eventTypes & GLFWHCK_EVENT_KEYBOARD_KEY)
  {
    glfwSetKeyCallback(window, glfwhckEventQueueKeyboardKeyCb);
  }
  if(eventTypes & GLFWHCK_EVENT_KEYBOARD_CHAR)
  {
    glfwSetCharCallback(window, glfwhckEventQueueKeyboardCharCb);
  }
}
