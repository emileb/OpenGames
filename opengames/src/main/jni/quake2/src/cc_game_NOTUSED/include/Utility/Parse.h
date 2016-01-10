/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//
// parse.h
//

/*
==============================================================================

	PARSE SESSIONS
 
==============================================================================
*/

/**
\typedef	uint8 EParseFlags

\brief	Defines an alias representing parse flags.
**/
typedef uint8 EParseFlags;

/**
\enum	

\brief	Values that represent parse flags. 
**/
enum
{
	PSF_ALLOW_NEWLINES		= BIT(0),	// Allow token parsing to go onto the next line
	PSF_CONVERT_NEWLINE		= BIT(1),	// Convert newline characters in quoted tokens to their escape character
	PSF_TO_LOWER			= BIT(2),	// Lower-case the token before returning
	PSF_WARNINGS_AS_ERRORS	= BIT(3),	// Treat all warnings as errors
};

/**
\typedef	uint8 ESessionProperties

\brief	Defines an alias representing parse session properties.
**/
typedef uint8 ESessionProperties;

/**
\enum	

\brief	Values that represent parse session properties. 
**/
enum
{
	PSP_COMMENT_BLOCK	= BIT(0),		// Treat "/*" "*/" as block-comment marker
	PSP_COMMENT_LINE	= BIT(1),		// Treat "//" as a line-comment marker
	PSP_COMMENT_POUND	= BIT(2),		// Treat "#" as a line-comment marker

	PSP_COMMENT_MASK	= (PSP_COMMENT_BLOCK|PSP_COMMENT_LINE|PSP_COMMENT_POUND),
};

#include <climits>
// Data type stuff
template <typename TType> inline const char *PS_DataName () { return "<unknown>"; }
template <> inline const char *PS_DataName <sint8> () { return "<sint8>"; }
template <> inline const char *PS_DataName <uint8> () { return "<uint8>"; }
template <> inline const char *PS_DataName <sint16> () { return "<sint16>"; }
template <> inline const char *PS_DataName <uint16> () { return "<uint16>"; }
template <> inline const char *PS_DataName <sint32> () { return "<sint32>"; }
template <> inline const char *PS_DataName <uint32> () { return "<uint32>"; }
template <> inline const char *PS_DataName <long> () { return "<long>"; }
template <> inline const char *PS_DataName <unsigned long> () { return "<unsigned long>"; }
template <> inline const char *PS_DataName <float> () { return "<float>"; }
template <> inline const char *PS_DataName <double> () { return "<double>"; }
template <> inline const char *PS_DataName <bool> () { return "<bool>"; }
template <typename TType>
inline bool PS_VerifyVec (const char *token, void *target)
{
	CC_ASSERT_EXPR (0, "PS_VerifyVec called without type specialization");
	return false;
}

// Parse integral value
template <typename TType>
inline bool PS_VerifyVecI (const char *token, void *target, const TType MinValue, const TType MaxValue)
{
	uint32 i, len = strlen (token);

	for (i = 0; i < len; i++)
	{
		if (token[i] >= '0' || token[i] <= '9')
			continue;
		if (token[i] == '-' && i == 0)
			continue;
		break;
	}

	if (i != len)
		return false;

	TType temp = (TType)atol (token);
	if ((MinValue && (temp < MinValue)) || temp > MaxValue)
		return false;

	*(TType*)target = (TType)temp;
	return true;
}

template <>
inline bool PS_VerifyVec <sint8> (const char *token, void *target)
{
	return PS_VerifyVecI <sint8> (token, target, SCHAR_MIN, SCHAR_MAX);
}

template <>
inline bool PS_VerifyVec <uint8> (const char *token, void *target)
{
	return PS_VerifyVecI <uint8> (token, target, 0, CHAR_MAX);
}

template <>
inline bool PS_VerifyVec <sint16> (const char *token, void *target)
{
	return PS_VerifyVecI <sint16> (token, target, SHRT_MIN, SHRT_MAX);
}

template <>
inline bool PS_VerifyVec <uint16> (const char *token, void *target)
{
	return PS_VerifyVecI <uint16> (token, target, 0, USHRT_MAX);
}

template <>
inline bool PS_VerifyVec <sint32> (const char *token, void *target)
{
	return PS_VerifyVecI <sint32> (token, target, INT_MIN, INT_MAX);
}

template <>
inline bool PS_VerifyVec <uint32> (const char *token, void *target)
{
	return PS_VerifyVecI <uint32> (token, target, 0, UINT_MAX);
}

template <>
inline bool PS_VerifyVec <long> (const char *token, void *target)
{
	return PS_VerifyVecI <long> (token, target, LONG_MIN, LONG_MAX);
}

