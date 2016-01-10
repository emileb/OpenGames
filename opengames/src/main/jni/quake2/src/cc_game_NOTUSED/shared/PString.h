class String
{
private:
	char	*_array;
	uint32	_count; // NOTE: count includes \0

protected:
	void Initialize (const char *str, int len = -1);
	void Destroy ();

public:
	String();
	String(nullptr_t);
	String(const char *str, int len = -1);
	String(const String &str);
	~String();

	void Clear ();
	String Clone () const;
	String &Concatenate (const char *str, int length = -1);
	String &Concatenate (const String &str);
	String &Remove (const int startIndex, const int length = -1);
	String Substring (const int startIndex, const int length = -1) const;
	int Compare (const char *str, int maxLength = -1) const;
	int CompareCaseInsensitive (const char *str, int maxLength = -1) const;
	int Compare (const String &str, int maxLength = -1) const;
	int CompareCaseInsensitive (const String &str, int maxLength = -1) const;
	// FIXME: insensitive versions
	int IndexOf (char ch, const int offset = 0) const;
	int IndexOf(const String &str, const int offset = 0) const;
	int IndexOf(const char *str, const int offset = 0) const;
	int LastIndexOf (const char ch, const int offset = 0) const;
	int LastIndexOf(const String &str, const int offset = 0) const;
	int LastIndexOf(const char *str, const int offset = 0) const;
	// FIXME: insensitive versions
	bool Contains (const String &str) const;
	bool Contains (const char *str) const;
	// FIXME: insensitive versions
	bool EndsWith (const String &str) const;
	bool EndsWith (const char *str) const;
	// FIXME: insensitive versions
	bool StartsWith (const String &str) const;
	bool StartsWith(const char *str) const;
	String &Insert (const char *str, const int position);
	String &Insert (const String &str, const int position);
	String &Insert (char ch, const int position);
	static String Format (const char *format, ...);
	bool IsNullOrEmpty () const;
	static bool IsNullOrEmpty(const String &str);
	static bool IsNullOrEmpty (const String *str);
	bool operator== (const String &right) const;
	bool operator== (const char *right) const;
	bool operator!= (const String &right) const;
	bool operator!= (const char *right) const;
	String &operator= (const String &r);
	String &operator= (const char *r);
	String &operator+= (const String &r);
	String &operator+= (const char *r);
	String operator+ (const String &r) const;
	String operator+ (const char *r) const;
	String &operator+= (const char &ch);
	String operator+ (const char &ch) const;
	char &operator[] (const int &index) const;
	static String Join(const String *list, int stringCount, int index = 0, int count = -1);
	static String Join(const TList<String> &list, int index = 0, int count = -1);
	static String Join(const char *separator, const TList<String> &list, int index = 0, int count = -1);
	static String Join(const char *separator, const String* list, int stringCount, int index = 0, int count = -1);
	String &Replace (const char from, const char to);
	String &Replace (const String &from, const String &to);
	String &Replace (const char *from, const char *to);
	bool IsNullOrWhiteSpace () const;

	template <typename T>
	static int IsAny (const T &v, const char *list, int num)
	{
		for (int i = 0; i < num; ++i)
			if (v == list[i])
				return i + 1;

		return 0;
	}

	TList<String> Split (const char *characters, int count, bool removeEmpty) const;
	TList<String> Split (const TList<char> &characters, bool removeEmpty) const;
	TList<String> Split (bool removeEmpty = false) const;

	TList<String> SplitWithCombiners (const char *characters, int charactersCount, bool removeEmpty, const char *combiners, int combinersCount);

	TList<char> ToCharArray () const;
	String &ToLower ();
	String &ToUpper ();
	String &Trim (int forceStart = -1, int forceEnd = -1);
	String &TrimStart ();
	String &TrimEnd ();
	uint32 Count() const;
	char At (int index) const;
	const char *CString() const;

	static const String &Empty();

#ifndef NO_STD_STRING_SUPPORT
	inline operator std::string() const
	{
		return std::string(CString());
	}

	inline String (const std::string &string)
	{
		Initialize(string.c_str(), string.length());
	}
#endif

	bool MatchesWildcard(const String &pattern);
};

inline String operator+ (const char *left, const String &right)
{
	return String(left) + right;
}

inline bool operator<(
		const String& _Left,
		const String& _Right)
	{	// test if string < string
		return (_Left.Compare(_Right) < 0);
	}

inline bool operator>(
		const String& _Left,
		const String& _Right)
	{	// test if string < string
		return (_Left.Compare(_Right) > 0);
	}


template<>
struct HashProvider<String>
{
	int CalculateHash(const String &value)
	{
		return -1;
	}
};