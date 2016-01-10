#ifndef WIN32
#include <limits.h>
#include <stdarg.h>
#include <zlib.h>
#endif

/**
\typedef	sint32 FileHandle

\brief	Defines an alias representing a file handle (internal).
**/
typedef sint32 FileHandle;

/**
\typedef	uint16 EFileOpMode

\brief	Defines an alias representing the file operation mode.
**/
typedef uint16 EFileOpMode;

/**
\enum	

\brief	Values that represent file operation modes. 
**/
enum
{
	FILEMODE_NONE			=	0, // Internally only

	FILEMODE_READ			=	BIT(0), // Open a file for reading
	FILEMODE_WRITE			=	BIT(1), // Open a file for writing (can be mixed with the reading)
	FILEMODE_APPEND			=	BIT(2), // Open a file for appending (cannot be mixed)

	FILEMODE_CREATE			=	BIT(3), // Will create the file if it doesn't exist
	FILEMODE_ASCII			=	BIT(4), // Open the file in ascii mode

	// GZ-related
	FILEMODE_GZ				=	BIT(5), // load with gz (de)compression
	FILEMODE_COMPRESS_NONE	=	BIT(6), // compression level 0
	FILEMODE_COMPRESS_LOW	=	BIT(7), // compression level 2
	FILEMODE_COMPRESS_MED	=	BIT(8), // compression level 5
	FILEMODE_COMPRESS_HIGH	=	BIT(9), // compression level 9
};

/**
\typedef	uint8 ESeekOrigin

\brief	Defines an alias representing the seek origin.
**/
typedef uint8 ESeekOrigin;

/**
\enum	

\brief	Values that represent seek origins (same as the C ones). 
**/
enum
{
	SEEKORIGIN_CUR = SEEK_CUR,
	SEEKORIGIN_SET = SEEK_SET,
	SEEKORIGIN_END = SEEK_END
};

/**
\typedef	long int filePos_t

\brief	Defines an alias representing a file position.
**/
typedef long int FilePos;

// directory searching

/**
\typedef	uint8 EDirSearchType

\brief	Defines an alias representing flags for directory searching.
**/
typedef uint8 EDirSearchType;

/**
\enum	

\brief	Values that represent flags for directory searching. 
**/
enum
{
	SFF_ARCH	= BIT(0),
	SFF_HIDDEN	= BIT(1),
	SFF_RDONLY	= BIT(2),
	SFF_SUBDIR	= BIT(3),
	SFF_SYSTEM	= BIT(4)
};

const int MAX_PATHNAME			= 128;	// Maximum path length
const int FS_MAX_FILE_INDICES	= 256;	// The max number of file indexes that can be loaded at one time

/**
\typedef	uint8 EFileType

\brief	Defines an alias representing type of a file.
**/
typedef uint8 EFileType;

/**
\enum	

\brief	Values that represent the types of files available to the file system.
		Currently only regular OS FILE handles and GZip file handles.
**/
enum
{
	FILE_REGULAR,		// Regular file
	FILE_GZ,			// gz-compressed file
};

/**
\class	CPathIndex

\brief	A single path index. 

\author	Paril
\date	25/05/2010

\todo	Is this class even required?
**/
class CPathIndex
{
public:
	String			pathName;	// Full path name

	/**
	\fn	CPathIndex ()
	
	\brief	Default constructor. 
	
	\author	Paril
	\date	25/05/2010
	**/
	CPathIndex () {}

	/**
	\fn	CPathIndex (String pathName)
	
	\brief	Constructor. 
	
	\author	Paril
	\date	25/05/2010
	
	\param	pathName	Full path name
	**/
	CPathIndex (String pathName) :
	  pathName(pathName)
	  {
	  };
};

/**
\typedef	std::vector<CPathIndex*> TPathListType

\brief	Defines an alias representing type of the path list.
**/
typedef std::vector<CPathIndex*> TPathListType;
extern TPathListType PathList;	// List of paths, implemented in cc_file_system.cpp

/**
\fn	void FS_Init (sint32 maxHandles)

\brief	Initializes the file system.

\author	Paril
\date	25/05/2010

\param	maxHandles	The maximum handles. 
**/
void FS_Init (sint32 maxHandles);

/**
\fn	void FS_RemovePath (const char *pathName)

\brief	Removes path 'pathName' from the list of paths.

\author	Paril
\date	25/05/2010

\param	pathName	Path name.
**/
void FS_RemovePath (const char *pathName);

