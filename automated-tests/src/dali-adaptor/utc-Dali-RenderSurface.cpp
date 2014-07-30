//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <utilX.h>
#include <map>
#include <string.h>
#include <iostream>

// CLASS HEADER
#include <stdlib.h>
#include <iostream>
#include <dali.h>
#include <dali-test-suite-utils.h>
#include <Ecore_X.h>

using namespace Dali;

void utc_dali_render_surface_startup(void)
{
    ecore_x_init(NULL);
    test_return_value = TET_UNDEF;
}

void utc_dali_render_surface_cleanup(void)
{
    ecore_x_shutdown();
    test_return_value = TET_PASS;
}

/**
 * @brief creating RenderSurface::PIXMAP type render surface.
 * Using CreateDefaultSurface() factory method instead of constructor.
 *
 * @procedure
 * @step 1 creating RenderSurface::PIXMAP type render surface.
 * @step 2 check is surface successfully created.
 *
 * @Passcondition: if returned pointer is not NULL.
 * @id DA-112
 */
int UtcDaliRenderSurfaceRenderSurface_p_01(void)
{
    tet_printf("RenderSurface::RenderSurface positive test 1\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, posSize, "Surface1");
    DALI_TEST_CHECK(pSurface);
    delete pSurface;

    END_TEST;
}

/**
 * @brief creating RenderSurface::WINDOW type render surface.
 * Using CreateDefaultSurface() factory method instead of constructor.
 *
 * @procedure
 * @step 1 creating RenderSurface::WINDOW type render surface.
 * @step 2 check is surface successfully created.
 *
 * @Passcondition: if returned pointer is not NULL.
 * @id DA-112
 */
int UtcDaliRenderSurfaceRenderSurface_p_02(void)
{
    tet_printf("RenderSurface::RenderSurface positive test 2\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::WINDOW, posSize, "Surface2");
    DALI_TEST_CHECK(pSurface);
    delete pSurface;

    END_TEST;
}

/**
 * @brief creating RenderSurface::NATIVE_BUFFER type render surface.
 * Using CreateDefaultSurface() factory method instead of constructor.
 *
 * @procedure
 * @step 1 creating RenderSurface::NATIVE_BUFFER type render surface.
 * @step 2 check is surface successfully created.
 *
 * @Passcondition: if returned pointer is not NULL.
 * @id DA-112
 */
int UtcDaliRenderSurfaceRenderSurface_p_03(void)
{
    tet_printf("RenderSurface::RenderSurface positive test 3\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::NATIVE_BUFFER, posSize, "Surface3");
    DALI_TEST_CHECK(pSurface);
    delete pSurface;

    END_TEST;
}

/**
 * @brief creating RenderSurface::NO_SURFACE type render surface.
 * Using CreateDefaultSurface() factory method instead of constructor.
 *
 * @procedure
 * @step 1 creating RenderSurface::NO_SURFACE type render surface.
 * @step 2 check is surface successfully created.
 *
 * @Passcondition: if returned pointer is not NULL.
 * @id DA-112
 */
int UtcDaliRenderSurfaceRenderSurface_p_04(void)
{
    tet_printf("RenderSurface::RenderSurface positive test 4\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::NO_SURFACE, posSize, "Surface4");
    DALI_TEST_CHECK(pSurface);
    delete pSurface;

    END_TEST;
}

/**
 * @brief checking type of render surface with RenderSurface::PIXMAP.
 *
 * @procedure
 * @step 1 creating RenderSurface::PIXMAP type render surface.
 * @step 2 get the surface type by calling GetType() method.
 *
 * @Passcondition: if returned type equals to the surface type.
 * @id DA-113
 */
int UtcDaliRenderSurfaceGetType_p_01(void)
{
    tet_printf("RenderSurface::GetType positive test case 1\n");

    RenderSurface::SurfaceType type;
    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, posSize, "Surface1");
    type = AnyCast<RenderSurface::SurfaceType>(pSurface->GetType());
    DALI_TEST_CHECK(type == RenderSurface::PIXMAP);
    delete pSurface;

    END_TEST;
}

/**
 * @brief checking type of render surface with RenderSurface::WINDOW.
 *
 * @procedure
 * @step 1 creating RenderSurface::WINDOW type render surface.
 * @step 2 get the surface type by calling GetType() method.
 *
 * @Passcondition: if returned type equals to the surface type.
 * @id DA-113
 */
int UtcDaliRenderSurfaceGetType_p_02(void)
{
    tet_printf("RenderSurface::GetType positive test case 2\n");

    RenderSurface::SurfaceType type;
    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::WINDOW, posSize, "Surface2");
    type = AnyCast<RenderSurface::SurfaceType>(pSurface->GetType());
    DALI_TEST_CHECK(type == RenderSurface::WINDOW);
    delete pSurface;

    END_TEST;
}

/**
 * @brief checking type of render surface with RenderSurface::NATIVE_BUFFER.
 *
 * @procedure
 * @step 1 creating RenderSurface::NATIVE_BUFFER type render surface.
 * @step 2 get the surface type by calling GetType() method.
 *
 * @Passcondition: if returned type equals to the surface type.
 * @id DA-113
 */
int UtcDaliRenderSurfaceGetType_p_03(void)
{
    tet_printf("RenderSurface::GetType positive test case 3\n");

    RenderSurface::SurfaceType type;
    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::NATIVE_BUFFER, posSize, "Surface3");
    type = AnyCast<RenderSurface::SurfaceType>(pSurface->GetType());
    DALI_TEST_CHECK(type == RenderSurface::NATIVE_BUFFER);
    delete pSurface;

    END_TEST;
}

/**
 * @brief checking type of render surface with RenderSurface::NO_SURFACE.
 *
 * @procedure
 * @step 1 creating RenderSurface::NO_SURFACE type render surface.
 * @step 2 get the surface type by calling GetType() method.
 *
 * @Passcondition: if returned type equals to the surface type.
 * @id DA-113
 */
int UtcDaliRenderSurfaceGetType_p_04(void)
{
    tet_printf("RenderSurface::GetType positive test case 4\n");

    RenderSurface::SurfaceType type;
    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::NO_SURFACE, posSize, "Surface4");
    type = AnyCast<RenderSurface::SurfaceType>(pSurface->GetType());
    DALI_TEST_CHECK(type == RenderSurface::NO_SURFACE);
    delete pSurface;

    END_TEST;
}

