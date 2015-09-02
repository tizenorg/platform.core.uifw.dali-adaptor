#ifdef WAYLAND_DEBUG

// HEADER
#include "wayland-performance-markers.h"

#include <dlfcn.h>
#include <stdio.h>

/**
 * Global performance interface and performance contexts for flush and dispatch queue.
 */
Dali::Internal::Adaptor::PerformanceInterface* gPerformanceInterface = NULL;
Dali::Internal::Adaptor::PerformanceInterface::ContextId gWaylandlFlushContext = 0;
Dali::Internal::Adaptor::PerformanceInterface::ContextId gWaylandlDispatch = 0;
Dali::Internal::Adaptor::PerformanceInterface::ContextId gWaylandlDispatchQueue = 0;


typedef int (*WaylandFlushFunc)(struct wl_display *display);
typedef int (*WaylandDisplayDispatchFunc)(struct wl_display *display);
typedef int (*WaylandDisplayDispatchQueueFunc)(struct wl_display *display, wl_event_queue *queue);

void SetPerformanceInterface( Dali::Internal::Adaptor::PerformanceInterface* performanceInterface )
{
  gPerformanceInterface = performanceInterface;
  gWaylandlFlushContext = gPerformanceInterface->AddContext("WL_FLUSH");
  gWaylandlDispatch = gPerformanceInterface->AddContext("WL_DISPATCH");
  gWaylandlDispatchQueue = gPerformanceInterface->AddContext("WL_DISPATCH_QUEUE");
}

// ensure  export LD_PRELOAD=/usr/lib/libdali-adaptor.so is set for these to work
int wl_display_flush(struct wl_display *display)
{
  static WaylandFlushFunc funcPtr = NULL;
  if( funcPtr == NULL)
  {
    funcPtr =(WaylandFlushFunc) dlsym(RTLD_NEXT, "wl_display_flush");
  }

  if( gPerformanceInterface )
  {
    gPerformanceInterface->AddMarker( Dali::Internal::Adaptor::PerformanceInterface::START, gWaylandlFlushContext );
  }

  int ret = funcPtr( display );

  if( gPerformanceInterface )
  {
    gPerformanceInterface->AddMarker( Dali::Internal::Adaptor::PerformanceInterface::END, gWaylandlFlushContext );
  }
  return ret;

}

int wl_display_dispatch(struct wl_display *display)
{
  static WaylandDisplayDispatchFunc funcPtr = NULL;
  if( funcPtr == NULL)
  {
    funcPtr =(WaylandDisplayDispatchFunc) dlsym(RTLD_NEXT, "wl_display_dispatch");
  }
  if( gPerformanceInterface )
  {
    gPerformanceInterface->AddMarker( Dali::Internal::Adaptor::PerformanceInterface::START, gWaylandlDispatch );
  }
  int ret = funcPtr( display );

  if( gPerformanceInterface )
  {
    gPerformanceInterface->AddMarker( Dali::Internal::Adaptor::PerformanceInterface::END, gWaylandlDispatch );
  }
  return ret;
}

int wl_display_dispatch_queue(struct wl_display *display, struct wl_event_queue *queue)
{
  static WaylandDisplayDispatchQueueFunc funcPtr = NULL;
  if( funcPtr == NULL)
  {
    funcPtr =(WaylandDisplayDispatchQueueFunc) dlsym(RTLD_NEXT, "wl_display_dispatch_queue");
  }
  if( gPerformanceInterface )
  {
    gPerformanceInterface->AddMarker( Dali::Internal::Adaptor::PerformanceInterface::START, gWaylandlDispatchQueue );
  }
  int ret = funcPtr( display, queue );

  if( gPerformanceInterface )
  {
    gPerformanceInterface->AddMarker( Dali::Internal::Adaptor::PerformanceInterface::END, gWaylandlDispatchQueue );
  }
  return ret;
}
#endif
