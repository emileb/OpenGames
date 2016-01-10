class Path
{
public:
	/*
	============
	Com_DefaultExtension

	If there is no extnsion in 'path', suffix 'extension'.
	============
	*/
	static void DefaultExtension(String &path, const String extension)
	{
		if (path.IsNullOrEmpty())
			return;

		// If path doesn't have an .ext, append extension (extension should include the .)
		int lastSlash = path.LastIndexOf('/');

		if (lastSlash == -1)
			lastSlash = 0;

		if (path.IndexOf('.') == -1)
			// no extension
			path.Concatenate(extension);
	}

	/*
	============
	Com_FileBase
	============
	*/
	static String GetFileNameWithoutExtension(const String &str)
	{
		int lastSlash = str.LastIndexOf('/');

		if (lastSlash == -1)
			lastSlash = 0;
		else
			lastSlash++;

		int dot = str.LastIndexOf('.');

		if (dot == -1)
			dot = str.Count() - 1;

		return str.Substring(lastSlash, dot - lastSlash);
	}


	/*
	============
	Com_FileExtension
	============
	*/
	static String GetExtension(const String &str)
	{
		int dot = str.LastIndexOf('.');

		if (dot == -1)
			return String::Empty();

		return str.Substring(dot + 1);
	}

	/*
	============
	Com_FilePath

	Returns the path up to, but not including the last /
	============
	*/
	static String GetFilePath(const String &str)
	{
		int lastSlash = str.LastIndexOf('/');

		if (lastSlash == -1)
			return str;

		return str.Substring(0, lastSlash);
	}

	/*
	============
	Com_NormalizePath
	============
	*/
	static String NormalizePath(const String &str)
	{
		String string(str);

		// replace \\ with /
		string.Replace('\\', '/');

		if (string.StartsWith("./"))
			string.Remove(0, 2);

		int offset = 0;
		while (true)
		{
			int loc = string.IndexOf("../", offset);

			if (loc == -1)
				break;

			int countBeforeThis = 0;

			for (int i = loc + 1; i >= 0; --i)
				if (string[i] == '/')
					countBeforeThis++;

			if (countBeforeThis < 2)
			{
				offset = loc + 3;
				continue;
			}
		}

		return string;
	}


	/*
	============
	Com_SkipPath
	============
	*/
	static String GetFileName(const String &str)
	{	
		int lastSlash = str.LastIndexOf('/');

		if (lastSlash == -1)
			lastSlash = 0;
		else
			lastSlash++;

		return str.Substring(lastSlash);
	}

	/*
	============
	Com_StripExtension
	============
	*/
	static String StripExtension(const String &str)
	{
		int dot = str.LastIndexOf(".");

		if (dot == -1)
			return str;

		return str.Substring(0, dot);
	}
};