/**
\fn	void FS_AddPath (const char *pathName)

\brief	Adds path 'pathName' to the list of paths. 

\author	Paril
\date	25/05/2010

\param	pathName	Path name. 
**/
void FS_AddPath (const char *pathName);

/**
\fn	void FS_ReorderPath (const char *pathName)

\brief	Re-orders path 'pathName' to the front of the list of paths

\author	Paril
\date	25/05/2010

\param	pathName	Path name. 
**/
void FS_ReorderPath (const char *pathName);

/**
\class	CFileHandleIndex

\brief	A single file handle index.

\author	Paril
\date	25/05/2010
**/
class CFileHandleIndex
{
public:
	FileHandle				handleIndex;	// Zero-based index of the handle
	String					name;	// The name (usually full file name)

	bool					inUse;	// True if this handle is currently loaded
	EFileType				fileType;	// Type of the file
	EFileOpMode				openMode;	// The operation mode

	union
	{
		FILE					*reg;
		gzFile					gz;
	} file;	// Union of the file type

	/**
	\fn	CFileHandleIndex()
	
	\brief	Default constructor. 
	
	\author	Paril
	\date	25/05/2010
	**/
	CFileHandleIndex() { }

	/**
	\fn	CFileHandleIndex(FileHandle &handleIndex)
	
	\brief	Constructor. 
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	handleIndex	Zero-based index of a file handle. 
	**/
	CFileHandleIndex(FileHandle &handleIndex) :
	  handleIndex(handleIndex)
	{
		Clear ();
	};

	/**
	\fn	inline bool IsRegular()
	
	\brief	Query if this file is a regular C file.
	
	\return	true if regular, false if not. 
	**/
	inline bool IsRegular() { return fileType == FILE_REGULAR; };

	/**
	\fn	inline bool IsGz()
	
	\brief	Query if this file is a GZip file. 
	
	\return	true if gz, false if not. 
	**/
	inline bool IsGz() { return fileType == FILE_GZ; };

	/**
	\fn	void Clear ()
	
	\brief	Clears this file handle to its blank/initial state. 
	**/
	void Clear ()
	{
		name.Clear();
		inUse = false;
		openMode = FILEMODE_NONE;
		file.reg = NULL;
	};
};

/**
\typedef	std::map<FileHandle, CFileHandleIndex> THandleIndexListType

\brief	Defines an alias representing type of the handle index list.
**/
typedef std::map<FileHandle, CFileHandleIndex> THandleIndexListType;

/**
\class	CFileHandleList

\brief	List of file handles.

\author	Paril
\date	25/05/2010
**/
class CFileHandleList
{
	FileHandle numHandlesAllocated;	// Number of handles allocated

	THandleIndexListType	OpenList,	// allocated free keys
							ClosedList;	// used keys

	/**
	\fn	THandleIndexListType::iterator Create ()
	
	\brief	Creates a new key and increases allocated handles.
			Returns the new key.
	
	\return	Iterator to the new key. 
	**/
	THandleIndexListType::iterator Create ();

	/**
	\fn	THandleIndexListType::iterator MoveToClosed (THandleIndexListType::iterator it)
	
	\brief	Moves the handle of iterator 'it' to the Closed list and returns the new iterator
	
	\author	Paril
	\date	25/05/2010
	
	\param	it	The iterator index that needs to be moved to the closed list. 
	
	\return	The iterator of the moved key. 
	**/
	THandleIndexListType::iterator MoveToClosed (THandleIndexListType::iterator it);

	/**
	\fn	THandleIndexListType::iterator MoveToOpen (THandleIndexListType::iterator it)
	
	\brief	Moves the handle of iterator 'it' to the Open list and returns the new iterator
	
	\author	Paril
	\date	25/05/2010
	
	\param	it	The iterator index that needs to be moved to the open list.
	
	\return	The iterator of the moved key. 
	**/
	THandleIndexListType::iterator MoveToOpen (THandleIndexListType::iterator it);

public:

	/**
	\fn	CFileHandleList (sint32 allocated = 0)
	
	\brief	Constructor. 
	
	\author	Paril
	\date	25/05/2010
	
	\param	allocated	Number of handles to create automatically. 
	**/
	CFileHandleList (sint32 allocated = 0);

	/**
	\fn	THandleIndexListType::iterator GetFreeHandle ()
	
	\brief	Gets a free handle. 
	
	\return	Returns either a free key in Open or
			creates a new key and returns it.
	**/
	THandleIndexListType::iterator GetFreeHandle ();

