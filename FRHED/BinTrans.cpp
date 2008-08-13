#include "precomp.h"
#include "hexwnd.h"
#include "Simparr.h"
#include "BinTrans.h"

//-------------------------------------------------------------------
char Text2BinTranslator::cTranslateAnsiToOem( char c )
{
	char sbuf[2], dbuf[2];
	sbuf[0]=c;
	sbuf[1]=0;
	CharToOemBuff( sbuf, dbuf, 1 );
	return dbuf[0];
}

//-------------------------------------------------------------------
// Find char c from pointer to array src on, return it's position.
int Text2BinTranslator::iFindBytePos( char* src, char c )
{
	int i=0;
	while (src[i] != c)
		i++;
	return i;
}

//-------------------------------------------------------------------
// Create translation of bytecode-string.
int Text2BinTranslator::GetTrans2Bin( SimpleArray<char>& sa, int charmode, int binmode )
{
	sa.ClearAll();

	int destlen = iLengthOfTransToBin( m_pT, m_nUpperBound );
	if (destlen > 0)
	{
		sa.SetSize( destlen );
		sa.ExpandToSize();
		iCreateBcTranslation( (char*) sa, m_pT, m_nUpperBound, charmode, binmode );
		return TRUE;
	}
	else
	{
		// Empty input string => don't allocate anything and return 0.
		return FALSE;
	}

	return TRUE;
}

//-------------------------------------------------------------------
// How long is a bytecode-string that encodes char-array src of length srclen?
// Return: Length of bytecode-string including zero-byte.
int Text2BinTranslator::iBytes2BytecodeDestLen( char* src, int srclen )
{
	int i, destlen = 1;
	for (i=0; i<srclen; i++)
	{
		if (src[i] == '<')
			destlen+=2; // Escapecode needed.
		else if( src[i] == '\\' )
			destlen+=2; // Escapecode needed.
		else if (src[i] >= 32 && src[i] < 127)
			destlen++; // Normal char.
		else if( src[i]==10 || src[i]==13 )
			destlen++; // LF/CR.
		else
			destlen+=7; // Escapecode needed: <bh:xx>
	}
	return destlen;
}

//-------------------------------------------------------------------
// Bytecode?
// Return = 0 if no bytecode
//		else = Length 1/2/4 if bytecode
int Text2BinTranslator::iIsBytecode( char* src, int len )
{
	int i=0;

	if (src[i] == '<')
	{
		if (i+1 < len)
		{
			switch (src[i+1])
			{
			case 'b': case 'w': case 'l': case 'f': case 'd':
				if (i+2 < len)
				{
					switch (src[i+2])
					{
						case 'd': case 'h': case 'l': case 'o':
							if (i+3 < len)
							{
								if (src[i+3] == ':')
								{
									int j,k;
									for (j=4; j < len; j++)
									{
										if (src[i+j] == '>')
											break;
									}
									if (j==4 || j==len)
										// No concluding ">" found.
										return FALSE;
									for (k=4; k<j; k++)
									{
										switch (src[i+2])
										{
										case 'd':
											if ((src[i+k]>='0' && src[i+k]<='9') || src[i+k]=='-')
												continue;
											else
												return FALSE; // Non-digit found.
											break;

										case 'h':
											if ((src[i+k]>='0' && src[i+k]<='9') ||
												(src[i+k]>='a' && src[i+k]<='f'))
												continue;
											else
												return FALSE; // Non-hex-digit.
											break;

										case 'o': case 'l': // float or double.
											if ((src[i+k]>='0' && src[i+k]<='9') || src[i+k]=='-' || src[i+k]=='.' || src[i+k]=='e' || src[i+k]=='E')
												continue;
											else
												return FALSE;
											break;
										}
									}
									switch (src[i+1])
									{
									default:
									case 'b': return 1;
									case 'w': return 2;
									case 'l': return 4;
									case 'f': return 4;
									case 'd': return 8;
									}
								}
								else
									return FALSE; // No ':'.
							}
							else
								return FALSE; // No space for ':'.
							break;

						default:
							return FALSE; // Wrong second option.
					}
				}
				else
					return FALSE; // No space for option 2.
				break;

			default:
				return FALSE; // Wrong first option.
				break;
			}
		}
		else
			return FALSE; // No space for option 1;
	}
	else
		return FALSE; // No '<'.
}

