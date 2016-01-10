#include "Shared.h"

/*
 * String class
 * 
 * A replacement for the std::string class.
 * 
 * 
 * 
 * 
 * 
 * 
 */

void String::Initialize (const char *str, int realLen)
{
	if (str == null)
	{
		_array = null;
		_count = 0;
		return;
	}

	int len = (realLen == -1) ? strlen(str) : realLen;

	_array = new char[len + 1];
	memcpy(_array, str, len);
	_array[len] = '\0';
	_count = len + 1;
}

void String::Destroy ()
{
	if (_count == 0)
		return;

	delete[] _array;
	_array = null;
	_count = 0;
}

String::String()
{
	Initialize(null);
}

String::String(nullptr_t)
{
	Initialize(null);
}

String::String(const char *str, int len)
{
	Initialize(str, len);
}

String::String (const String &str)
{
	Initialize(str._array);
}

String::~String()
{
	Destroy();
}

void String::Clear ()
{
	Destroy();
	Initialize(null);
}

String String::Clone () const
{
	return String(*this);
}

String &String::Concatenate (const char *str, int length)
{
	if (_count == 0 || _array == null)
	{
		Initialize(str, length);
		return *this;
	}

	int len = (length == -1) ? strlen(str) : length;

	int newCount = Count() + len + 1;

	char *oldArray = _array;
	_array = new char[newCount];
	memcpy (_array, oldArray, _count - 1);
	memcpy(_array + _count - 1, str, len);
	_array[newCount - 1] = '\0';
	delete[] oldArray;
	_count = newCount;

	return *this;
}

String &String::Concatenate (const String &str)
{
	return Concatenate(str.CString(), str.Count());
}

String &String::Remove (const int startIndex, const int length)
{
	int realLength = length;

	if (realLength == -1)
		realLength = Count() - startIndex;

	if (startIndex >= (int)Count() || startIndex < 0)
		throw Exceptions::IndexOutOfRange("startIndex");
	if ((startIndex + realLength) > (int)Count() || realLength < 0)
		throw Exceptions::IndexOutOfRange("length");

	int newCount = Count() - realLength;

	char *oldArray = _array;
	_array = new char[newCount + 1];
	char *left = oldArray;
	int leftCount = startIndex;

	char *right = oldArray + startIndex + realLength;
	int rightCount = (_count - 1) - startIndex - realLength;

	memcpy(_array, left, leftCount);
	memcpy(_array + leftCount, right, rightCount);
	_array[newCount] = 0;

	_count = newCount + 1;

	delete[] oldArray;

	return *this;
}

String String::Substring (const int startIndex, const int length) const
{
	int newStringLength = length;

	if (length == -1)
		newStringLength = Count() - startIndex;

	if (startIndex >= (int)Count() || startIndex < 0)
		throw Exceptions::IndexOutOfRange("startIndex");
	if ((startIndex + newStringLength) > (int)Count() || newStringLength < 0)
		throw Exceptions::IndexOutOfRange("length");

	String newStr;
	newStr._array = new char[newStringLength + 1];
	memcpy(newStr._array, _array + startIndex, newStringLength);
	newStr._array[newStringLength] = 0;
	newStr._count = newStringLength + 1;

	return newStr;
}

int String::Compare (const char *str, int maxLength) const
{
	if (maxLength == -1)
		maxLength = (((int)strlen(str) > Count()) ? Count() : (int)strlen(str)) + 1;

	return strncmp(_array, str, maxLength);
}

int String::CompareCaseInsensitive (const char *str, int maxLength) const
{
	if (maxLength == -1)
		maxLength = (((int)strlen(str) > Count()) ? Count() : (int)strlen(str)) + 1;

	return strnicmp(_array, str, maxLength);
}

int String::Compare (const String &str, int maxLength) const
{
	if (maxLength == -1)
		maxLength = ((str.Count() > Count()) ? Count() : str.Count()) + 1;

	return strncmp(_array, str._array, maxLength);
}

int String::CompareCaseInsensitive (const String &str, int maxLength) const
{
	if (maxLength == -1)
		maxLength = ((str.Count() > Count()) ? Count() : str.Count()) + 1;

	return strnicmp(_array, str._array, maxLength);
}

// FIXME: insensitive versions
int String::IndexOf (char ch, const int offset) const
{
	if (offset > (int)Count() || offset < 0)
		throw Exceptions::IndexOutOfRange("offset");

	for (uint32 i = offset; i < Count(); ++i)
	{
		if (_array[i] == ch)
			return i;
	}

	return -1;
}

