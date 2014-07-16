
#include "sdl-application.h"
#include <cassert>

#include <dali/public-api/dali-core.h>

#ifdef EMSCRIPTEN
#include "emscripten/emscripten.h"
#endif

#include <SDL/SDL.h>


// see
// ~/installs/emscripten/emscripten/system/include/emscripten/emscripten.h
extern void emscripten_set_main_loop(void (*func)(), int fps, int simulate_infinite_loop);

Dali::SdlApplication *app = NULL;

void EmscriptenMouseEvent(double x, double y, int downUpMotion)
{
  DALI_ASSERT_ALWAYS(app);

  if(app)
  {
    app->SendTouchEvent(x,y, downUpMotion);
  }
}

void ProcessEvents()
{
  SDL_PumpEvents();

  //
  // This wasnt working prior to emscripten v1.25
  //
  // But it only gives event inside the gl window.
  // When dragging (for rotation etc) I think we want the drag/rotate to continue outside the window
  //
  // So we'll disable this handling for now
  //
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    // switch(event.type)
    // {
    //   case SDL_NOEVENT:
    //   {
    //     // this gets called way too much????
    //     printf("no event\n");
    //     break;
    //   }
    //   case SDL_MOUSEMOTION:
    //   {
    //     SDL_MouseMotionEvent *m = (SDL_MouseMotionEvent*)&event;
    //     // assert(m->state == 0);
    //     int x, y;
    //     SDL_GetMouseState(&x, &y);
    //     assert(x == m->x && y == m->y);
    //     printf("motion: %d,%d  %d,%d state:%d\n", m->x, m->y, m->xrel, m->yrel, m->state);
    //     break;
    //   }
    //   case SDL_MOUSEBUTTONDOWN:
    //   {
    //     SDL_MouseButtonEvent *m = (SDL_MouseButtonEvent*)&event;
    //     printf("button down: %d,%d  %d,%d\n", m->button, m->state, m->x, m->y);
    //     break;
    //   }
    //   case SDL_MOUSEBUTTONUP:
    //   {
    //     SDL_MouseButtonEvent *m = (SDL_MouseButtonEvent*)&event;
    //     printf("button up: %d,%d  %d,%d\n", m->button, m->state, m->x, m->y);
    //     // Remove another click we want to ignore
    //     // assert(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONDOWN) == 1);
    //     // assert(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_MOUSEBUTTONUP, SDL_MOUSEBUTTONUP) == 1);
    //     break;
    //   }
    //   //
    //   // keys
    //   //
    //   case SDL_KEYDOWN:
    //   {
    //     break;
    //   }
    //   case SDL_KEYUP:
    //   {
    //     // don't handle the modifier key events
    //     if (event.key.keysym.sym == SDLK_LCTRL ||
    //         event.key.keysym.sym == SDLK_LSHIFT ||
    //         event.key.keysym.sym == SDLK_LALT)
    //     {
    //       return;
    //     }
    //     if ((event.key.keysym.mod & KMOD_LCTRL) || (event.key.keysym.mod & KMOD_RCTRL))
    //     {
    //       printf(":ctrl down:");
    //     }
    //     if ((event.key.keysym.mod & KMOD_LSHIFT) || (event.key.keysym.mod & KMOD_RSHIFT))
    //     {
    //       printf(":shift down:");
    //     }
    //     if ((event.key.keysym.mod & KMOD_LALT) || (event.key.keysym.mod & KMOD_RALT))
    //     {
    //       printf(":alt down:");
    //     }

    //     switch (event.key.keysym.sym)
    //     {
    //       case SDLK_RIGHT: printf("right\n"); break;
    //       case SDLK_LEFT: printf("left\n"); break;
    //       case SDLK_DOWN: printf("down\n"); break;
    //       case SDLK_UP: printf("up\n"); break;
    //       case SDLK_a: printf("a\n"); break;
    //       default:
    //       {
    //         // if (event.key.keysym.scancode == SDL_SCANCODE_B) {
    //         //   printf("b scancode\n"); break;
    //         // }
    //         printf("unknown key: sym %d scancode %d\n", event.key.keysym.sym, event.key.keysym.scancode);
    //         //emscripten_run_script("throw 'done'"); // comment this out to leave event handling active. Use the following to log DOM keys:
    //                                                // addEventListener('keyup', function(event) { console.log(event.keyCode) }, true)
    //       }
    //     }
    //     break;
    //   }
    //   default: /* Report an unhandled event */
    //   {
    //     // lots of these at the moment?????
    //     // printf("I don't know what this event is!%d\n", event.type);
    //   }
    // } // switch(event)
  } // Poll(event)

} // ProcessEvents()


void EmscriptenRenderOnce()
{
  DALI_ASSERT_ALWAYS(app);

  static int w = 0;
  static int h = 0;

  int _x = EM_ASM_INT_V({ return dali.canvas.width; });
  int _y = EM_ASM_INT_V({ return dali.canvas.height; });

  bool resize = false;
  if( _x != w )
  {
    w = _x;
    resize = true;
  }
  if( _y != h )
  {
    h = _y;
    resize = true;
  }
  if( resize )
  {
    app->SetSurfaceWidth(w, h);
  }

  ProcessEvents();

  app->DoUpdate( );

  app->DoRender();

}

int main(int argc, char *argv[])
{
  using namespace Dali;

  // need to reference everything as emscripten/llvm will cut it all out.
  Dali::ImageActor ia;
  Dali::TextActor ta;

  app = new SdlApplication( 400, 400, SdlApplication::DEFAULT_HORIZONTAL_DPI,  SdlApplication::DEFAULT_VERTICAL_DPI );

#ifdef EMSCRIPTEN

  emscripten_set_main_loop(EmscriptenRenderOnce, 0, 1);

#else

  app->MainLoop();

#endif

  return 1;
}