/**
 * @brief get surface of RenderSurface.
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 get the surface type by calling GetSurface() method.
 *
 * @Passcondition: if returned Any value is not empty.
 * @id DA-114
 */
int UtcDaliRenderSurfaceGetSurface_p_01(void)
{
    tet_printf("RenderSurface::GetSurface positive test case 1\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, posSize, "Surface1");
    DALI_TEST_CHECK(!pSurface->GetSurface().Empty());
    delete pSurface;

    END_TEST;
}

/**
 * @brief get display of RenderSurface.
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 get display by calling GetDisplay() method.
 *
 * @Passcondition: if returned Any value is not empty.
 * @id DA-115
 */
int UtcDaliRenderSurfaceGetDisplay_p_01(void)
{
    tet_printf("RenderSurface::GetDisplay positive test case 1\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, posSize, "Surface1");
    DALI_TEST_CHECK(!pSurface->GetDisplay().Empty());
    delete pSurface;

    END_TEST;
}

/**
 * @brief creating RenderSurface and get its position size.
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 get position size by calling GetPositionSize() method.
 *
 * @Passcondition: if getted position size equals to setted.
 * @id DA-116
 */
int UtcDaliRenderSurfaceGetPositionSize_p_01(void)
{
    tet_printf("RenderSurface::GetPositionSize positive test case 1\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, PositionSize(10, 10, 10, 10), "Surface1");
    PositionSize gettedPosSize = pSurface->GetPositionSize();
    DALI_TEST_CHECK(posSize == gettedPosSize);

    delete pSurface;

    END_TEST;
}

/**
 * @brief getting render mode of RenderSurface(RENDER_DEFAULT case).
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 set render mode RenderSurface::RENDER_DEFAULT to the surface.
 * @step 3 get render mode of the surface.
 *
 * @Passcondition: if get value equals to set value.
 * @id DA-117
 */
int UtcDaliRenderSurfaceGetRenderMode_p_01(void)
{
    tet_printf("RenderSurface::GetRenderMode positive test case 1\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, PositionSize(10, 10, 10, 10), "Surface1");
    RenderSurface::RenderMode renderMode = RenderSurface::RENDER_DEFAULT;
    pSurface->SetRenderMode(renderMode);
    DALI_TEST_CHECK(renderMode == pSurface->GetRenderMode());
    delete pSurface;

    END_TEST;
}

/**
 * @brief getting render mode of RenderSurface(RENDER_SYNC case).
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 set render mode RenderSurface::RENDER_SYNC to the surface.
 * @step 3 get render mode of the surface.
 *
 * @Passcondition: if get value equals to set value.
 * @id DA-117
 */
int UtcDaliRenderSurfaceGetRenderMode_p_02(void)
{
    tet_printf("RenderSurface::GetRenderMode positive test case 2\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, PositionSize(10, 10, 10, 10), "Surface1");
    RenderSurface::RenderMode renderMode = RenderSurface::RENDER_SYNC;
    pSurface->SetRenderMode(renderMode);
    DALI_TEST_CHECK(renderMode == pSurface->GetRenderMode());
    delete pSurface;

    END_TEST;
}


/**
 * @brief getting render mode of RenderSurface(RENDER_24FPS case).
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 set render mode RenderSurface::RENDER_24FPS to the surface.
 * @step 3 get render mode of the surface.
 *
 * @Passcondition: if get value equals to set value.
 * @id DA-117
 */
int UtcDaliRenderSurfaceGetRenderMode_p_03(void)
{
    tet_printf("RenderSurface::GetRenderMode positive test case 3\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, PositionSize(10, 10, 10, 10), "Surface1");
    RenderSurface::RenderMode renderMode = RenderSurface::RENDER_24FPS;
    pSurface->SetRenderMode(renderMode);
    DALI_TEST_CHECK(renderMode == pSurface->GetRenderMode());
    delete pSurface;

    END_TEST;
}


/**
 * @brief getting render mode of RenderSurface(RENDER_30FPS case).
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 set render mode RenderSurface::RENDER_30FPS to the surface.
 * @step 3 get render mode of the surface.
 *
 * @Passcondition: if get value equals to set value.
 * @id DA-117
 */
int UtcDaliRenderSurfaceGetRenderMode_p_04(void)
{
    tet_printf("RenderSurface::GetRenderMode positive test case 4\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, PositionSize(10, 10, 10, 10), "Surface1");
    RenderSurface::RenderMode renderMode = RenderSurface::RENDER_30FPS;
    pSurface->SetRenderMode(renderMode);
    DALI_TEST_CHECK(renderMode == pSurface->GetRenderMode());
    delete pSurface;

    END_TEST;
}


/**
 * @brief getting render mode of RenderSurface(RENDER_60FPS case).
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 set render mode RenderSurface::RENDER_60FPS to the surface.
 * @step 3 get render mode of the surface.
 *
 * @Passcondition: if get value equals to set value.
 * @id DA-117
 */
int UtcDaliRenderSurfaceGetRenderMode_p_05(void)
{
    tet_printf("RenderSurface::GetRenderMode positive test case 5\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, PositionSize(10, 10, 10, 10), "Surface1");
    RenderSurface::RenderMode renderMode = RenderSurface::RENDER_60FPS;
    pSurface->SetRenderMode(renderMode);
    DALI_TEST_CHECK(renderMode == pSurface->GetRenderMode());
    delete pSurface;

    END_TEST;
}

/**
 * @brief setting render mode of RenderSurface(RENDER_DEFAULT case).
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 set render mode RenderSurface::RENDER_DEFAULT to the surface.
 * @step 3 get render mode of the surface.
 *
 * @Passcondition: if get value equals to set value.
 * @id DA-118
 */
int UtcDaliRenderSurfaceSetRenderMode_p_01(void)
{
    tet_printf("RenderSurface::SetRenderMode positive test case 1\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, PositionSize(10, 10, 10, 10), "Surface1");
    RenderSurface::RenderMode renderMode = RenderSurface::RENDER_DEFAULT;
    pSurface->SetRenderMode(renderMode);
    DALI_TEST_CHECK(renderMode == pSurface->GetRenderMode());

    delete pSurface;

    END_TEST;
}

/**
 * @brief setting render mode of RenderSurface(RENDER_SYNC case).
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 set render mode RenderSurface::RENDER_SYNC to the surface.
 * @step 3 get render mode of the surface.
 *
 * @Passcondition: if get value equals to set value.
 * @id DA-118
 */
int UtcDaliRenderSurfaceSetRenderMode_p_02(void)
{
    tet_printf("RenderSurface::SetRenderMode positive test case 2\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, PositionSize(10, 10, 10, 10), "Surface1");
    RenderSurface::RenderMode renderMode = RenderSurface::RENDER_SYNC;
    pSurface->SetRenderMode(renderMode);
    DALI_TEST_CHECK(renderMode == pSurface->GetRenderMode());
    delete pSurface;

    END_TEST;
}

/**
 * @brief setting render mode of RenderSurface(RENDER_24FPS case).
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 set render mode RenderSurface::RENDER_24FPS to the surface.
 * @step 3 get render mode of the surface.
 *
 * @Passcondition: if get value equals to set value.
 * @id DA-118
 */
int UtcDaliRenderSurfaceSetRenderMode_p_03(void)
{
    tet_printf("RenderSurface::SetRenderMode positive test case 3\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, PositionSize(10, 10, 10, 10), "Surface1");
    RenderSurface::RenderMode renderMode = RenderSurface::RENDER_24FPS;
    pSurface->SetRenderMode(renderMode);
    DALI_TEST_CHECK(renderMode == pSurface->GetRenderMode());
    delete pSurface;

    END_TEST;
}

/**
 * @brief setting render mode of RenderSurface(RENDER_30FPS case).
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 set render mode RenderSurface::RENDER_30FPS to the surface.
 * @step 3 get render mode of the surface.
 *
 * @Passcondition: if get value equals to set value.
 * @id DA-118
 */
int UtcDaliRenderSurfaceSetRenderMode_p_04(void)
{
    tet_printf("RenderSurface::SetRenderMode positive test case 4\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, PositionSize(10, 10, 10, 10), "Surface1");
    RenderSurface::RenderMode renderMode = RenderSurface::RENDER_30FPS;
    pSurface->SetRenderMode(renderMode);
    DALI_TEST_CHECK(renderMode == pSurface->GetRenderMode());
    delete pSurface;

    END_TEST;
}

/**
 * @brief setting render mode of RenderSurface(RENDER_60FPS case).
 *
 * @procedure
 * @step 1 creating render surface.
 * @step 2 set render mode RenderSurface::RENDER_60FPS to the surface.
 * @step 3 get render mode of the surface.
 *
 * @Passcondition: if get value equals to set value.
 * @id DA-118
 */
int UtcDaliRenderSurfaceSetRenderMode_p_05(void)
{
    tet_printf("RenderSurface::SetRenderMode positive test case 5\n");

    PositionSize posSize(10, 10, 10, 10);
    RenderSurface* pSurface = CreateDefaultSurface(RenderSurface::PIXMAP, PositionSize(10, 10, 10, 10), "Surface1");
    RenderSurface::RenderMode renderMode = RenderSurface::RENDER_60FPS;
    pSurface->SetRenderMode(renderMode);
    DALI_TEST_CHECK(renderMode == pSurface->GetRenderMode());

    delete pSurface;

    END_TEST;
}
