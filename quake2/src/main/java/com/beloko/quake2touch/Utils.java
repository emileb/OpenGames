package com.beloko.quake2touch;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class Utils {
	static String LOG = "Utils";



	static public void copyFile(InputStream in, OutputStream out) throws IOException {
		byte[] buffer = new byte[1024];
		int read;
		while((read = in.read(buffer)) != -1){
			out.write(buffer, 0, read);
		}
		out.close(); 
	}

	public static boolean checkFiles(String basePath, boolean demo)
	{
		File[] files = new File(basePath).listFiles();
		boolean ok=true;
		String[] demo_files = {"pak0.pak"};
		String[] full_files = {"pak0.pak"};

		String[] expected;

		if (demo)
			expected = demo_files;
		else
			expected = full_files;

		if (files!=null)
		{

			for (String e: expected)
			{
				boolean found=false;
				for (File f: files)
				{
					//Log.d(LOG,"check " + f.toString());
					if (f.toString().toLowerCase().endsWith(e))
						found = true;
				}
				if (!found) ok = false;
			}
		}
		else	
			ok =  false;

		return ok;
	}
}
