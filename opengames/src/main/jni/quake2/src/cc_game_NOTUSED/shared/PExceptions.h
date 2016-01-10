/*
 * Exception class
 * 
 * Instead of using the std::Exception class in C++, we'll be using this.
 * 
 * Simply put, it just creates an exception with the message in it.
 * 
 */

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

class Exception
{
protected:
	const char *message;

public:
	Exception () :
	  message()
	  {
	  };

	Exception (const char *message) :
	  message(message)
	  {
	  };

	const char *Message ()
	{
		return message;
	}
};

inline const char *GetTempString(const char *fmt, ...)
{
	static char tempBuffer[8192];
	va_list		argptr;

	va_start (argptr, fmt);
	vsnprintf (tempBuffer, sizeof(tempBuffer), fmt, argptr);
	va_end (argptr);

	tempBuffer[sizeof(tempBuffer)-1] = '\0';

	return tempBuffer;	
}

namespace Exceptions
{  // Exceptions namespace, to make them easier to find

class IndexOutOfRange : public Exception
{
public:
	IndexOutOfRange() :
	  Exception("The specified index is out of range")
	  {
	  };

	IndexOutOfRange(const char *variable) :
	  Exception(GetTempString("The variable \"%s\" is out of range", variable))
	  {
	  };
};

class NotImplemented : public Exception
{
public:
	NotImplemented() :
	  Exception("The specified function/method has not been implemented yet.")
	  {
	  };
};

class Networking : public Exception
{
public:
	Networking(const char *message) :
	  Exception(message)
	  {
	  };
};

class ArgumentInvalid : public Exception
{
public:
	ArgumentInvalid() :
	  Exception("The argument is invalid in the specified state.")
	  {
	  };
};

} // end namespace Exception