	/**
	\fn	CFileHandleIndex *GetHandle (FileHandle Key)
	
	\brief	Gets the CFileHandleIndex of a FileHandle 
	
	\author	Paril
	\date	25/05/2010
	
	\param	Key	The key. 
	
	\return	null if it fails, else the handle. 
	**/
	CFileHandleIndex *GetHandle (FileHandle Key);

	/**
	\fn	void PushFreeHandle (FileHandle Handle)
	
	\brief	Pushes a key back into the Open list.
			Use this when you're done with a key
	
	\author	Paril
	\date	25/05/2010
	
	\param	Handle	The handle. 
	**/
	void PushFreeHandle (FileHandle Handle);

	/**
	\fn	static FileHandle FS_GetFreeHandle (CFileHandleIndex **handle)
	
	\brief	Grabs a free handle 
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	handle	If non-null, the handle. 
	
	\return	. 
	**/
	static FileHandle FS_GetFreeHandle (CFileHandleIndex **handle);

	/**
	\fn	static CFileHandleIndex *FS_GetHandle (FileHandle &fileNum)
	
	\brief	Returns the handle pointer for handle "fileNum"
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	fileNum	The file number. 
	
	\return	null if it fails, else the handle. 
	**/
	static CFileHandleIndex *FS_GetHandle (FileHandle &fileNum);
};

/**
\class	CFile

\brief	A wrapper class for handling file streams. 

\author	Paril
\date	25/05/2010
**/
class CFile
{
	friend class CFileHandleList;
	friend void FS_Init (sint32 maxHandles);
	static class CFileHandleList *IndexList; // list of indexes; held statically here

	CFileHandleIndex	*Handle;	// The handle

	/**
	\fn	static void OutputError (const char *errorMsg)
	
	\brief	Output error. 
	
	\author	Paril
	\date	25/05/2010
	
	\param	errorMsg	Message describing the error.

	\todo	actually use this function more
	**/
	// Error management
	static void OutputError (const char *errorMsg)
	{
		CC_ASSERT_EXPR(0, errorMsg);
	}

	/**
	\fn	static const char *OpenModeFromEnum (EFileOpMode Mode)
	
	\brief	Returns a file mode from an EFileOpMode enum.
			Always returns in same order:
			[r/a][w][b][+][c]

	\author	Paril
	\date	25/05/2010
	
	\param	Mode	The mode. 
	
	\return	"ERR" if it fails, else a file mode string. 
	**/
	static const char *OpenModeFromEnum (EFileOpMode Mode)
	{
		static char mode[5];
		sint32 currentPos = 0;

		// Reset old mode
		mode[0] = mode[1] = mode[2] = mode[3] = mode[4] = 0;

		if (Mode & FILEMODE_APPEND)
		{
			// Shove the a
			mode[currentPos++] = 'a';

			// Shove the + if we want to write
			if (Mode & FILEMODE_WRITE)
				mode[currentPos++] = '+';

			// Are we binary?
			if (!(Mode & FILEMODE_ASCII))
				mode[currentPos++] = 'b';
		}
		else if (Mode & FILEMODE_READ)
		{
			// Calculate the characters
			// We wanted reading, so push that in there.
			if (Mode & FILEMODE_CREATE)
				mode[currentPos++] = 'w';
			else
				mode[currentPos++] = 'r';

			// Did we want writing?
			// If we must exist, we need the +
			// to get r+ (reading + writing and must exist)
			if (Mode & FILEMODE_WRITE)
				mode[currentPos++] = '+';

			// Are we binary?
			if (!(Mode & FILEMODE_ASCII))
				mode[currentPos++] = 'b';
		}
		// If we got here, we only wanted
		// to open a file for writing.
		else if (Mode & FILEMODE_WRITE)
		{
			// Error checking
			if (Mode & FILEMODE_APPEND)
			{
				CFile::OutputError ("Write and Append mixed\n");
				return "ERR";
			}

			if (Mode & FILEMODE_CREATE)
				mode[currentPos++] = 'w';
			else
			{
				mode[currentPos++] = 'r';
				mode[currentPos++] = '+';
			}

			// Are we binary?
			if (!(Mode & FILEMODE_ASCII))
				mode[currentPos++] = 'b';
		}

		if (Mode & FILEMODE_GZ)
		{
			if (Mode & FILEMODE_COMPRESS_NONE)
				mode[currentPos++] = '0';
			else if (Mode & FILEMODE_COMPRESS_LOW)
				mode[currentPos++] = '2';
			else if (Mode & FILEMODE_COMPRESS_MED)
				mode[currentPos++] = '5';
			else if (Mode & FILEMODE_COMPRESS_HIGH)
				mode[currentPos++] = '9';
			else
				mode[currentPos++] = '5';
		}

		return mode;
	}

public:
	/**
	\fn	CFile (const char *fileName, EFileOpMode Mode)
	
	\brief	Constructor. 
	
	\author	Paril
	\date	25/05/2010
	
	\param	fileName	Filename of the file. 
	\param	Mode		The file operation mode. 
	**/
	CFile (const char *fileName, EFileOpMode Mode) :
	  Handle (0)
	{
		Open (fileName, Mode);
	};

