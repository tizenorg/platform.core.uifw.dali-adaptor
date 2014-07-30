#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <dali/dali.h>
#include <dali-test-suite-utils.h>
#include <Ecore_X.h>
#include <dali/public-api/math/vector2.h>

using namespace Dali;

static Dali::Adaptor* adaptor;

void utc_dali_device_layout_startup(void)
{
    ecore_x_init(NULL);
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Layout Application");
    adaptor = &Dali::Adaptor::New(window);

    test_return_value = TET_UNDEF;
}

void utc_dali_device_layout_cleanup(void)
{
    delete adaptor;
    ecore_x_shutdown();

    test_return_value = TET_PASS;
}

/**
 * @brief Device layout constructor.
 *
 * @procedure
 * @step 1 create instance of DeviceLayout using default constructor
 * @step 2 check the pointer to the instance
 *
 * @Passcondition: Handler is initialized.
 * @id DA-067
 */
int UtcDaliDeviceLayoutDeviceLayout_p_01(void)
{
    tet_printf("DeviceLayout default constructor called\n");
    DeviceLayout* dl = new DeviceLayout();
    DALI_TEST_CHECK(dl);
    delete dl;
    END_TEST;
}

/**
 * @brief Device layout constructor vith parameters.
 *
 * @procedure
 * @step 1 create instance of DeviceLayout using constructor with params
 * @step 2 check the pointer to the instance
 *
 * @Passcondition: Handler is initialized.
 * @id DA-068
 */
int UtcDaliDeviceLayoutDeviceLayout_p_02(void)
{
    tet_printf("DeviceLayout constructor with parameters called\n");
    DeviceLayout* dl = new DeviceLayout(Vector2(720.0f, 1280.0f), 4.65f, Vector2(316.0f, 316.0f), 35.0f);
    DALI_TEST_CHECK(dl);
    delete dl;
    END_TEST;
}