template <>
inline bool PS_VerifyVec <unsigned long> (const char *token, void *target)
{
	return PS_VerifyVecI <unsigned long> (token, target, 0, ULONG_MAX);
}

// Parse float
template <typename TType>
inline bool PS_VerifyVecF (const char *token, void *target)
{
	bool		dot = false;
	uint32		i, len = strlen (token);

	for (i = 0; i < len; i++)
	{
		if (token[i] >= '0' || token[i] <= '9')
			continue;
		
		if (token[i] == '.' && !dot)
		{
			dot = true;
			continue;
		}
		
		if (i == 0)
		{
			if (token[i] == '-')
				continue;
		}

		else if (i == (len-1) && (Q_tolower(token[i]) == 'f' || Q_tolower(token[i]) == 'd'))
			continue;

		break;
	}

	if (i != len)
		return false;

	*(TType*)target = (TType)atof (token);
	return true;
}

template <>
inline bool PS_VerifyVec <float> (const char *token, void *target)
{
	return PS_VerifyVecF <float> (token, target);
}

template <>
inline bool PS_VerifyVec <double> (const char *token, void *target)
{
	return PS_VerifyVecF <double> (token, target);
}

template <>
inline bool PS_VerifyVec <bool> (const char *token, void *target)
{
	if (!strcmp (token, "1") || !Q_stricmp (token, "true"))
	{
		*(bool*)target = true;
		return true;
	}

	if (!strcmp (token, "0") || !Q_stricmp (token, "false"))
	{
		*(bool*)target = false;
		return true;
	}

	return false;
}

class CParser
{
	String				ScratchToken;	// Used for temporary storage during data-type/post parsing

	uint32				CurrentColumn;
	uint32				CurrentLine;
	String				CurrentToken;

	const char			*DataPointer;
	const char			*DataPointerLast;

	uint32				NumErrors;
	uint32				NumWarnings;

	ESessionProperties	Properties;

public:

	CParser () :
	  ScratchToken(),
	  CurrentColumn(1),
	  CurrentLine(1),
	  CurrentToken(),
	  DataPointer(NULL),
	  DataPointerLast(NULL),
	  NumErrors(0),
	  NumWarnings(0),
	  Properties(0)
	  {
	  };

	CParser (const char *DataPointer, ESessionProperties Properties) :
	  ScratchToken(),
	  CurrentColumn(1),
	  CurrentLine(1),
	  DataPointer(DataPointer),
	  DataPointerLast(DataPointer),
	  NumErrors(0),
	  NumWarnings(0),
	  Properties(Properties),
	  CurrentToken()
	{
		// Make sure incoming data is valid
		if (!DataPointer)
			return;
	};

	~CParser ()
	{
	};

	inline uint32 GetErrorCount () const
	{
		return NumErrors;
	}

	inline uint32 GetWarningCount () const
	{
		return NumWarnings;
	}

	inline uint32 GetLine () const
	{
		return CurrentLine;
	}
	
	inline uint32 GetColumn () const
	{
		return CurrentColumn;
	}

	void Start (const char *DataPointer, ESessionProperties Properties)
	{
		CParser (DataPointer, Properties);
	};

	const String &GetCurrentToken () const
	{
		return CurrentToken;
	};

	inline bool IsEOF () const
	{
		return !DataPointer;
	};

	bool ParseToken (EParseFlags Flags, String &Target)
	{
		bool Valid = ParseToken(Flags, NULL);
		Target = GetCurrentToken();
		return Valid;
	};