int String::IndexOf(const String &str, const int offset) const
{
	if (offset > (int)Count() || offset < 0)
		throw Exceptions::IndexOutOfRange("offset");

	for (uint32 i = offset; i < Count(); ++i)
	{
		if ((i + str.Count()) > Count())
			break;

		if (strncmp(_array + i, str.CString(), str.Count()) == 0)
			return i;
	}

	return -1;
}

int String::IndexOf(const char *str, const int offset) const
{
	if (offset > (int)Count() || offset < 0)
		throw Exceptions::IndexOutOfRange("offset");

	uint32 len = strlen(str);
	for (uint32 i = offset; i < Count(); ++i)
	{
		if ((i + len) > Count())
			break;

		if (strncmp(_array + i, str, len) == 0)
			return i;
	}

	return -1;
}

int String::LastIndexOf (const char ch, const int offset) const
{
	if (offset > (int)Count() || offset < 0)
		throw Exceptions::IndexOutOfRange("offset");

	for (int i = Count() - 1 - offset; i >= 0; --i)
	{
		if (_array[i] == ch)
			return i;
	}

	return -1;
}

int String::LastIndexOf(const String &str, const int offset) const
{
	if (offset > (int)Count() || offset < 0)
		throw Exceptions::IndexOutOfRange("offset");

	for (int i = Count() - 1 - offset; i >= 0; --i)
	{
		if ((i + str.Count()) > Count())
			continue;

		if (strncmp(_array + i, str.CString(), str.Count()) == 0)
			return i;
	}

	return -1;
}

int String::LastIndexOf(const char *str, const int offset) const
{
	if (offset > (int)Count() || offset < 0)
		throw Exceptions::IndexOutOfRange("offset");

	uint32 len = strlen(str);
	for (int i = Count() - 1 - offset; i >= 0; --i)
	{
		if ((i + len) > Count())
			continue;

		if (strncmp(_array + i, str, len) == 0)
			return i;
	}

	return -1;
}

// FIXME: insensitive versions
bool String::Contains (const String &str) const
{
	return IndexOf(str) != -1;
}

bool String::Contains (const char *str) const
{
	return IndexOf(str) != -1;
}

// FIXME: insensitive versions
bool String::EndsWith (const String &str) const
{
	return (strncmp(str._array, (_array + Count()) - str.Count(), str.Count()) == 0);
}

bool String::EndsWith (const char *str) const
{
	return (strncmp(str, (_array + Count()) - strlen(str), strlen(str)) == 0);
}

// FIXME: insensitive versions
bool String::StartsWith (const String &str) const
{
	return (strncmp(str._array, _array, str.Count()) == 0);
}

bool String::StartsWith(const char *str) const
{
	return (strncmp(str, _array, strlen(str)) == 0);
}

String &String::Insert (const char *str, const int position)
{
	if (position >= (int)Count() || position < 0)
		throw Exceptions::IndexOutOfRange("position");

	int len = strlen(str);
	int newLength = _count + len;

	char *oldArray = _array;
	_array = new char[newLength];

	char *left = oldArray;
	int leftChars = position;

	char *right = oldArray + position;
	int rightChars = (_count - 1) - position;

	memcpy(_array, left, leftChars);
	memcpy(_array + position, str, len);
	memcpy(_array + len + position, right, rightChars);
	_count += len;
	_array[_count - 1] = 0;

	delete[] oldArray;
	return *this;
}

String &String::Insert (const String &str, const int position)
{
	if (position >= (int)Count() || position < 0)
		throw Exceptions::IndexOutOfRange("position");

	int newLength = _count + str.Count();

	char *oldArray = _array;
	_array = new char[newLength];

	char *left = oldArray;
	int leftChars = position;

	char *right = oldArray + position;
	int rightChars = (_count - 1) - position;

	memcpy(_array, left, leftChars);
	memcpy(_array + position, str._array, str.Count());
	memcpy(_array + str.Count() + position, right, rightChars);
	_count += str.Count();
	_array[_count - 1] = 0;

	delete[] oldArray;
	return *this;
}

String &String::Insert (char ch, const int position)
{
	char tempString[2] = { ch, 0 };
	return Insert(tempString, position);
}

/*static*/ String String::Format (const char *format, ...)
{
	static char tempBuffer[8192];
	va_list		argptr;

	va_start (argptr, format);
	vsnprintf (tempBuffer, sizeof(tempBuffer), format, argptr);
	va_end (argptr);

	tempBuffer[sizeof(tempBuffer)-1] = '\0';

	return String(tempBuffer);
}

