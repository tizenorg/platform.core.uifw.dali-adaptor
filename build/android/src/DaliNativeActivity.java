package com.sec.dalidemo;

import android.app.Activity;
import android.os.Bundle;
import android.util.DisplayMetrics;

public class DaliNativeActivity extends android.app.NativeActivity 
{
	@Override
	public void onCreate(Bundle savedInstanceState) 
	{
	    super.onCreate(savedInstanceState);

	    DisplayMetrics dm = new DisplayMetrics();
	    getWindowManager().getDefaultDisplay().getMetrics(dm);

	    test_setDisplayConfig(dm.xdpi, dm.ydpi, dm.density);
	}

	private native void test_setDisplayConfig(float xdpi, float ydpi, float displayscale);

    static 
    {
      System.loadLibrary("vsync");
      System.loadLibrary("graphicbufferwrapper");
      System.loadLibrary("daliapp");
    }
 }