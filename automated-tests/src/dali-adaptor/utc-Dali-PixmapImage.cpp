//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <X11/Xlib.h>
#include <iostream>
#include <stdlib.h>
#include <dali/dali.h>
#include <Ecore_X.h>
#include <dali-test-suite-utils.h>
#include <Ecore_X.h>


using namespace Dali;

static Dali::Window* window;
static Dali::Adaptor* adaptor;

void utc_dali_pixelimage_startup(void)
{
    ecore_x_init(NULL);
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);
    test_return_value = TET_UNDEF;
}

void utc_dali_pixelimage_cleanup(void)
{
    delete window;
    delete adaptor;
    ecore_x_shutdown();

    test_return_value = TET_PASS;
}

/**
 * @brief creating PixmapImage by using method PixmapImage::New with COLOR_DEPTH_DEFAULT
 *
 * @procedure
 * @step 1 creating PixmapImage with PixmapImage::COLOR_DEPTH_DEFAULT color depth
 * @step 2 check is pixmap image successfully created
 *
 * @Passcondition: test successed if pixmap image created.
 * @id DA-106
 */
int UtcDaliPixmapImageNew_p_01(void)
{
    tet_printf("PixmapImage::New positive test case 1\n");
    unsigned int width = 256;
    unsigned int height = 256;
    PixmapImage::ColorDepth depth = PixmapImage::COLOR_DEPTH_DEFAULT;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, depth, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    END_TEST;
}

/**
 * @brief creating PixmapImage by using method PixmapImage::New with COLOR_DEPTH_8
 *
 * @procedure
 * @step 1 creating PixmapImage with PixmapImage::COLOR_DEPTH_8 color depth
 * @step 2 check is pixmap image successfully created
 *
 * @Passcondition: test successed if pixmap image created.
 * @id DA-106
 */
int UtcDaliPixmapImageNew_p_02(void)
{
    tet_printf("PixmapImage::New positive test case 2\n");
    unsigned int width = 256;
    unsigned int height = 256;
    PixmapImage::ColorDepth depth = PixmapImage::COLOR_DEPTH_8;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, depth, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    END_TEST;
}

/**
 * @brief creating PixmapImage by using method PixmapImage::New with COLOR_DEPTH_16
 *
 * @procedure
 * @step 1 creating PixmapImage with PixmapImage::COLOR_DEPTH_16 color depth
 * @step 2 check is pixmap image successfully created
 *
 * @Passcondition: test successed if pixmap image created.
 * @id DA-106
 */
int UtcDaliPixmapImageNew_p_03(void)
{
    tet_printf("PixmapImage::New positive test case 3\n");
    unsigned int width = 256;
    unsigned int height = 256;
    PixmapImage::ColorDepth depth = PixmapImage::COLOR_DEPTH_16;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, depth, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    END_TEST;
}

/**
 * @brief creating PixmapImage by using method PixmapImage::New with COLOR_DEPTH_24
 *
 * @procedure
 * @step 1 creating PixmapImage with PixmapImage::COLOR_DEPTH_24 color depth
 * @step 2 check is pixmap image successfully created
 *
 * @Passcondition: test successed if pixmap image created.
 * @id DA-106
 */
int UtcDaliPixmapImageNew_p_04(void)
{
    tet_printf("PixmapImage::New positive test case 4\n");
    unsigned int width = 256;
    unsigned int height = 256;
    PixmapImage::ColorDepth depth = PixmapImage::COLOR_DEPTH_24;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, depth, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    END_TEST;
}

/**
 * @brief creating PixmapImage by using method PixmapImage::New with COLOR_DEPTH_32
 *
 * @procedure
 * @step 1 creating PixmapImage with PixmapImage::COLOR_DEPTH_32 color depth
 * @step 2 check is pixmap image successfully created
 *
 * @Passcondition: test successed if pixmap image created.
 * @id DA-106
 */
int UtcDaliPixmapImageNew_p_05(void)
{
    tet_printf("PixmapImage::New positive test case 5\n");
    unsigned int width = 256;
    unsigned int height = 256;
    PixmapImage::ColorDepth depth = PixmapImage::COLOR_DEPTH_32;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, depth, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    END_TEST;
}