bool String::IsNullOrEmpty () const
{
	return (_array == null || _count == 0);
}

/*static*/ bool String::IsNullOrEmpty(const String &str)
{
	return str.IsNullOrEmpty();
}

/*static*/ bool String::IsNullOrEmpty (const String *str)
{
	if (str == null)
		return true;

	return str->IsNullOrEmpty();
}

bool String::operator== (const String &right) const
{
	return Compare(right) == 0;
}

bool String::operator== (const char *right) const
{
	if (right == null)
		return false;

	return Compare(right) == 0;
}

bool String::operator!= (const String &right) const
{
	return !(*this == right);
}

bool String::operator!= (const char *right) const
{
	if (right == null)
		return false;

	return !(*this == right);
}

String &String::operator= (const String &r)
{
	if (&r == this)
		return *this;

	Destroy();

	if (r._count == 0)
		return *this;

	_count = r._count;

	_array = new char[_count];
	memcpy(_array, r._array, _count);

	return *this;
}

String &String::operator= (const char *r)
{
	if (r == _array)
		return *this;

	Destroy();
	
	if (strlen(r) == 0)
		return *this;

	_count = strlen(r) + 1;

	_array = new char[_count];
	memcpy(_array, r, _count - 1);
	_array[_count - 1] = 0;

	return *this;
}

String &String::operator+= (const String &r)
{
	return Concatenate(r);
}

String &String::operator+= (const char *r)
{
	return Concatenate(r);
}

String String::operator+ (const String &r) const
{
	return String(*this).Concatenate(r);
}

String String::operator+ (const char *r) const
{
	return String(*this).Concatenate(r);
}

String &String::operator+= (const char &ch)
{
	return Concatenate(&ch, 1);
}

String String::operator+ (const char &ch) const
{
	return String(*this).Concatenate(&ch, 1);
}

char &String::operator[] (const int &index) const
{
	return _array[index];
}

/*static*/ String String::Join(const char *separator, const String* list, int stringCount, int index, int count)
{
	String newStr;

	for (int j = index; ; j++)
	{
		if (count == -1)
		{
			if (&list[j] == null)
				break;
		}
		else
		{
			if (j > count)
				break;
		}

		if (j != index && strlen(separator) != 0)
			newStr.Concatenate(separator);

		newStr.Concatenate(list[j]);
	}

	return newStr;
}

/*static*/ String String::Join(const char *separator, const TList<String> &list, int index, int count)
{
	return Join(separator, list.Array(), list.Count(), index, count);
}

/*static*/ String String::Join (const String *list, int stringCount, int index, int count)
{
	return Join("", list, stringCount, index, count);
}

/*static*/ String String::Join (const TList<String> &list, int index, int count)
{
	return Join("", list.Array(), list.Count(), index, count);
}

String &String::Replace (const char from, const char to)
{
	for (uint32 i = 0; i < Count(); ++i)
	{
		if (_array[i] == from)
			_array[i] = to;
	}

	return *this;
}

String &String::Replace (const String &from, const String &to)
{
	int index = -(int)to.Count();
	while (true)
	{
		int v = IndexOf(from, index + to.Count());

		if (v == -1)
			break;

		Remove(v, from.Count());
		Insert(to, v);
	}

	return *this;
}

String &String::Replace (const char *from, const char *to)
{
	int toCount = strlen(to);
	int fromCount = strlen(from);

	int index = -toCount;
	while (true)
	{
		int v = IndexOf(from, index + toCount);

		if (v == -1)
			break;

		Remove(v, fromCount);
		Insert(to, v);
	}

	return *this;
}

bool String::IsNullOrWhiteSpace () const
{
	if (IsNullOrEmpty())
		return true;

	for (uint32 i = 0; i < Count(); ++i)
		if (!std::isspace(_array[i]))
			return false;

	return true;
}

TList<String> String::SplitWithCombiners (const char *characters, int charactersCount, bool removeEmpty, const char *combiners, int combinersCount)
{
	int index = 0;
	String buffer;
	TList<String> strings;

	while (true)
	{
		var ch = (uint32)index >= Count() ? 0 : operator[](index);
		int combiner = IsAny(ch, combiners, combinersCount);

		if (combiner != 0)
		{
			index++;
			if (!removeEmpty || buffer.Count() != 0)
			{
				strings.Add(buffer);
				buffer.Clear();
			}

			while (true)
			{
				if ((uint32)index >= Count())
					break;

				ch = operator[](index);

				if (ch == combiners[combiner - 1])
					break;

				buffer += ch;
				index++;
			}

			strings.Add(buffer);
			buffer.Clear();
		}
		else
		{
			if ((uint32)index == Count() || IsAny(ch, characters, charactersCount))
			{
				if (!removeEmpty || buffer.Count() != 0)
				{
					strings.Add(buffer);
					buffer.Clear();
				}
			}
			else
				buffer += ch;
		}

		if ((uint32)index >= Count())
			break;

		index++;
	}

	return strings;
}

