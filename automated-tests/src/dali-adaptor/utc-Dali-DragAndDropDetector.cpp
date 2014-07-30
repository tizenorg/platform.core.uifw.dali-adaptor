#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <dali/dali.h>
#include <dali-test-suite-utils.h>
#include <Ecore_X.h>
#include <dali/public-api/math/vector2.h>

using namespace Dali;

static Dali::Window* window;
static Dali::Adaptor* adaptor;
static bool functionCallFlag;

void utc_dali_drag_and_drop_detector_startup(void)
{
    ecore_x_init(NULL);
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "Drag and drop application"));

    adaptor = &Dali::Adaptor::New(*window);

    test_return_value = TET_UNDEF;
}

void utc_dali_drag_and_drop_detector_cleanup(void)
{
    delete window;
    delete adaptor;
    ecore_x_shutdown();

    test_return_value = TET_PASS;
}

void OnCalled( DragAndDropDetector detector )
{
    functionCallFlag = true;
}

/**
 * @brief DragAndDropDetector constructor.
 *
 * @procedure
 * @step 1 create instance of DragAndDropDetector using default constructor
 * @step 2 check the pointer to the instance
 *
 * @Passcondition: Handler is uninitialized.
 * @id DA-070
 */
int UtcDaliDragAndDropDetectorDragAndDropDetector_p_01(void)
{
    tet_printf("DragAndDrop constructor called\n");
    DragAndDropDetector* ddd = new DragAndDropDetector();
    DALI_TEST_CHECK(!*ddd);
    delete ddd;
    END_TEST;
}

/**
 * @brief Returns the current position of the dragged object.
 *
 * @procedure
 * @step 1 create instance of DragAndDropDetector
 * @step 2 check the handler
 * @step 3 get the vector of coordinates
 * @step 4 check if these coordinates are correct
 *
 * @Passcondition: Coordinates are correct.
 * @id DA-072
 */
int UtcDaliDragAndDropDetectorGetCurrentScreenPosition_p_01(void)
{
    tet_printf("GetCurrentScreenPosition called\n");
    DragAndDropDetector ddd ( window->GetDragAndDropDetector() );
    DALI_TEST_CHECK( ddd );

    Vector2 v2 = ddd.GetCurrentScreenPosition();
    DALI_TEST_CHECK( (v2.x==0) && (v2.y==0) );
    END_TEST;
}

/**
 * @brief Returns the current position of the dragged object.
 *
 * @procedure
 * @step 1 create uninitialized instance of DragAndDropDetector
 * @step 2 get the vector of coordinates
 * @step 3 check if DaliException rised
 *
 * @Passcondition: DaliException rised.
 * @id DA-072
 */