	/**
	\fn	virtual ~CFile ()
	
	\brief	Destructor.
			Frees the file if it is valid.
	
	\author	Paril
	\date	25/05/2010
	**/
	virtual ~CFile ()
	{
		if (!Handle)
			return;

		Close ();
	};

	/**
	\fn	void Open (const char *fileName, EFileOpMode Mode)
	
	\brief	Opens the given file. 
	
	\author	Paril
	\date	25/05/2010
	
	\param	fileName	Filename of the file. 
	\param	Mode		The file operation mode. 
	**/
	void Open (const char *fileName, EFileOpMode Mode)
	{
		const char *openMode = CFile::OpenModeFromEnum(Mode);

		if (CC_ASSERT_EXPR (strcmp(openMode,"ERR"), "Invalid file mode passed"))
			return;

		// Open up the file.
		void *fp = NULL;

		// Search each of the search paths
		for (TPathListType::iterator it = PathList.begin(); it < PathList.end(); ++it)
		{
			String newFileName;
			CPathIndex *Index = (*it);

			char slashCheck = Index->pathName[Index->pathName.Count()-1];
			if (slashCheck != '\\' && slashCheck != '/')
				newFileName = Index->pathName + "/" + fileName;
			else
				newFileName = Index->pathName + fileName;

			// Try opening it
			if (Mode & FILEMODE_GZ)
				fp = gzopen(newFileName.CString(), openMode);
			else
				fp = fopen(newFileName.CString(), openMode);

			if (fp != NULL)
				break; // We got it
		}

		// Absolute path?
		if (Mode & FILEMODE_GZ)
			fp = gzopen(fileName, openMode);
		else
			fp = fopen(fileName, openMode);

		// Did we open?
		if (!fp)
			return; // Nope

		// Yep
		// Allocate a free handle
		CFileHandleList::FS_GetFreeHandle(&Handle);

		if (Handle)
		{
			Handle->inUse = true;
			Handle->name = fileName;
			Handle->openMode = Mode;

			if (Mode & FILEMODE_GZ)
				Handle->fileType = FILE_GZ;
			else
				Handle->fileType = FILE_REGULAR;

			Handle->file.reg = (FILE*)fp;
		}
		else
		{
			if (Mode & FILEMODE_GZ)
				gzclose ((gzFile)fp);
			else
				fclose ((FILE*)fp);
		}
	};

	/**
	\fn	void Close ()
	
	\brief	Closes file. 
	**/
	void Close ()
	{
		if (!Handle)
			return;

		if (Handle->IsRegular())
			fclose(Handle->file.reg);
		else
			gzclose(Handle->file.gz);

		// Invalidate the handle
		IndexList->PushFreeHandle (Handle->handleIndex);
		Handle = NULL;
	};

	/**
	\fn	bool EndOfFile ()
	
	\brief	Queries if this file has reached EOF. 
	
	\return	true if EOF, else false. 
	**/
	bool EndOfFile ()
	{
		if (!Handle)
			return true;

		if (Handle->IsRegular())
			return !!feof(Handle->file.reg);
		return !!gzeof(Handle->file.gz);
	};

	/**
	\fn	static bool Exists (const char *fileName)
	
	\brief	Query if a file exists.
	
	\author	Paril
	\date	25/05/2010
	
	\param	fileName	Filename of the file. 
	
	\return	true if file exists, false if it does not. 
	**/
	static bool Exists (const char *fileName)
	{
		CFile File (fileName, FILEMODE_READ);

		if (!File.Valid())
			return false;
		return true;
	};