	// A quick note:
	// ParseToken will give you a pointer to a CONST char.
	// DON'T CHANGE RETURNED TOKENS!
	// To get a copy, use GetCurrentToken ()
	bool ParseToken (EParseFlags flags, const char **target)
	{
		// Check if the incoming data offset is valid (see if we hit EOF last the last run)
		const char *data = DataPointer;

		if (!data)
		{
			AddError ("PARSE ERROR: called PS_ParseToken and already hit EOF\n");
			return false;
		}

		DataPointerLast = DataPointer;

		// Clear the current token
		CurrentToken.Clear ();
		char c = 0;

		while (true)
		{
			// Skip whitespace
			while ((c = *data) <= ' ')
			{
				switch (c)
				{
				case '\0':
					DataPointer = NULL;
					return false;

				case '\n':
					if (!(flags & PSF_ALLOW_NEWLINES))
					{
						DataPointer = data;
						if (CurrentToken.IsNullOrEmpty())
							return false;

						if (target)
							*target = CurrentToken.CString();
						return true;
					}

					CurrentColumn = 0;
					CurrentLine++;
					break;

				default:
					CurrentColumn++;
					break;
				}

				data++;
			}

			// Skip comments
			if (SkipComments (&data))
			{
				if (!data)
				{
					DataPointer = NULL;
					return false;
				}
			}
			else
				// No comment, don't skip anymore whitespace
				break;
		} 

		// Handle quoted strings specially
		// FIXME: PSP_QUOTES_TOKENED
		if (c == '\"')
		{
			CurrentColumn++;
			data++;

			while (true)
			{
				c = *data++;
				switch (c)
				{
				case '\0':
					CurrentColumn++;
					DataPointer = data;
					AddError ("PARSE ERROR: hit EOF while inside quotation\n");
					return false;

				case '\"':
					CurrentColumn++;
					DataPointer = data;

					// Empty token
					if (CurrentToken.IsNullOrEmpty())
						return false;

					if (flags & PSF_CONVERT_NEWLINE)
					{
						if (!ConvertEscape (flags))
							return false;
					}

					if (target)
						*target = CurrentToken.CString();
					return true;

				case '\n':
					if (!(flags & PSF_ALLOW_NEWLINES))
					{
						DataPointer = data;
						if (!CurrentToken[0])
							return false;

						if (target)
							*target = CurrentToken.CString();
						return true;
					}

					CurrentColumn = 0;
					CurrentLine++;
					break;

				default:
					CurrentColumn++;
					break;
				}

				if (flags & PSF_TO_LOWER)
					c = Q_tolower (c);

				CurrentToken += c;
			}
		}

		// Parse a regular word
		while (true)
		{
			if (c <= ' ' || c == '\"')	// FIXME: PSP_QUOTES_TOKENED
				break;	// Stop at spaces and quotation marks

			// Stop at opening comments
			if (Properties & PSP_COMMENT_MASK)
			{
				if (c == '#' && Properties & PSP_COMMENT_POUND)
					break;
				if (c == '/')
				{
					if (data[1] == '/' && Properties & PSP_COMMENT_LINE)
						break;
					if (data[1] == '*' && Properties & PSP_COMMENT_BLOCK)
						break;
				}

				if (c == '*' && data[1] == '/' && Properties & PSP_COMMENT_BLOCK)
				{
					DataPointer = data;
					AddError ("PARSE ERROR: end-comment '*/' with no opening\n");
					return false;
				}
			}

			if (flags & PSF_TO_LOWER)
				c = Q_tolower(c);

			// Store character
			CurrentToken += c;

			CurrentColumn++;
			c = *++data;
		}

		// Done
		DataPointer = data;

		// Empty token
		if (!CurrentToken[0])
			return false;

		if (flags & PSF_CONVERT_NEWLINE)
		{
			if (!ConvertEscape (flags))
				return false;
		}

		if (target)
			*target = CurrentToken.CString();
		return true;
	};
	
	void UndoParse ()
	{
		DataPointer = DataPointerLast;
	};

	void SkipLine ()
	{
		// Check if the incoming data offset is valid (see if we hit EOF last the last run)
		const char	*data = DataPointer;

		if (!data)
		{
			AddError ("PARSE ERROR: called PS_SkipLine and already hit EOF\n");
			return;
		}

		DataPointerLast = DataPointer;

		// Skip to the end of the line
		while (*data && *data != '\n')
		{
			data++;
			CurrentColumn++;
		}

		DataPointer = data;
	};

	template <typename TType>
	bool ParseDataType (EParseFlags flags, void *target, uint32 numVecs)
	{
		const char *token;
		// Parse the next token
		if (!ParseToken (flags, &token))
			return false;

		// Individual tokens
		// FIXME: support () [] {} brackets
		if (!strchr (token, ' ') && !strchr (token, ','))
		{
			for (uint32 i = 0; i < numVecs; i++)
			{
				if (i)
				{
					// Parse the next token
					if (!ParseToken (flags, &token))
					{
						AddError ("PARSE ERROR: vector missing parameters!\n");
						return false;
					}

					// Storage position
					target = (TType *)target+1;
				}

				// Check the data type
				if (!PS_VerifyVec<TType> (token, target))
				{
					AddError ("PARSE ERROR: does not evaluate to desired data type!\n");
					return false;
				}
			}

			return true;
		}

		// Single token with all vectors
		// FIXME: support () [] {} brackets
		const char *data = token;

		for (uint32 i = 0; i < numVecs; i++)
		{
			char c;

			ScratchToken.Clear ();
			size_t len = 0;

			// Skip white-space
			while (true)
			{
				c = *data++;

				if (c <= ' ' || c == ',')
					continue;
				
				if (c == '\0')
					break;
				
				break;
			}

			// Parse this token into a sub-token stored in ScratchToken
			while (true)
			{
				if (c <= ' ' || c == ',')
				{
					data--;
					break;	// Stop at white space and commas
				}

				ScratchToken += c;
				len++;
				c = *data++;
			}

			len++;

			// Too few vecs
			if (ScratchToken.IsNullOrEmpty())
			{
				AddError ("PARSE ERROR: missing vector parameters!\n");
				return false;
			}

			// Check the data type and set the target
			if (!PS_VerifyVec<TType> (ScratchToken.CString(), target))
			{
				AddError ("PARSE ERROR: '%s' does not evaluate to desired data type %s!\n", ScratchToken.CString(), PS_DataName<TType> ());
				return false;
			}

			// Next storage position
			target = (TType *)target+1;
		}

		// Check for too much data
		while (true)
		{
			char c = *data++;

			if (c == '\0')
				break;

			if (c > ' ')
			{
				AddError ("PARSE ERROR: too many vector parameters!\n");
				return false;
			}
		}

		return true;
	}

