package com.beloko.idtech;

import java.io.FileWriter;
import java.io.PrintWriter;
import java.lang.Thread.UncaughtExceptionHandler;
import java.util.Date;

import android.util.Log;

public class CustomUncaughtExceptionHandler implements UncaughtExceptionHandler {

	
	private UncaughtExceptionHandler defaultUEH;
	String filename;
	
	public CustomUncaughtExceptionHandler(String name) {
		this.defaultUEH = Thread.getDefaultUncaughtExceptionHandler();
		filename = name;
	}
	
	@Override
	public void uncaughtException(Thread thread, Throwable tr) {
		String logMessage = String.format("CustomUncaughtExceptionHandler.uncaughtException: Thread %d Message %s", thread.getId(), tr.getMessage());

		Log.e("CRASH", logMessage);

		tr.printStackTrace();

		if (true) {
			PrintWriter printWriter = null;

			try {
				printWriter = new PrintWriter(new FileWriter("/sdcard/" + filename  + ".txt", true));

				logMessage = String.format("%s\r\n\r\nThread: %d\r\n\r\nMessage:\r\n\r\n%s\r\n\r\nStack Trace:\r\n\r\n%s",
						new Date(), 
						thread.getId(), 
						tr.getMessage(), 
						Log.getStackTraceString(tr));

				printWriter.print(logMessage);
				printWriter.print("\n\n---------------------------------------------------------------------------\n\n");
			}
			catch (Throwable tr2) {
			}
			finally {
				if (printWriter != null) {
					printWriter.close();
				}
			}
		}
		//re throw
		 defaultUEH.uncaughtException(thread, tr);
	}
}