	/**
	\fn	void Write (const void *buffer, size_t size)
	
	\brief	Writes 'size' bytes from 'buffer' to this file
	
	\author	Paril
	\date	25/05/2010
	
	\param	buffer	The buffer. 
	\param	size	The size. 
	**/
	void Write (const void *buffer, size_t size)
	{
		if (!Handle)
			return;

		CC_ASSERT_EXPR ((Handle->openMode & FILEMODE_WRITE), "Tried to write on a read\n");

		size_t len;

		if (Handle->IsRegular())
			len = fwrite (buffer, size, 1, Handle->file.reg);
		else
			len = gzwrite (Handle->file.gz, buffer, size);
		
		//CC_ASSERT_EXPR (len == size, "Read size != wanted size");
	};

	/**
	\fn	template <typename TType> void Write (const void *buffer)
	
	\brief	Specialization for const void*
	
	\author	Paril
	\date	25/05/2010
	
	\param	buffer	The buffer. 
	**/
	template <typename TType>
	void Write (const void *buffer)
	{
		Write (buffer, sizeof(TType));
	};

	/**
	\fn	template <typename TType> void Write (const TType &Ref)
	
	\brief	Writes a primitive data type to the file.
	
	\author	Paril
	\date	25/05/2010
	
	\param	Ref	The reference to the primitive data type. 
	**/
	template <typename TType>
	void Write (const TType &Ref)
	{
		Write (&Ref, sizeof(TType));
	};

	/**
	\fn	void Write (const String &Ref)
	
	\brief	Writes an String to the file
	
	\author	Paril
	\date	25/05/2010
	
	\param	Ref	The string. 
	**/
	void Write (const String &Ref)
	{
		WriteString(Ref.CString());
	};

	/**
	\fn	void WriteString (const char *Str)
	
	\brief	Writes a C string to the file.
	
	\author	Paril
	\date	25/05/2010
	
	\param	Str	The string. 
	**/
	void WriteString (const char *Str)
	{
		sint32 Length = (Str) ? strlen(Str) + 1 : -1;

		Write<> (Length);

		if (Length > 1)
			Write (Str, Length);
	};

	/**
	\fn	template <typename TType> void WriteArray (TType *Array, size_t Length)
	
	\brief	Writes an array of 'TType' to the file.
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	Array	The array. 
	\param	Length			The length. 
	**/
	template <typename TType>
	void WriteArray (TType *Array, size_t Length)
	{
		Write (Array, sizeof(TType) * Length);
	};

	/**
	\fn	void Read (void *buffer, size_t size)
	
	\brief	Reads 'size' bytes into 'buffer'
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	buffer	If non-null, the buffer. 
	\param	size			The size. 
	**/
	void Read (void *buffer, size_t size)
	{
		if (!Handle)
			return;

		CC_ASSERT_EXPR ((Handle->openMode & FILEMODE_READ), "Tried to read on a write\n");

		size_t len;

		if (Handle->IsRegular())
			len = fread (buffer, 1, size, Handle->file.reg);
		else
			len = gzread (Handle->file.gz, buffer, size);

		//CC_ASSERT_EXPR (len == size, "Read size != wanted size");
	};

	/**
	\fn	template <typename TType> void Read (void *buffer)
	
	\brief	Specialization for void*
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	buffer	If non-null, the void * to read. 
	**/
	template <typename TType>
	void Read (void *buffer)
	{
		Read (buffer, sizeof(TType));
	};

	/**
	\fn	template <typename TType> TType Read ()
	
	\brief	Reads a primitive data type, and returns it.
	
	\author	Paril
	\date	25/05/2010
	
	\return	. 
	**/
	template <typename TType>
	TType Read ()
	{
		TType Val;
		Mem_Zero (&Val, sizeof(TType));

		Read (&Val, sizeof(TType));
		return Val;
	};

	/**
	\fn	char *ReadString (sint32 Tag)
	
	\brief	Reads a C string from a file allocated using a memory tag
	
	\author	Paril
	\date	25/05/2010
	
	\param	Tag	The memory tag to allocate the string memory under. 
	
	\return	null if it fails, else the string. 
	**/
	char *ReadString (sint32 Tag)
	{
		if (!Handle)
			return NULL;

		sint32 Length;
		Read (&Length, sizeof(Length));
		
		char *tempBuffer = NULL;
		if (Length > 1)
		{
			tempBuffer = QNew (Tag) char[Length];
			Read (tempBuffer, Length);
		}
		return tempBuffer;
	};

	/**
	\fn	String ReadString ()
	
	\brief	Reads a dynamic String from a file.
	
	\return	The String. 
	**/
	String ReadString ()
	{
		if (!Handle)
			return String::Empty();

		char *stringBuffer = ReadString(TAG_GENERIC);

		if (!stringBuffer)
			return String::Empty();

		String str (stringBuffer);
		QDelete[] stringBuffer;
		return str;
	};