TList<String> String::Split (const char *characters, int count, bool removeEmpty) const
{
	TList<String> split;

	int lastGot = 0;
	uint32 i = 0;
	while (true)
	{
		if ((i >= Count()) || IsAny(_array[i], characters, count))
		{
			if (((i < Count()) ? lastGot != (int)(i - lastGot) : true) &&
				(i - lastGot) != 0)
			{
				split.Add(Substring(lastGot, i - lastGot));
				lastGot = i + 1;
			}
		}

		if (i >= Count())
			break;

		++i;
	}

	return split;
}

TList<String> String::Split (const TList<char> &characters, bool removeEmpty) const
{
	return Split(characters.Array(), (int)characters.Count(), removeEmpty);
}

const char defaultSplitChars[] = {' ', '\t', '\r', '\n'};
TList<String> String::Split (bool removeEmpty) const
{
	return Split(defaultSplitChars, 4, removeEmpty);
}

TList<char> String::ToCharArray () const
{
	return TList<char> (_array, Count());
}

String &String::ToLower ()
{
	for (uint32 i = 0; i < Count(); ++i)
		_array[i] = std::tolower(_array[i]);

	return *this;
}

String &String::ToUpper ()
{
	for (uint32 i = 0; i < Count(); ++i)
		_array[i] = std::toupper(_array[i]);

	return *this;
}

String &String::Trim (int forceStart, int forceEnd)
{
	if (IsNullOrWhiteSpace())
	{
		Clear();
		return *this;
	}

	int whiteStart = forceStart;
	int whiteEnd = forceEnd;

	if (whiteStart == -1)
		whiteStart = 0;
	if (whiteEnd == -1)
		whiteEnd = Count() - 1;

	if (forceStart == -1 && std::isspace(_array[whiteStart]))
	{
		for (uint32 i = 0; i < Count(); ++i)
		{
			if (!std::isspace(_array[i]))
			{
				whiteStart = i;
				break;
			}
		}
	}

	if (forceEnd == -1 && std::isspace(_array[whiteEnd]))
	{
		for (int i = Count() - 1; i >= 0; --i)
		{
			if (!std::isspace(_array[i]))
			{
				whiteEnd = i;
				break;
			}
		}
	}

	char *oldArray = _array;
	int newCount = whiteEnd - whiteStart + 1;
	_array = new char[newCount + 1];
	memcpy (_array, oldArray + whiteStart, newCount);
	_array[newCount] = 0;
	_count = newCount + 1;
	delete[] oldArray;

	return *this;
}

String &String::TrimStart ()
{
	return Trim(-1, Count() - 1);
}

String &String::TrimEnd ()
{
	return Trim(0, -1);
}

uint32 String::Count() const
{
	return (_count == 0) ? 0 : (_count - 1);
}

char String::At (int index) const
{
	if (index >= (int)Count() || index < 0)
		throw Exceptions::IndexOutOfRange("index");

	return _array[index];
}

const char *String::CString() const
{
	return _array;
}

static const String EmptyString ("");
/*static*/ const String &String::Empty()
{
	return EmptyString;
}
 
bool String::MatchesWildcard(const String &pattern)
{
	enum State {
		Exact,      	// exact match
		Any,        	// ?
		AnyRepeat    	// *
	};

	const char *s = CString();
	const char *p = pattern.CString();
	const char *q = 0;
	int state = 0;

	bool match = true;
	while (match && *p) {
		if (*p == '*') {
			state = AnyRepeat;
			q = p+1;
		} else if (*p == '?') state = Any;
		else state = Exact;

		if (*s == 0) break;

		switch (state) {
		case Exact:
			match = *s == *p;
			s++;
			p++;
			break;

		case Any:
			match = true;
			s++;
			p++;
			break;

		case AnyRepeat:
			match = true;
			s++;

			if (*s == *q) p++;
			break;
		}
	}

	if (state == AnyRepeat) return (*s == *q);
	else if (state == Any) return (*s == *p);
	else return match && (*s == *p);
} 