/**
 * @brief creating PixmapImage by using method PixmapImage::New with zero width arg
 *
 * @procedure
 * @step 1 creating PixmapImage with zero width
 * @step 2 check is pixmap image successfully created
 *
 * @Passcondition: test successed if pixmap image not created.
 * @id DA-106
 */
int UtcDaliPixmapImageNew_n_01(void)
{
    tet_printf("PixmapImage::New negative test case 1\n");
    unsigned int height = 256;
    PixmapImage::ColorDepth depth = PixmapImage::COLOR_DEPTH_DEFAULT;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(0, height, depth, Adaptor::Get());
    DALI_TEST_CHECK(!pPixmapImg);

    END_TEST;
}

/**
 * @brief creating PixmapImage by using method PixmapImage::New with zero height arg
 *
 * @procedure
 * @step 1 creating PixmapImage with zero height
 * @step 2 check is pixmap image successfully created
 *
 * @Passcondition: test successed if pixmap image not created.
 * @id DA-106
 */
int UtcDaliPixmapImageNew_n_02(void)
{
    tet_printf("PixmapImage::New negative test case 2\n");
    unsigned int width = 256;
    PixmapImage::ColorDepth depth = PixmapImage::COLOR_DEPTH_DEFAULT;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, 0, depth, Adaptor::Get());
    DALI_TEST_CHECK(!pPixmapImg);

    END_TEST;
}

/**
 * @brief creating PixmapImage by using method PixmapImage::New with Ecore_X_Pixmap
 *
 * @procedure
 * @step 1 creating PixmapImage with Ecore_X_Pixmap
 * @step 2 check is pixmap image successfully created
 *
 * @Passcondition: test successed if pixmap image created.
 * @id DA-107
 */