	/**
	\fn	String ReadLine ()
	
	\brief	Reads an entire line from the file
	
	\return	The line. 
	**/
	String ReadLine ()
	{
		if (!Handle)
			return String::Empty();

		String tempStr;

		while (true)
		{
			char tempChar = Read<char> ();

			if (tempChar == '\n' || tempChar == '\0')
				break;

			tempStr += tempChar;
		};

		return tempStr;
	};

	/**
	\fn	void ReadLine (char *buf, size_t maxSize)
	
	\brief	Reads an entire line from a file with the max size 'maxSize' into 'buf'
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	buf	If non-null, the buffer. 
	\param	maxSize		Size of the maximum. 
	**/
	void ReadLine (char *buf, size_t maxSize)
	{
		if (!Handle)
			return;

		String line = ReadLine ();

		Q_snprintfz (buf, maxSize-1, "%s", line.CString());
		buf[maxSize-1] = 0;
	};

	/**
	\fn	template <typename TType> void ReadArray (TType *Array, size_t Length)
	
	\brief	Reads an array of 'Length' 'TType's and stores it in 'Array'
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	Array	If non-null, the array. 
	\param	Length			The length. 
	**/
	template <typename TType>
	void ReadArray (TType *Array, size_t Length)
	{
		if (Array == NULL)
		{
			TType *TArray = QNew (TAG_GENERIC) TType[Length];
			Read (TArray, sizeof(TType) * Length);
			QDelete[] TArray;
			return;
		}

		Read (Array, sizeof(TType) * Length);
	};

	/**
	\fn	void Seek (const ESeekOrigin seekOrigin, const size_t seekOffset)
	
	\brief	Moves the file position. 
	
	\author	Paril
	\date	25/05/2010
	
	\param	seekOrigin	The seek origin. 
	\param	seekOffset	The seek offset. 
	**/
	void Seek (const ESeekOrigin seekOrigin, const size_t seekOffset)
	{
		if (!Handle)
			return;

		if (Handle->IsRegular())
			fseek (Handle->file.reg, seekOffset, seekOrigin);
		else
			gzseek (Handle->file.gz, seekOffset, seekOrigin);
	};

	/**
	\fn	void Print (const char *fmt, ...)
	
	\brief	Prints to a file. 
	
	\author	Paril
	\date	25/05/2010
	
	\param	fmt	Describes the format to use. 
	**/
	void Print (const char *fmt, ...)
	{
		if (!Handle)
			return;

		va_list		argptr;
		CTempMemoryBlock text = CTempHunkSystem::Allocator.GetBlock(SHRT_MAX/2);

		va_start (argptr, fmt);
		vsnprintf (text.GetBuffer<char>(), text.GetSize() - 1, fmt, argptr);
		va_end (argptr);

		text.GetBuffer<char>()[text.GetSize() - 1] = 0;
		Write (text.GetBuffer<char>(), strlen(text.GetBuffer<char>()));
	};

	/**
	\fn	inline size_t Length ()
	
	\brief	Returns the length of the entire file. 
	
	\return	Length of the file. 
	**/
	inline size_t Length ()
	{
		if (!Handle)
			return 0;

		FilePos currentPos = Tell();

		Seek (SEEKORIGIN_END, 0);
		size_t len = Tell();
		Seek (SEEKORIGIN_SET, currentPos);

		return len;
	};

	/**
	\fn	inline FilePos Tell ()
	
	\brief	Queries the file position of this stream. 
	
	\return	Returns current position of the file. 
	**/
	inline FilePos Tell ()
	{
		if (!Handle)
			return -1;

		if (Handle->IsRegular())
			return ftell(Handle->file.reg);
		else
			return gztell(Handle->file.gz);
	};

	/**
	\fn	inline bool Valid ()
	
	\brief	Checks if the file is valid.
			An invalid file is not loaded.
	
	\return	true if it is valid, false if it is not. 
	**/
	inline bool Valid ()
	{
		return (Handle != NULL);
	};

	// Stream-like functionality
	CFile &operator<< (bool &val)
	{
		Write<bool> (val);
		return *this;
	};

	CFile &operator<< (sint16 &val)
	{
		Write<sint16> (val);
		return *this;
	};

	CFile &operator<< (uint16 &val)
	{
		Write<uint16> (val);
		return *this;
	};

	CFile &operator<< (sint32 &val)
	{
		Write<sint32> (val);
		return *this;
	};

	CFile &operator<< (uint32 &val)
	{
		Write<uint32> (val);
		return *this;
	};

