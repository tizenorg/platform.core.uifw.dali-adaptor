
#include "sdl-application.h"

#include <dali/public-api/dali-core.h>

#ifdef EMSCRIPTEN
#include "emscripten/emscripten.h"
#endif

// see
// ~/installs/emscripten/emscripten/system/include/emscripten/emscripten.h
extern void emscripten_set_main_loop(void (*func)(), int fps, int simulate_infinite_loop);

Dali::SdlApplication *app = NULL;

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
