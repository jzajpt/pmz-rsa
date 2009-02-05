/*
 * Common/Base64.h - Funkce pro praci s Base64.
 *
 * Prakticka maturitni zkouska, 2005 - 2006
 * Implementace sifrovaci algoritmu RSA.
 * 
 * Copyright (c) 2005-2006 Jiri Zajpt, <jzajpt@blueberry.cz>
 */

#ifndef _COMMON_BASE64__H


char* Base64EncodeString( const char* originalString, size_t stringLength );
char* Base64DecodeString( const char* encodedString, size_t stringLength );


#endif // _COMMON_BASE64__H