	CFile &operator<< (long &val)
	{
		Write<long> (val);
		return *this;
	};

	CFile &operator<< (unsigned long &val)
	{
		Write<unsigned long> (val);
		return *this;
	};

	CFile &operator<< (float &val)
	{
		Write<float> (val);
		return *this;
	};

	CFile &operator<< (double &val)
	{
		Write<double> (val);
		return *this;
	};

	CFile &operator<< (long double &val)
	{
		Write<long double> (val);
		return *this;
	};

	CFile &operator<< (const void *val)
	{
		Write<void*> (val);
		return *this;
	};

	CFile &operator>> (bool &val)
	{
		val = Read<bool> ();
		return *this;
	};

	CFile &operator>> (sint16 &val)
	{
		val = Read<sint16> ();
		return *this;
	};

	CFile &operator>> (uint16 &val)
	{
		val = Read<uint16> ();
		return *this;
	};

	CFile &operator>> (sint32 &val)
	{
		val = Read<sint32> ();
		return *this;
	};

	CFile &operator>> (uint32 &val)
	{
		val = Read<uint32> ();
		return *this;
	};

	CFile &operator>> (long &val)
	{
		val = Read<long> ();
		return *this;
	};

	CFile &operator>> (unsigned long &val)
	{
		val = Read<unsigned long> ();
		return *this;
	};

	CFile &operator>> (float &val)
	{
		val = Read<float> ();
		return *this;
	};

	CFile &operator>> (double &val)
	{
		val = Read<double> ();
		return *this;
	};

	CFile &operator>> (long double &val)
	{
		val = Read<long double> ();
		return *this;
	};

	CFile &operator>> (void *&val)
	{
		val = Read<void*> ();
		return *this;
	};
};

inline CFile &operator<< (CFile &Stream, String &val)
{
	Stream.Write (val);
	return Stream;
};

inline CFile &operator>> (CFile &Stream, String &val)
{
	val = Stream.ReadString ();
	return Stream;
};

/**
\class	CFileBuffer

\brief	A class that holds a file buffer.

\author	Paril
\date	25/05/2010
**/
class CFileBuffer
{
	uint8 *Buffer;	// The buffer
	size_t BufSize;	// Size of the buffer

public:
	/**
	\fn	CFileBuffer (const char *FileName, bool Terminate)
	
	\brief	Constructor. 
	
	\author	Paril
	\date	25/05/2010
	
	\param	FileName	Filename of the file. 
	\param	Terminate	True if the string should be terminated. 
	**/
	CFileBuffer (const char *FileName, bool Terminate) :
	  Buffer(NULL),
	  BufSize(0)
	{
		Open (FileName, Terminate);
	};

	/**
	\fn	~CFileBuffer ()
	
	\brief	Destructor. 
			Frees the buffer if it is still there.
	
	\author	Paril
	\date	25/05/2010
	**/
	~CFileBuffer ()
	{
		Close ();
	};

	/**
	\fn	void Open (const char *FileName, bool Terminate)
	
	\brief	Opens a file buffer, optionally null-terminating it.
	
	\author	Paril
	\date	25/05/2010
	
	\param	FileName	Filename of the file. 
	\param	Terminate	True if the string should be terminated. 
	**/
	void Open (const char *FileName, bool Terminate)
	{
		CFile File (FileName, FILEMODE_READ);

		if (!File.Valid())
			return;

		size_t len = File.Length();

		size_t termLen = (Terminate) ? 2 : 0;
		Buffer = QNew (TAG_GENERIC) uint8[len + termLen];

		File.Read (Buffer, len);

		if (Terminate)
			strncpy((char *)Buffer+len, "\n\0", termLen);

		BufSize = len + termLen;
	};

	/**
	\fn	void Close ()
	
	\brief	Closes the file buffer. 
	**/
	void Close ()
	{
		if (Valid())
			QDelete[] Buffer;
	};

	/**
	\fn	template <typename type> inline type *GetBuffer ()
	
	\brief	Gets the buffer.
	
	\return	null if it fails, else the buffer. 
	**/
	template <typename type>
	inline type *GetBuffer ()
	{
		return (type*)Buffer;
	};

	/**
	\fn	inline size_t GetLength ()
	
	\brief	Gets the length of the buffer. 
	
	\return	The length. 
	**/
	inline size_t GetLength ()
	{
		return BufSize;
	};

