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

glfwhckEventQueue* glfwhckEventQueueNew(GLFWwindow* window, int const eventTypes)
{
  glfwhckEventQueue* queue = calloc(1, sizeof(glfwhckEventQueue));
  queue->bufferSize = 0;
  queue->bufferCapacity = 32;
  queue->bufferPos = 0;
  queue->buffer = calloc(queue->bufferCapacity, sizeof(glfwhckEvent));

  glfwSetWindowUserPointer(window, queue);

  if(eventTypes & GLFWHCK_EVENT_WINDOW_POSITION)
  {
    glfwSetWindowPosCallback(window, glfwhckEventQueueWindowPosCb);
  }

  if(eventTypes & GLFWHCK_EVENT_WINDOW_RESIZE)
  {
    glfwSetWindowSizeCallback(window, glfwhckEventQueueWindowSizeCb);
  }

  return queue;
}

void glfwhckEventQueueFree(glfwhckEventQueue* queue)
{
  free(queue->buffer);
  free(queue);
}

void glfwhckEventQueuePush(glfwhckEventQueue* queue, glfwhckEvent const* event)
{
  if(queue->bufferSize == queue->bufferCapacity)
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

int const glfwhckEventQueueEmpty(glfwhckEventQueue* queue)
{
  return queue->bufferSize == 0 ? GL_TRUE : GL_FALSE;
}

