/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.beloko.opengames.quakegl;

import android.app.Activity;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

import com.beloko.opengames.AppSettings;
import com.beloko.opengames.CDAudioPlayer;
import com.beloko.opengames.GD;
import com.beloko.opengames.Utils;
import com.beloko.touchcontrols.CustomCommands;
import com.beloko.touchcontrols.TouchControlsSettings;

import java.io.File;


public class QuakeActivity extends Activity {

	QuakeView mQuakeView;

	static QuakeLib mQuakeLib;

	boolean mKeepScreenOn = true; 

	@Override protected void onCreate(Bundle icicle) {
		Log.i("QuakeActivity", "onCreate");
		super.onCreate(icicle);

		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);    	
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		AppSettings.setGame(GD.IDGame.Quake);//Always Quake
		AppSettings.reloadSettings(getApplication()); 
		
		if (CDAudioPlayer.checkHasAudioFiles(getIntent().getStringExtra("mod_dir")))
			CDAudioPlayer.initFiles(getIntent().getStringExtra("mod_dir"));
		
		String basePath = getIntent().getStringExtra("base_path");
		String args = getIntent().getStringExtra("args");
		
		
		
		if (mQuakeLib == null) {
			mQuakeLib = new QuakeLib();
		
		
		
			Utils.copyPNGAssets(this, AppSettings.graphicsDir);
			//mQuakeLib.setBasePath(basePath);
			mQuakeLib.setCachePath(basePath + "/cache/"); 
			mQuakeLib.setGraphicsBase(AppSettings.graphicsDir);  

			WifiManager wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);
			WifiInfo wifiInfo = wifiManager.getConnectionInfo();
			int ipAddress = wifiInfo.getIpAddress();
			mQuakeLib.ipAddr(ipAddress); 
			
		
		} 
		
		mQuakeLib.preInit();
		
		if (mKeepScreenOn) {
			getWindow().setFlags(
					WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
					WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		}
		QuakeLib engine =  new QuakeLib();
		TouchControlsSettings.setup(this, engine);
		TouchControlsSettings.loadSettings(this);
		TouchControlsSettings.sendToQuake();

		CustomCommands.setup(this, engine, getIntent().getStringExtra("main_qc"), getIntent().getStringExtra("mod_qc"));
		
	
		if (mQuakeView == null) {
			mQuakeView = new QuakeView(getApplication(),engine,this);
			mQuakeView.setArgs(args);
			mQuakeView.setQuakeLib(mQuakeLib);
		}
		setContentView(mQuakeView);
	}

	@Override protected void onPause() {
		super.onPause();
		if (mQuakeView != null) {
			mQuakeView.onPause();
		}
		CDAudioPlayer.onPause();
	}

	@Override protected void onResume() {
		super.onResume();
		if (mQuakeView != null) {
			mQuakeView.onResume();
		}
		CDAudioPlayer.onResume();
	}

	@Override protected void onDestroy() {
		super.onDestroy();
		if (mQuakeLib != null) {
		//  	mQuakeLib.quit();
		}
		//Restart does not work properly, KILL
		//Wait for any config files to be written
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		System.exit(0);
	}   


	private boolean fileExists(String s) {
		File f = new File(s);
		return f.exists();
	}

}