int UtcDaliDragAndDropDetectorGetCurrentScreenPosition_n_01(void)
{
    tet_printf("GetCurrentScreenPosition called\n");
    bool testPass = false;
    DragAndDropDetector ddd;

    try
    {
        Vector2 v2 = ddd.GetCurrentScreenPosition();
        testPass = false;
    }
    catch(DaliException ex)
    {
        testPass = true;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief Get the string, that represents the dropped element.
 *
 * @procedure
 * @step 1 create instance of DragAndDropDetector
 * @step 2 check the handler
 * @step 3 get the representing string
 * @step 4 check if string is empty
 *
 * @Passcondition: String is empty.
 * @id DA-071
 */
int UtcDaliDragAndDropDetectorGetContent_p_01(void)
{
    tet_printf("GetContent test case\n");
    DragAndDropDetector ddd ( window->GetDragAndDropDetector() );
    DALI_TEST_CHECK( ddd );

    std::string str = ddd.GetContent();
    DALI_TEST_CHECK(str.empty());
    END_TEST;
}

/**
 * @brief Get the string, that represents the dropped element.
 *
 * @procedure
 * @step 1 create uninitialized instance of DragAndDropDetector
 * @step 2 get the representing string
 * @step 3 check if DaliException rised
 *
 * @Passcondition: DaliException rised.
 * @id DA-071
 */
int UtcDaliDragAndDropDetectorGetContent_n_01(void)
{
    tet_printf("GetContent test case\n");
    bool testPass = false;
    DragAndDropDetector ddd;

    try
    {
        std::string str = ddd.GetContent();
        testPass = false;
    }
    catch(DaliException ex)
    {
        testPass = true;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief This is emitted when a dragged object enters a DALi window.
 *
 * @procedure
 * @step 1 create instance of DragAndDropDetector
 * @step 2 check the handler
 * @step 3 connect to callback function OnCalled
 * @step 4 emit the signal
 * @step 5 disconnect the function
 * @step 6 check if function was called and no exception rised
 *
 * @Passcondition: Function was called and no exception rised.
 * @id DA-073
 */
int UtcDaliDragAndDropDetectorEnteredSignal_p_01(void)
{
    tet_printf("EnteredSignal test case\n");
    bool testPass = false;
    functionCallFlag = false;
    DragAndDropDetector ddd ( window->GetDragAndDropDetector() );
    DALI_TEST_CHECK( ddd );

    try
    {
        ddd.EnteredSignal().Connect( &OnCalled );
        ddd.EnteredSignal().Emit( ddd );
        ddd.EnteredSignal().Disconnect( &OnCalled );
        testPass = true;
    }
    catch(DaliException ex)
    {
        testPass = false;
    }
    DALI_TEST_CHECK(testPass && functionCallFlag);
    END_TEST;
}

/**
 * @brief Negative test case for callback function.
 *
 * @procedure
 * @step 1 create uninitialized instance of DragAndDropDetector
 * @step 2 call the function
 * @step 6 check if exception rised
 *
 * @Passcondition: Exception rised.
 * @id DA-073
 */
int UtcDaliDragAndDropDetectorEnteredSignal_n_01(void)
{
    tet_printf("EnteredSignal test case\n");
    bool testPass = false;
    DragAndDropDetector ddd;

    try
    {
        ddd.EnteredSignal();
        testPass = false;
    }
    catch(DaliException ex)
    {
        testPass = true;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief This is emitted when a dragged object leaves a DALi window.
 *
 * @procedure
 * @step 1 create instance of DragAndDropDetector
 * @step 2 check the handler
 * @step 3 connect to callback function OnCalled
 * @step 4 emit the signal
 * @step 5 disconnect the function
 * @step 6 check if function was called and no exception rised
 *
 * @Passcondition: Function was called and no exception rised.
 * @id DA-074
 */
int UtcDaliDragAndDropDetectorExitedSignal_p_01(void)
{
    tet_printf("ExitedSignal test case\n");
    bool testPass = false;
    functionCallFlag = false;
    DragAndDropDetector ddd ( window->GetDragAndDropDetector() );
    DALI_TEST_CHECK( ddd );

    try
    {
        ddd.ExitedSignal().Connect( &OnCalled );
        ddd.ExitedSignal().Emit( ddd );
        ddd.ExitedSignal().Disconnect( &OnCalled );
        testPass = true;
    }
    catch(DaliException ex)
    {
        testPass = false;
    }
    DALI_TEST_CHECK(testPass && functionCallFlag);
    END_TEST;
}

/**
 * @brief Negative test case for callback function.
 *
 * @procedure
 * @step 1 create uninitialized instance of DragAndDropDetector
 * @step 2 call the function
 * @step 6 check if exception rised
 *
 * @Passcondition: Exception rised.
 * @id DA-074
 */
int UtcDaliDragAndDropDetectorExitedSignal_n_01(void)
{
    tet_printf("ExitedSignal test case\n");
    bool testPass = false;
    DragAndDropDetector ddd;

    try
    {
        ddd.ExitedSignal();
        testPass = false;
    }
    catch(DaliException ex)
    {
        testPass = true;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief This is emitted when a dragged object is moved within the DALi window.
 *
 * @procedure
 * @step 1 create instance of DragAndDropDetector
 * @step 2 check the handler
 * @step 3 connect to callback function OnCalled
 * @step 4 emit the signal
 * @step 5 disconnect the function
 * @step 6 check if function was called and no exception rised
 *
 * @Passcondition: Function was called and no exception rised.
 * @id DA-075
 */
int UtcDaliDragAndDropDetectorMovedSignal_p_01(void)
{
    tet_printf("MovedSignal test case\n");
    bool testPass = false;
    functionCallFlag = false;
    DragAndDropDetector ddd ( window->GetDragAndDropDetector() );
    DALI_TEST_CHECK( ddd );

    try
    {
        ddd.MovedSignal().Connect( &OnCalled );
        ddd.MovedSignal().Emit( ddd );
        ddd.MovedSignal().Disconnect( &OnCalled );
        testPass = true;
    }
    catch(DaliException ex)
    {
        testPass = false;
    }
    DALI_TEST_CHECK(testPass && functionCallFlag);
    END_TEST;
}

/**
 * @brief Negative test case for callback function.
 *
 * @procedure
 * @step 1 create uninitialized instance of DragAndDropDetector
 * @step 2 call the function
 * @step 6 check if exception rised
 *
 * @Passcondition: Exception rised.
 * @id DA-075
 */
int UtcDaliDragAndDropDetectorMovedSignal_n_01(void)
{
    tet_printf("MovedSignal test case\n");
    bool testPass = false;
    DragAndDropDetector ddd;

    try
    {
        ddd.MovedSignal();
        testPass = false;
    }
    catch(DaliException ex)
    {
        testPass = true;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief This is emitted when a dragged object is dropped within a DALi window.
 *
 * @procedure
 * @step 1 create instance of DragAndDropDetector
 * @step 2 check the handler
 * @step 3 connect to callback function OnCalled
 * @step 4 emit the signal
 * @step 5 disconnect the function
 * @step 6 check if function was called and no exception rised
 *
 * @Passcondition: Function was called and no exception rised.
 * @id DA-076
 */
int UtcDaliDragAndDropDetectorDroppedSignal_p_01(void)
{
    tet_printf("DroppedSignal test case\n");
    bool testPass = false;
    functionCallFlag = false;
    DragAndDropDetector ddd ( window->GetDragAndDropDetector() );
    DALI_TEST_CHECK( ddd );

    try
    {
        ddd.DroppedSignal().Connect( &OnCalled );
        ddd.DroppedSignal().Emit( ddd );
        ddd.DroppedSignal().Disconnect( &OnCalled );
        testPass = true;
    }
    catch(DaliException ex)
    {
        testPass = false;
    }
    DALI_TEST_CHECK(testPass && functionCallFlag);
    END_TEST;
}

/**
 * @brief Negative test case for callback function.
 *
 * @procedure
 * @step 1 create uninitialized instance of DragAndDropDetector
 * @step 2 call the function
 * @step 6 check if exception rised
 *
 * @Passcondition: Exception rised.
 * @id DA-076
 */
int UtcDaliDragAndDropDetectorDroppedSignal_n_01(void)
{
    tet_printf("DroppedSignal test case\n");
    bool testPass = false;
    DragAndDropDetector ddd;

    try
    {
        ddd.DroppedSignal();
        testPass = false;
    }
    catch(DaliException ex)
    {
        testPass = true;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}
