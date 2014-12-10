# Command to auto-generate JNI header files.
# This doesn't run automatically during the build, because it uses the
# intermediate files from the Java build.  To use it:
# In Eclipse, build the project to generate the class files
# <path to android-ndk>/ndk-build jni-headers
jniRoot  := jni/jniGlue/headers
javaRoot := src/com/sec/daliviews
jni-headers : \
    $(jniRoot)/AdapterView-jni-gen.h \
    $(jniRoot)/Attrs-jni-gen.h \
    $(jniRoot)/ContainerViewBase-jni-gen.h \
    $(jniRoot)/IconView-jni-gen.h \
    $(jniRoot)/IconWidgetView-jni-gen.h \
    $(jniRoot)/ImageView-jni-gen.h \
    $(jniRoot)/ImageMeshView-jni-gen.h \
    $(jniRoot)/Item-jni-gen.h \
    $(jniRoot)/ListAdapter-jni-gen.h \
    $(jniRoot)/NativeApp-jni-gen.h \
    $(jniRoot)/NativeViewBase-jni-gen.h \
    $(jniRoot)/PageView-jni-gen.h \
    $(jniRoot)/PeerBase-jni-gen.h \
    $(jniRoot)/TextView-jni-gen.h \
    $(jniRoot)/TextureView-jni-gen.h \
    $(jniRoot)/ContainerView-jni-gen.h \
    $(jniRoot)/LayoutBase-jni-gen.h \
    $(jniRoot)/LinearLayout-jni-gen.h \
    $(jniRoot)/GridLayout-jni-gen.h \
    $(jniRoot)/CardLayout-jni-gen.h \
    $(jniRoot)/FrameLayout-jni-gen.h \
    $(jniRoot)/AbsoluteLayout-jni-gen.h \
    $(jniRoot)/PageLayout-jni-gen.h \
    $(jniRoot)/MagazineLayout-jni-gen.h \
    $(jniRoot)/DragAndDrop-jni-gen.h \
    $(jniRoot)/DragState-jni-gen.h \
    $(jniRoot)/UIAnimationSet-jni-gen.h \
    $(jniRoot)/UIAnimationPlayer-jni-gen.h \
    $(jniRoot)/PageIndicator-jni-gen.h \
    $(jniRoot)/UIAnimationDefs-jni-gen.h \
    $(jniRoot)/FixedLayout-jni-gen.h \
    $(jniRoot)/Button-jni-gen.h \
    $(jniRoot)/CheckBox-jni-gen.h \
    $(jniRoot)/Adapter-jni-gen.h \
    $(jniRoot)/ResizeFrame-jni-gen.h \
    $(jniRoot)/ResizeFrameBorder-jni-gen.h \
    $(jniRoot)/ResizeFrameHandle-jni-gen.h \
    $(jniRoot)/ResourceConnector-jni-gen.h \
    $(jniRoot)/StatefulDrawable-jni-gen.h \
    $(jniRoot)/ClippedContainerView-jni-gen.h \
    $(jniRoot)/CardViewContainer-jni-gen.h \
    $(jniRoot)/PagedAdapterView-jni-gen.h \
    $(jniRoot)/PagedListAdapter-jni-gen.h \
    $(jniRoot)/PageViewContainer-jni-gen.h \
    $(jniRoot)/MiniPageView-jni-gen.h \
    $(jniRoot)/MiniPageLayout-jni-gen.h \
    $(jniRoot)/FixedPageView-jni-gen.h \
    $(jniRoot)/AnimView-jni-gen.h \
    $(jniRoot)/ViewRecycler-jni-gen.h \
    $(jniRoot)/NativeAnimation-jni-gen.h \
    $(jniRoot)/TranslateAnimation-jni-gen.h \
    $(jniRoot)/RotateAnimation-jni-gen.h \
    $(jniRoot)/ScaleAnimation-jni-gen.h \
    $(jniRoot)/AlphaAnimation-jni-gen.h \
    $(jniRoot)/ColorAnimation-jni-gen.h \
    $(jniRoot)/NativeEffect-jni-gen.h \
    $(jniRoot)/ShadowEffect-jni-gen.h \
    $(jniRoot)/GlowEffect-jni-gen.h \
    $(jniRoot)/NativeEventHitlist-jni-gen.h \
    $(jniRoot)/ModedView-jni-gen.h \