int UtcDaliPixmapImageNew_p_06(void)
{
    tet_printf("PixmapImage::New positive test case 6\n");

    unsigned int width = 256;
    unsigned int height = 256;
    unsigned int depth = 16;
    Ecore_X_Pixmap pixmap = ecore_x_pixmap_new(NULL, width, height, depth);

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(pixmap, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    END_TEST;
}

/**
 * @brief creating PixmapImage by using method PixmapImage::New with X11 Pixmap
 *
 * @procedure
 * @step 1 creating PixmapImage with X11 pixmap
 * @step 2 check is pixmap image successfully created
 *
 * @Passcondition: test successed if pixmap image created.
 * @id DA-107
 */
int UtcDaliPixmapImageNew_p_07(void)
{
    tet_printf("PixmapImage::New positive test case 7\n");

    Display* dpy = XOpenDisplay(NULL);

    unsigned int width = 256;
    unsigned int height = 256;
    unsigned int depth = 16;

    Pixmap pixmap = XCreatePixmap(dpy, DefaultRootWindow(dpy), width, height, depth);

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(pixmap, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    END_TEST;
}

/**
 * @brief getting pixmap from PixmapImage created with Ecore_X_Pixmap
 *
 * @procedure
 * @step 1 creating PixmapImage with Ecore_X_Pixmap
 * @step 2 get X11 Pixmap of the PixmapImage
 * @step 3 get Ecore_X_Pixmap of the PixmapImage
 *
 * @Passcondition: returned Any pixmaps are not empty.
 * @id DA-108
 */
int UtcDaliPixmapImageGetPixmap_p_01(void)
{
    tet_printf("PixmapImage::GetPixmap positive test case 1\n");

    unsigned int width = 256;
    unsigned int height = 256;
    unsigned int depth = 16;

    Ecore_X_Pixmap pixmap = ecore_x_pixmap_new(NULL, width, height, depth);
    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(pixmap, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    Pixmap gettedX11Pixmap = AnyCast<Pixmap>(pPixmapImg->GetPixmap(PixmapImage::X11));
    DALI_TEST_CHECK(gettedX11Pixmap);

    Ecore_X_Pixmap gettedEcorePixmap = AnyCast<Ecore_X_Pixmap>(pPixmapImg->GetPixmap(PixmapImage::ECORE_X11));
    DALI_TEST_CHECK(gettedEcorePixmap);

    END_TEST;
}

/**
 * @brief getting pixmap from PixmapImage created with X11 Pixmap
 *
 * @procedure
 * @step 1 creating PixmapImage with X11 Pixmap
 * @step 2 get X11 Pixmap of the PixmapImage
 * @step 3 get Ecore_X_Pixmap of the PixmapImage
 *
 * @Passcondition: returned Any pixmaps are not empty.
 * @id DA-108
 */
int UtcDaliPixmapImageGetPixmap_p_02(void)
{
    tet_printf("PixmapImage::GetPixmap positive test case 2\n");

    Display* dpy = XOpenDisplay(NULL);
    unsigned int width = 256;
    unsigned int height = 256;
    unsigned int depth = 16;

    Pixmap pixmap = XCreatePixmap(dpy, DefaultRootWindow(dpy), width, height, depth);
    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(pixmap, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    Pixmap gettedX11Pixmap = AnyCast<Pixmap>(pPixmapImg->GetPixmap(PixmapImage::X11));
    DALI_TEST_CHECK(gettedX11Pixmap);

    Ecore_X_Pixmap gettedEcorePixmap = AnyCast<Ecore_X_Pixmap>(pPixmapImg->GetPixmap(PixmapImage::ECORE_X11));
    DALI_TEST_CHECK(gettedEcorePixmap);

    END_TEST;
}

/**
 * @brief getting display from PixmapImage created with X11 Pixmap
 *
 * @procedure
 * @step 1 creating PixmapImage with X11 Pixmap
 * @step 2 get display of the PixmapImage
 *
 * @Passcondition: returned Any display is not empty.
 * @id DA-109
 */
int UtcDaliPixmapImageGetDisplay_p_01(void)
{
    tet_printf("PixmapImage::GetDisplay positive test case 1\n");

    unsigned int width = 256;
    unsigned int height = 256;
    unsigned int depth = 16;

    Ecore_X_Pixmap pixmap = ecore_x_pixmap_new(NULL, width, height, depth);
    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(pixmap, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    Any display = pPixmapImg->GetDisplay();

    DALI_TEST_CHECK(!display.Empty());

    END_TEST;
}

/**
 * @brief getting display from PixmapImage created with Ecore_X_Pixmap
 *
 * @procedure
 * @step 1 creating PixmapImage with Ecore_X_Pixmap
 * @step 2 get display of the PixmapImage
 *
 * @Passcondition: returned Any display is not empty.
 * @id DA-109
 */
int UtcDaliPixmapImageGetDisplay_p_02(void)
{
    tet_printf("PixmapImage::GetDisplay positive test case 2\n");

    Display* dpy = XOpenDisplay(NULL);
    unsigned int width = 256;
    unsigned int height = 256;
    unsigned int depth = 16;

    Pixmap pixmap = XCreatePixmap(dpy, DefaultRootWindow(dpy), width, height, depth);
    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(pixmap, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    Any display = pPixmapImg->GetDisplay();

    DALI_TEST_CHECK(!display.Empty());

    END_TEST;
}

/**
 * @brief calling GetPixels() method of PixmapImage created with PixmapImage::COLOR_DEPTH_32 depth
 *
 * @procedure
 * @step 1 creating PixmapImage with PixmapImage::COLOR_DEPTH_32 depth
 * @step 2 check is PixmapImage successfully created
 * @step 3 getting pixels
 *
 * @Passcondition: the method return true, returned pixel's properties are equal to properties
 * passed to PixmapImage constructor and pixels format is RGBA8888.
 * @id DA-110
 */
int UtcDaliPixmapImageGetPixels_p_01(void)
{
    tet_printf("PixmapImage::GetPixels positive test case 1\n");
    unsigned int width = 256;
    unsigned int height = 512;
    PixmapImage::ColorDepth depth = PixmapImage::COLOR_DEPTH_32;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, depth, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    unsigned int pixelsWidth = 0;
    unsigned int pixelsHeight = 0;
    Pixel::Format pixelsFormat = Pixel::A8;
    std::vector<unsigned char> pixbuf;
    DALI_TEST_CHECK(pPixmapImg->GetPixels(pixbuf, pixelsWidth, pixelsHeight, pixelsFormat));
    DALI_TEST_CHECK(pixbuf.size() == width*height*4);
    DALI_TEST_CHECK(pixelsWidth == width);
    DALI_TEST_CHECK(pixelsHeight == height);
    DALI_TEST_CHECK(pixelsFormat == Pixel::RGBA8888);

    END_TEST;
}

/**
 * @brief calling GetPixels() method of PixmapImage created with PixmapImage::COLOR_DEPTH_24 depth
 *
 * @procedure
 * @step 1 creating PixmapImage with PixmapImage::COLOR_DEPTH_24 depth
 * @step 2 check is PixmapImage successfully created
 * @step 3 getting pixels
 *
 * @Passcondition: the method return true, returned pixel's properties are equal to properties
 * passed to PixmapImage constructor and pixels format is RGB888.
 * @id DA-110
 */
int UtcDaliPixmapImageGetPixels_p_02(void)
{
    tet_printf("PixmapImage::GetPixels positive test case 2\n");
    unsigned int width = 256;
    unsigned int height = 512;
    PixmapImage::ColorDepth depth = PixmapImage::COLOR_DEPTH_24;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, depth, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    unsigned int pixelsWidth = 0;
    unsigned int pixelsHeight = 0;
    Pixel::Format pixelsFormat = Pixel::A8;
    std::vector<unsigned char> pixbuf;
    DALI_TEST_CHECK(pPixmapImg->GetPixels(pixbuf, pixelsWidth, pixelsHeight, pixelsFormat));
    DALI_TEST_CHECK(pixbuf.size() == width*height*3);
    DALI_TEST_CHECK(pixelsWidth == width);
    DALI_TEST_CHECK(pixelsHeight == height);
    DALI_TEST_CHECK(pixelsFormat == Pixel::RGB888);

    END_TEST;
}

/**
 * @brief calling GetPixels() of PixmapImage created with unsupported
 * for this method PixmapImage::COLOR_DEPTH_16 depth
 *
 * @procedure
 * @step 1 creating PixmapImage with PixmapImage::COLOR_DEPTH_16 depth
 * @step 2 check is PixmapImage successfully created
 * @step 3 getting pixels
 *
 * @Passcondition: the method return false and method didn't change input.
 * @id DA-110
 */
int UtcDaliPixmapImageGetPixels_n_01(void)
{
    tet_printf("PixmapImage::GetPixels negative test case 1\n");
    unsigned int width = 256;
    unsigned int height = 512;
    PixmapImage::ColorDepth depth = PixmapImage::COLOR_DEPTH_16;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, depth, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    unsigned int pixelsWidth = 0;
    unsigned int pixelsHeight = 0;
    Pixel::Format pixelsFormat = Pixel::A8;
    std::vector<unsigned char> pixbuf;
    DALI_TEST_CHECK(!pPixmapImg->GetPixels(pixbuf, pixelsWidth, pixelsHeight, pixelsFormat));
    DALI_TEST_CHECK(pixbuf.size() == 0);
    DALI_TEST_CHECK(pixelsWidth == 0);
    DALI_TEST_CHECK(pixelsHeight == 0);
    DALI_TEST_CHECK(pixelsFormat == Pixel::A8);

    END_TEST;
}

/**
 * @brief calling GetPixels() of PixmapImage created with unsupported
 * for this method PixmapImage::COLOR_DEPTH_8 depth
 *
 * @procedure
 * @step 1 creating PixmapImage with PixmapImage::COLOR_DEPTH_8 depth
 * @step 2 check is PixmapImage successfully created
 * @step 3 getting pixels
 *
 * @Passcondition: the method return false and method didn't change input.
 * @id DA-110
 */
int UtcDaliPixmapImageGetPixels_n_02(void)
{
    tet_printf("PixmapImage::GetPixels negative test case 2\n");
    unsigned int width = 256;
    unsigned int height = 512;
    PixmapImage::ColorDepth depth = PixmapImage::COLOR_DEPTH_8;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, depth, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    unsigned int pixelsWidth = 0;
    unsigned int pixelsHeight = 0;
    Pixel::Format pixelsFormat = Pixel::A8;
    std::vector<unsigned char> pixbuf;
    DALI_TEST_CHECK(!pPixmapImg->GetPixels(pixbuf, pixelsWidth, pixelsHeight, pixelsFormat));
    DALI_TEST_CHECK(pixbuf.size() == 0);
    DALI_TEST_CHECK(pixelsWidth == 0);
    DALI_TEST_CHECK(pixelsHeight == 0);
    DALI_TEST_CHECK(pixelsFormat == Pixel::A8);

    END_TEST;
}

/**
 * @brief encoding PixmapImage into *.jpg
 *
 * @procedure
 * @step 1 creating PixmapImage
 * @step 2 check is the PixmapImage successfully created
 * @step 3 encoding the PixmapImage into *.jpg
 *
 * @Passcondition: if the method return true.
 * @id DA-111
 */
int UtcDaliPixmapImageEncodeToFile_p_01(void)
{
    tet_printf("PixmapImage::EncodeToFile positive test case 1\n");
    unsigned int width = 256;
    unsigned int height = 512;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, PixmapImage::COLOR_DEPTH_DEFAULT, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    DALI_TEST_CHECK(pPixmapImg->EncodeToFile("file.jpg"));

    END_TEST;
}

/**
 * @brief encoding PixmapImage into *.jpeg
 *
 * @procedure
 * @step 1 creating PixmapImage
 * @step 2 check is the PixmapImage successfully created
 * @step 3 encoding the PixmapImage into *.jpeg
 *
 * @Passcondition: if the method return true.
 * @id DA-111
 */
int UtcDaliPixmapImageEncodeToFile_p_02(void)
{
    tet_printf("PixmapImage::EncodeToFile positive test case 2\n");
    unsigned int width = 256;
    unsigned int height = 512;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, PixmapImage::COLOR_DEPTH_DEFAULT, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    DALI_TEST_CHECK(pPixmapImg->EncodeToFile("file.jpeg"));

    END_TEST;
}

/**
 * @brief encoding PixmapImage into *.png
 *
 * @procedure
 * @step 1 creating PixmapImage
 * @step 2 check is the PixmapImage successfully created
 * @step 3 encoding the PixmapImage into *.png
 *
 * @Passcondition: if the method return true.
 * @id DA-111
 */
int UtcDaliPixmapImageEncodeToFile_p_03(void)
{
    tet_printf("PixmapImage::EncodeToFile positive test case 3\n");
    unsigned int width = 256;
    unsigned int height = 512;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, PixmapImage::COLOR_DEPTH_DEFAULT, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    DALI_TEST_CHECK(pPixmapImg->EncodeToFile("file3.png"));

    END_TEST;
}

/**
 * @brief encoding PixmapImage into file with unsupported format(*.doc)
 *
 * @procedure
 * @step 1 creating PixmapImage
 * @step 2 check is the PixmapImage successfully created
 * @step 3 encoding the PixmapImage into *.doc
 *
 * @Passcondition: if the method return false.
 * @id DA-111
 */
int UtcDaliPixmapImageEncodeToFile_n_01(void)
{
    tet_printf("PixmapImage::EncodeToFile negative test case 1\n");
    unsigned int width = 256;
    unsigned int height = 512;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, PixmapImage::COLOR_DEPTH_DEFAULT, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    DALI_TEST_CHECK(!pPixmapImg->EncodeToFile("file.doc"));

    END_TEST;
}

/**
 * @brief calling the method with empty string
 *
 * @procedure
 * @step 1 creating PixmapImage
 * @step 2 check is the PixmapImage successfully created
 * @step 3 encoding the PixmapImage to file with incorrect name("")
 *
 * @Passcondition: if the method return false.
 * @id DA-111
 */
int UtcDaliPixmapImageEncodeToFile_n_02(void)
{
    tet_printf("PixmapImage::EncodeToFile negative test case 2\n");
    unsigned int width = 256;
    unsigned int height = 512;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, PixmapImage::COLOR_DEPTH_DEFAULT, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    DALI_TEST_CHECK(!pPixmapImg->EncodeToFile(""));

    END_TEST;
}

/**
 * @brief encoding PixmapImage into file without extension
 *
 * @procedure
 * @step 1 creating PixmapImage
 * @step 2 check is the PixmapImage successfully created
 * @step 3 encoding the PixmapImage to file with incorrect name("file")
 *
 * @Passcondition: if the method return false.
 * @id DA-111
 */
int UtcDaliPixmapImageEncodeToFile_n_03(void)
{
    tet_printf("PixmapImage::EncodeToFile negative test case 3\n");
    unsigned int width = 256;
    unsigned int height = 512;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, PixmapImage::COLOR_DEPTH_DEFAULT, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    DALI_TEST_CHECK(!pPixmapImg->EncodeToFile("file"));

    END_TEST;
}

/**
 * @brief encoding PixmapImage into file with empty extension
 *
 * @procedure
 * @step 1 creating PixmapImage
 * @step 2 check is the PixmapImage successfully created
 * @step 3 encoding the PixmapImage to file with incorrect name("file.")
 *
 * @Passcondition: if the method return false.
 * @id DA-111
 */
int UtcDaliPixmapImageEncodeToFile_n_04(void)
{
    tet_printf("PixmapImage::EncodeToFile negative test case 4\n");
    unsigned int width = 256;
    unsigned int height = 512;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, PixmapImage::COLOR_DEPTH_DEFAULT, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    DALI_TEST_CHECK(!pPixmapImg->EncodeToFile("file."));

    END_TEST;
}

/**
 * @brief encoding PixmapImage into file with name containing valid
 * extension string but with unsupported extension
 *
 * @procedure
 * @step 1 creating PixmapImage
 * @step 2 check is the PixmapImage successfully created
 * @step 3 encoding the PixmapImage to file with incorrect name("jpg.png.doc")
 *
 * @Passcondition: if the method return false.
 * @id DA-111
 */
int UtcDaliPixmapImageEncodeToFile_n_05(void)
{
    tet_printf("PixmapImage::EncodeToFile negative test case 5\n");
    unsigned int width = 256;
    unsigned int height = 512;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, PixmapImage::COLOR_DEPTH_DEFAULT, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    DALI_TEST_CHECK(!pPixmapImg->EncodeToFile("jpg.png.doc"));

    END_TEST;
}

/**
 * @brief encoding PixmapImage into file with name containing '/'
 *
 * @procedure
 * @step 1 creating PixmapImage
 * @step 2 check is the PixmapImage successfully created
 * @step 3 encoding the PixmapImage to file with incorrect name("fi/le.jpg")
 *
 * @Passcondition: if the method return false.
 * @id DA-111
 */
int UtcDaliPixmapImageEncodeToFile_n_06(void)
{
    tet_printf("PixmapImage::EncodeToFile negative test case 6\n");
    unsigned int width = 256;
    unsigned int height = 512;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, PixmapImage::COLOR_DEPTH_DEFAULT, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    DALI_TEST_CHECK(!pPixmapImg->EncodeToFile("fi/le.jpg"));

    END_TEST;
}

/**
 * @brief encoding PixmapImage into file with name containing '\0'
 *
 * @procedure
 * @step 1 creating PixmapImage
 * @step 2 check is the PixmapImage successfully created
 * @step 3 encoding the PixmapImage to file with incorrect name("fi.jpg\0le.jpg")
 *
 * @Passcondition: if the method return false.
 * @id DA-111
 */
int UtcDaliPixmapImageEncodeToFile_n_07(void)
{
    tet_printf("PixmapImage::EncodeToFile negative test case 7\n");
    unsigned int width = 256;
    unsigned int height = 512;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, PixmapImage::COLOR_DEPTH_DEFAULT, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    std::string fileName("fi.jpgGle.jpg");
    fileName[6] = '\0';
    tet_printf("%s\n", fileName.c_str());
    DALI_TEST_CHECK(!pPixmapImg->EncodeToFile(fileName));

    END_TEST;
}

/**
 * @brief encoding PixmapImage into file with too long name
 *
 * @procedure
 * @step 1 creating PixmapImage
 * @step 2 check is the PixmapImage successfully created
 * @step 3 encoding the PixmapImage to file with too long name(1024)
 *
 * @Passcondition: if the method return false.
 * @id DA-111
 */
int UtcDaliPixmapImageEncodeToFile_n_08(void)
{
    tet_printf("PixmapImage::EncodeToFile negative test case 8\n");
    unsigned int width = 256;
    unsigned int height = 512;

    Dali::PixmapImagePtr pPixmapImg = Dali::PixmapImage::New(width, height, PixmapImage::COLOR_DEPTH_DEFAULT, Adaptor::Get());
    DALI_TEST_CHECK(pPixmapImg);

    char* buff = new char[1024];
    memset(buff, 'a', 1024 * sizeof(char));
    buff[1023] = '\0';
    buff[1019] = '.';
    buff[1020] = 'j';
    buff[1021] = 'p';
    buff[1022] = 'g';

    DALI_TEST_CHECK(!pPixmapImg->EncodeToFile(buff));

    END_TEST;
}

