/*
 * BigNum/BigNum.cc - Implementace tridy pro praci s libovolne velkymi cisly.
 *
 * Prakticka maturitni zkouska, 2005 - 2006
 * Implementace sifrovaci algoritmu RSA.
 * 
 * Copyright (c) 2005-2006 Jiri Zajpt, <jzajpt@blueberry.cz>
 */

#include <iostream>
#include "../Common/Types.h"
#include "Base64.h"


void
Base64EncodeAtom( char* data, size_t dataLength, char* encoded )
{
	if ( data == NULL || encoded == NULL || dataLength == 0 )
		return;

	uint32_t    buffer;
	static char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	buffer = data[ 0 ] << 16;
	if ( dataLength >= 2 )
		buffer |= data[ 1 ] << 8;
	if ( dataLength == 3 )
		buffer |= data[ 2 ];

	encoded[ 0 ] = base64Chars[ ( buffer >> 18 ) & 0x3F ];
	encoded[ 1 ] = base64Chars[ ( buffer >> 12 ) & 0x3F ];

	if ( dataLength == 2 )
	{
		encoded[ 2 ] = base64Chars[ ( buffer >> 6 ) & 0x3F ];
		encoded[ 3 ] = '=';
	}
	else if ( dataLength == 3 )
	{
		encoded[ 2 ] = base64Chars[ ( buffer >> 6 ) & 0x3F ];
		encoded[ 3 ] = base64Chars[ buffer & 0x3F ];
	}
	else
	{
		encoded[ 2 ] = '=';
		encoded[ 3 ] = '=';
	}
}


int
Base64CharToInt( char c )
{
	static char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int i;
	if ( c == '=' )
		return 0;
	for ( i = 0 ; i < 64 ; i++ )
	{
		if ( base64Chars[ i ] == c )
			return i;
	}
	return -1;
};


void
Base64DecodeAtom( char* data, char* decoded )
{
	if ( data == NULL || decoded == NULL )
		return;

	uint32_t buffer;

	buffer = Base64CharToInt( data[ 3 ] );
	buffer |= Base64CharToInt( data[ 2 ] ) << 6;
	buffer |= Base64CharToInt( data[ 1 ] ) << 12;
	buffer |= Base64CharToInt( data[ 0 ] ) << 18;


	decoded[ 0 ] = ( buffer >> 16 ) & 0xFF;
	decoded[ 1 ] = ( buffer >> 8 ) & 0xFF;
	decoded[ 2 ] = buffer & 0xFF;

}


char*
Base64EncodeString( const char* originalString, size_t stringLength )
{
	uint iterations = stringLength / 3;

	char* encodedString = new char[ ( iterations * 4 ) + 5 ];
	if ( !encodedString )
		return NULL;

	memset( encodedString, 0, ( iterations * 4 ) + 5 );
	
	int i;
	char* pIn = const_cast< char* >( originalString );
	char* pOut = encodedString;
	for ( i = 0 ; i < iterations ; i++ )
	{
		Base64EncodeAtom( pIn, 3, pOut );
		pIn += 3;
		pOut += 4;
	}

	uint rest = stringLength % 3;
	if ( rest != 0 )
	{
		Base64EncodeAtom( pIn, rest, pOut );
	}

	return encodedString;
}


char*
Base64DecodeString( const char* encodedString, size_t stringLength )
{
	uint iterations = stringLength / 4;

	char* decodedString = new char[ ( iterations * 3 ) + 1 ];
	if ( !decodedString )
		return NULL;

	decodedString[ ( iterations * 3 ) + 1 ] = 0;
	
	int i;
	char* pIn = const_cast< char* >( encodedString );
	char* pOut = decodedString;

	for ( i = 0 ; i < iterations ; i++ )
	{
		Base64DecodeAtom( pIn, pOut );
		pIn += 4;
		pOut += 3;
	}

	return decodedString;
}