	// Error management
	void AddError (const char *errorMsg, ...)
	{
#ifdef _DEBUG
#ifdef WIN32
#if !defined(CC_STDC_CONFORMANCE)
		OutputDebugStringA (errorMsg);
#endif
		assert (0);
#endif
#endif
		DebugPrintf ("%s\n", errorMsg);
	}

	void AddWarning (const char *errorMsg, ...)
	{
#ifdef _DEBUG
#ifdef WIN32
#if !defined(CC_STDC_CONFORMANCE)
		OutputDebugStringA (errorMsg);
#endif
		assert (0);
#endif
#endif
		DebugPrintf ("%s\n", errorMsg);
	}

	// Private interface
private:

	bool SkipComments (const char **data)
	{
		// See if any comment types are allowed
		if (!(Properties & PSP_COMMENT_MASK))
			return false;

		const char *p = *data;

		switch (*p)
		{
		case '#':
			// Skip "# comments"
			if (Properties & PSP_COMMENT_POUND)
			{
				while (*p != '\n')
				{
					if (*p == '\0')
					{
						*data = NULL;
						return true;
					}

					p++;
					CurrentColumn++;
				}

				*data = p;
				return true;
			}
			break;

		case '*':
			// This shouldn't happen with proper commenting
			if (p[1] == '/' && (Properties & PSP_COMMENT_BLOCK))
			{
				p += 2;
				AddError ("PARSE ERROR: end-comment '*/' with no opening\n");
				*data = NULL;
				return true;
			}
			break;

		case '/':
			// Skip "// comments"
			if (p[1] == '/' && (Properties & PSP_COMMENT_LINE))
			{
				while (*p != '\n')
				{
					if (*p == '\0')
					{
						*data = NULL;
						return true;
					}

					p++;
					CurrentColumn++;
				}

				*data = p;
				return true;
			}

			// Skip "/* comments */"
			if (p[1] == '*' && (Properties & PSP_COMMENT_BLOCK))
			{
				// Skip initial "/*"
				p += 2;
				CurrentColumn += 2;

				// Skip everything until "*/"
				while (*p && (*p != '*' || p[1] != '/'))
				{
					if (*p == '\n')
					{
						CurrentColumn = 0;
						CurrentLine++;
					}
					else
						CurrentColumn++;

					p++;
				}

				// Skip the final "*/"
				if (*p == '*' && p[1] == '/')
				{
					p += 2;
					CurrentColumn += 2;
					*data = p;
					return true;
				}

				// Didn't find final "*/" (hit EOF)
				AddError ("PARSE ERROR: unclosed comment and hit EOF\n");
				*data = NULL;
				return true;
			}
			break;
		}

		// No comment block handled
		return false;
	}

	bool ConvertEscape (EParseFlags flags)
	{
		// If it's blank then why even try?
		if (CurrentToken.IsNullOrEmpty())
			return true;

		// Convert escape characters
		for (size_t i = 0; i < CurrentToken.Count(); i++)
		{
			if (CurrentToken[i] != '\\')
			{
				ScratchToken += CurrentToken[i];
				continue;
			}

			// Hit a '\'
			switch (CurrentToken[i + 1])
			{
			case 'n':
				if (flags & PSF_CONVERT_NEWLINE)
				{
					ScratchToken += '\n';
					i++;
					continue;
				}
				break;

			default:
				AddWarning ("PARSE WARNING: unknown escape character '%c%c', ignoring\n", CurrentToken[i], CurrentToken[i+1]);
				ScratchToken += CurrentToken[i++];
				ScratchToken += CurrentToken[i];
				break;
			}
		}

		// Copy scratch back to the current token
		CurrentToken = ScratchToken;
		return true;
	}
};