$(jniRoot)/AdapterView-jni-gen.h : $(javaRoot)/views/AdapterView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.AdapterView
$(jniRoot)/Attrs-jni-gen.h : $(javaRoot)/layouts/Attrs.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.layouts.Attrs
$(jniRoot)/ContainerViewBase-jni-gen.h : $(javaRoot)/views/ContainerViewBase.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.ContainerViewBase
$(jniRoot)/IconView-jni-gen.h : $(javaRoot)/views/IconView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.IconView
$(jniRoot)/IconWidgetView-jni-gen.h : $(javaRoot)/views/IconWidgetView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.IconWidgetView
$(jniRoot)/ImageView-jni-gen.h : $(javaRoot)/views/ImageView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.ImageView
$(jniRoot)/ImageMeshView-jni-gen.h : $(javaRoot)/views/ImageMeshView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.ImageMeshView
$(jniRoot)/Item-jni-gen.h : $(javaRoot)/views/Item.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.Item
$(jniRoot)/ListAdapter-jni-gen.h : $(javaRoot)/views/ListAdapter.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.ListAdapter
$(jniRoot)/NativeApp-jni-gen.h : $(javaRoot)/views/NativeApp.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.NativeApp
$(jniRoot)/NativeViewBase-jni-gen.h : $(javaRoot)/views/NativeViewBase.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.NativeViewBase
$(jniRoot)/PeerBase-jni-gen.h : $(javaRoot)/views/PeerBase.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.PeerBase
$(jniRoot)/TextView-jni-gen.h : $(javaRoot)/views/TextView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.TextView
$(jniRoot)/PageView-jni-gen.h : $(javaRoot)/views/PageView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.PageView
$(jniRoot)/TextureView-jni-gen.h : $(javaRoot)/views/TextureView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.TextureView
$(jniRoot)/ContainerView-jni-gen.h : $(javaRoot)/views/ContainerView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.ContainerView
$(jniRoot)/ClippedContainerView-jni-gen.h : $(javaRoot)/views/ClippedContainerView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.ClippedContainerView
$(jniRoot)/CardViewContainer-jni-gen.h : $(javaRoot)/views/CardViewContainer.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.CardViewContainer
$(jniRoot)/LayoutBase-jni-gen.h : $(javaRoot)/layouts/LayoutBase.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.layouts.LayoutBase
$(jniRoot)/LinearLayout-jni-gen.h : $(javaRoot)/layouts/LinearLayout.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.layouts.LinearLayout
$(jniRoot)/GridLayout-jni-gen.h : $(javaRoot)/layouts/GridLayout.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.layouts.GridLayout
$(jniRoot)/CardLayout-jni-gen.h : $(javaRoot)/layouts/CardLayout.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.layouts.CardLayout
$(jniRoot)/FrameLayout-jni-gen.h : $(javaRoot)/layouts/FrameLayout.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.layouts.FrameLayout
$(jniRoot)/AbsoluteLayout-jni-gen.h : $(javaRoot)/layouts/AbsoluteLayout.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.layouts.AbsoluteLayout
$(jniRoot)/FixedLayout-jni-gen.h : $(javaRoot)/layouts/FixedLayout.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.layouts.FixedLayout
$(jniRoot)/PageLayout-jni-gen.h : $(javaRoot)/layouts/PageLayout.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.layouts.PageLayout
$(jniRoot)/MagazineLayout-jni-gen.h : $(javaRoot)/layouts/MagazineLayout.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.layouts.MagazineLayout
$(jniRoot)/DragAndDrop-jni-gen.h : $(javaRoot)/dragAndDrop/DragAndDrop.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.dragAndDrop.DragAndDrop
$(jniRoot)/DragState-jni-gen.h : $(javaRoot)/dragAndDrop/DragState.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.dragAndDrop.DragState
$(jniRoot)/UIAnimationPlayer-jni-gen.h : $(javaRoot)/uiAnimations/UIAnimationPlayer.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.uiAnimations.UIAnimationPlayer
$(jniRoot)/UIAnimationSet-jni-gen.h : $(javaRoot)/uiAnimations/UIAnimationSet.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.uiAnimations.UIAnimationSet
$(jniRoot)/PageIndicator-jni-gen.h : $(javaRoot)/views/PageIndicator.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.PageIndicator
$(jniRoot)/UIAnimationDefs-jni-gen.h : $(javaRoot)/uiAnimations/UIAnimationDefs.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.uiAnimations.UIAnimationDefs
$(jniRoot)/Button-jni-gen.h : $(javaRoot)/views/Button.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.Button
$(jniRoot)/CheckBox-jni-gen.h : $(javaRoot)/views/CheckBox.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.CheckBox
$(jniRoot)/Adapter-jni-gen.h : $(javaRoot)/views/Adapter.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.Adapter
$(jniRoot)/ResizeFrame-jni-gen.h : $(javaRoot)/views/ResizeFrame.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.ResizeFrame
$(jniRoot)/ResizeFrameBorder-jni-gen.h : $(javaRoot)/views/ResizeFrameBorder.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.ResizeFrameBorder
$(jniRoot)/ResizeFrameHandle-jni-gen.h : $(javaRoot)/views/ResizeFrameHandle.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.ResizeFrameHandle
$(jniRoot)/ResourceConnector-jni-gen.h : $(javaRoot)/resource/ResourceConnector.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.resource.ResourceConnector
$(jniRoot)/StatefulDrawable-jni-gen.h : $(javaRoot)/views/StatefulDrawable.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.StatefulDrawable
$(jniRoot)/PagedAdapterView-jni-gen.h : $(javaRoot)/views/PagedAdapterView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.PagedAdapterView
$(jniRoot)/PagedListAdapter-jni-gen.h : $(javaRoot)/views/PagedListAdapter.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.PagedListAdapter
$(jniRoot)/PageViewContainer-jni-gen.h : $(javaRoot)/views/PageViewContainer.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.PageViewContainer
$(jniRoot)/MiniPageView-jni-gen.h : $(javaRoot)/views/MiniPageView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.MiniPageView
$(jniRoot)/MiniPageLayout-jni-gen.h : $(javaRoot)/layouts/MiniPageLayout.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.layouts.MiniPageLayout
$(jniRoot)/FixedPageView-jni-gen.h : $(javaRoot)/views/FixedPageView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.FixedPageView
$(jniRoot)/AnimView-jni-gen.h : $(javaRoot)/views/AnimView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.AnimView
$(jniRoot)/ViewRecycler-jni-gen.h : $(javaRoot)/views/ViewRecycler.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.ViewRecycler
$(jniRoot)/NativeAnimation-jni-gen.h : $(javaRoot)/animation/NativeAnimation.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.animation.NativeAnimation
$(jniRoot)/TranslateAnimation-jni-gen.h : $(javaRoot)/animation/TranslateAnimation.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.animation.TranslateAnimation
$(jniRoot)/RotateAnimation-jni-gen.h : $(javaRoot)/animation/RotateAnimation.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.animation.RotateAnimation
$(jniRoot)/ScaleAnimation-jni-gen.h : $(javaRoot)/animation/ScaleAnimation.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.animation.ScaleAnimation
$(jniRoot)/ColorAnimation-jni-gen.h : $(javaRoot)/animation/ColorAnimation.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.animation.ColorAnimation
$(jniRoot)/AlphaAnimation-jni-gen.h : $(javaRoot)/animation/AlphaAnimation.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.animation.AlphaAnimation
$(jniRoot)/NativeEffect-jni-gen.h : $(javaRoot)/effects/NativeEffect.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.effects.NativeEffect
$(jniRoot)/ShadowEffect-jni-gen.h : $(javaRoot)/effects/ShadowEffect.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.effects.ShadowEffect
$(jniRoot)/GlowEffect-jni-gen.h : $(javaRoot)/effects/GlowEffect.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.effects.GlowEffect
$(jniRoot)/NativeEventHitlist-jni-gen.h : $(javaRoot)/events/NativeEventHitlist.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.events.NativeEventHitlist
$(jniRoot)/ModedView-jni-gen.h : $(javaRoot)/views/ModedView.java
	javah -force -o $@ -classpath bin/classes:${CLASSPATH} com.sec.daliviews.views.ModedView