//-------------------------------------------------------------------
// Get value of *one* bytecode token.
// Return: value of code.
// bytecode must be checked before!!
int Text2BinTranslator::iTranslateOneBytecode( char* dest, char* src, int srclen, int binmode )
{
	int i, k=0;
	char buf[50];
	for (i=4; i<srclen; i++)
	{
		if (src[i]=='>')
			break;
		else
		{
			buf[k++] = src[i];
		}
	}
	buf[k] = 0;
	int value;
	float fvalue;
	double dvalue;
	switch (src[2]) // Get value from text.
	{
	case 'd':
		sscanf (buf, "%d", &value);
		break;

	case 'h':
		sscanf (buf, "%x", &value);
		break;

	case 'l':
		sscanf (buf, "%f", &fvalue);
		break;

	case 'o':
		sscanf (buf, "%lf", &dvalue);
		break;
	}

	if (binmode == LITTLEENDIAN_MODE)
	{
		switch (src[1])
		{
		case 'b':
			dest[0] = (char) value;
			break;

		case 'w':
			dest[0] = (char)(value & 0xff);
			dest[1] = (char)((value & 0xff00)>>8);
			break;

		case 'l':
			dest[0] = (char)(value & 0xff);
			dest[1] = (char)((value & 0xff00)>>8);
			dest[2] = (char)((value & 0xff0000)>>16);
			dest[3] = (char)((value & 0xff000000)>>24);
			break;

		case 'f':
			*((float*)dest) = fvalue;
			break;

		case 'd':
			*((double*)dest) = dvalue;
			break;
		}
	}
	else // BIGENDIAN_MODE
	{
		switch (src[1])
		{
		case 'b':
			dest[0] = (char) value;
			break;

		case 'w':
			dest[0] = HIBYTE (LOWORD (value));
			dest[1] = LOBYTE (LOWORD (value));
			break;

		case 'l':
			dest[0] = HIBYTE (HIWORD (value));
			dest[1] = LOBYTE (HIWORD (value));
			dest[2] = HIBYTE (LOWORD (value));
			dest[3] = LOBYTE (LOWORD (value));
			break;

		case 'f':
			{
				char* p = (char*) &fvalue;
				int i;
				for (i=0; i<4; i++)
				{
					dest[i] = p[3-i];
				}
			}
			break;

		case 'd':
			{
				char* p = (char*) &dvalue;
				int i;
				for (i=0; i<8; i++)
				{
					dest[i] = p[7-i];
				}
			}
			break;
		}
	}
	return value;
}

//-------------------------------------------------------------------
// Get length of translated array of bytes from text.
int Text2BinTranslator::iLengthOfTransToBin( char* src, int srclen )
{
	int i, destlen = 0, l, k;
	for (i=0; i<srclen; i++)
	{
		if ((l = iIsBytecode (&(src[i]), srclen-i)) == 0)
		{
			if (src[i] == '\\')
			{
				if (i+1 < srclen)
				{
					if (src[i+1] == '<')
					{
						// Code for "<" alone without decoding.
						destlen++;
						i++;
					}
					else if( src[i+1] == '\\' )
					{
						// Code for "\\".
						destlen++;
						i++;
					}
					else
					{
						destlen++;
					}
				}
				else
				{
					destlen++;
				}
			}
			else
			{
				destlen++;
			}
		}
		else
		{
			destlen += l;
			for (k=i; i<srclen; k++)
			{
				if (src[k]=='>')
					break;
			}
			i = k;
		}
	}
	return destlen;
}

//-------------------------------------------------------------------
// dest must be set to right length before calling.
int Text2BinTranslator::iCreateBcTranslation( char* dest, char* src, int srclen, int charmode, int binmode )
{
	int i, di=0, bclen;
	for (i=0; i<srclen; i++)
	{
		if ((bclen = iIsBytecode(&(src[i]), srclen-i)) > 0) // Get length of byte-code.
		{
			// Bytecode found.
			iTranslateOneBytecode (&(dest[di]), &(src[i]), srclen-i, binmode);
			di += bclen;
			i += iFindBytePos( &( src[i] ), '>' );
		}
		else // Normal character.
		{
			if (src[i] == '\\') // Special char "\<" or "\\"?
			{
				if (i+1 < srclen)
				{
					if (src[i+1] == '<')
					{
						dest[di++] = '<'; // Special char recognized.
						i++;
					}
					else if( src[i+1] == '\\' )
					{
						dest[di++] = '\\'; // Special char recognized.
						i++;
					}
					else
						dest[di++] = src[i]; // Unknown special char.
				}
				else
					dest[di++] = src[i]; // Not enough space for special char.
			}
			else
			{
				// No special char.
				switch (charmode)
				{
				case ANSI_SET:
					dest[di++] = src[i];
					break;

				case OEM_SET:
					dest[di++] = cTranslateAnsiToOem (src[i]);
					break;
				}
			}
		}
	}
	return di;
}

//-------------------------------------------------------------------
Text2BinTranslator::Text2BinTranslator( char* ps )
{
	// Create a Text2BinTranslator from a normal char array-string.
	m_nGrowBy = 64;
	Clear();
	SetToString( ps );
}

//-------------------------------------------------------------------
Text2BinTranslator::Text2BinTranslator()
{
	SimpleString::SimpleString();
}

//-------------------------------------------------------------------
int Text2BinTranslator::bCompareBin( Text2BinTranslator& tr2, int charmode, int binmode )
{
	SimpleArray<char> sa1, sa2;
	GetTrans2Bin( sa1, charmode, binmode );
	tr2.GetTrans2Bin( sa2, charmode, binmode );
	return ( sa1 == sa2 );
}