	/**
	\fn	inline bool Valid ()
	
	\brief	Checks if buffer is valid.
			You can do this to check if the file loaded or not.
	
	\return	true if it succeeds, false if it fails. 
	**/
	inline bool Valid ()
	{
		return (Buffer != NULL);
	};
};

/**
\typedef	std::vector<String> TFindFilesType

\brief	Defines an alias representing the list used by CFindFiles.
**/
typedef std::vector<String> TFindFilesType;

/**
\class	CFindFilesCallback

\brief	Callback for CFindFiles.
	
\ref Utilities "File System - Find Files"

\author	Paril
\date	25/05/2010
**/
class CFindFilesCallback
{
public:
	/**
	\fn	virtual void Query (String &fileName)
	
	\brief	Query function; must be overriden to do anything.
			This is called for every file that is found.
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	fileName	Filename of the file. 
	**/
	virtual void Query (String &fileName) {};
};

/**
\class	CFindFiles

\brief	A class used to find files and store them in a list (or optionally use a callback class).

\ref Utilities "File System - Find Files"

\author	Paril
\date	25/05/2010
**/
class CFindFiles
{
public:
	String			Path;	// Path to find in
	String			Filter;	// The filter
	String			Extension;	// The extension
	bool			AddDir;	// true to add the full path to the found files
	bool			Recurse;	// true to process recursively, false to process locally to Path only
	TFindFilesType	Files;	// The files that were found

	/**
	\fn	CFindFiles ()
	
	\brief	Default constructor. 
	
	\author	Paril
	\date	25/05/2010
	**/
	CFindFiles () :
		Path(),
		Filter(),
		Extension(),
		AddDir(false),
		Recurse(false),
		Files()
		{
		};

	/**
	\fn	CFindFiles (char *Path, char *Filter, char *Extension, bool AddDir = true,
		bool Recurse = false)
	
	\brief	Constructor. 
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	Path		If non-null, the path to start finding in. 
	\param [in,out]	Filter		If non-null, the filter. 
	\param [in,out]	Extension	If non-null, the extension. 
	\param	AddDir				true to add the full path to the found files. 
	\param	Recurse				true to process recursively, false to process locally to Path only. 
	**/
	CFindFiles (char *Path, char *Filter, char *Extension, bool AddDir = true, bool Recurse = false) :
		Path(Path),
		Filter(Filter),
		Extension(Extension),
		AddDir(AddDir),
		Recurse(Recurse),
		Files()
		{
			FindFiles (NULL);
		};

	/**
	\fn	CFindFiles (CFindFilesCallback *CallBack, char *Path, char *Filter, char *Extension,
		bool AddDir = true, bool Recurse = false)
	
	\brief	Constructor. 
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	CallBack	If non-null, pointer to callback to use with the files. 
	\param [in,out]	Path		If non-null, the path to start finding in. 
	\param [in,out]	Filter		If non-null, the filter. 
	\param [in,out]	Extension	If non-null, the extension. 
	\param	AddDir				true to add the full path to the found files. 
	\param	Recurse				true to process recursively, false to process locally to Path only. 
	**/
	CFindFiles (CFindFilesCallback *CallBack, char *Path, char *Filter, char *Extension, bool AddDir = true, bool Recurse = false) :
		Path(Path),
		Filter(Filter),
		Extension(Extension),
		AddDir(AddDir),
		Recurse(Recurse),
		Files()
		{
			FindFiles (CallBack);
		};

	/**
	\fn	CFindFiles (CFindFilesCallback &CallBack, char *Path, char *Filter, char *Extension,
		bool AddDir = true, bool Recurse = false)
	
	\brief	Constructor. 
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	CallBack	Reference to callback to use with the files. 
	\param [in,out]	Path		If non-null, the path to start finding in. 
	\param [in,out]	Filter		If non-null, the filter. 
	\param [in,out]	Extension	If non-null, the extension. 
	\param	AddDir				true to add the full path to the found files. 
	\param	Recurse				true to process recursively, false to process locally to Path only. 
	**/
	CFindFiles (CFindFilesCallback &CallBack, char *Path, char *Filter, char *Extension, bool AddDir = true, bool Recurse = false) :
		Path(Path),
		Filter(Filter),
		Extension(Extension),
		AddDir(AddDir),
		Recurse(Recurse),
		Files()
		{
			FindFiles (&CallBack);
		};

	/**
	\fn	void FindFiles (CFindFilesCallback *Callback)
	
	\brief	Does the actual finding.
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	Callback	If non-null, the callback to call for each file. 
	**/
	void FindFiles (CFindFilesCallback *Callback);
};
